/*
 * Copyright (C) 2021,2022 Dan Arrhenius <dan@ultramarin.se>
 *
 * This file is part of libultrabus.
 *
 * libultrabus is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <ultrabus/org_freedesktop_DBus_ObjectManager.hpp>
#include <ultrabus/org_freedesktop_DBus.hpp>
#include <ultrabus/dbus_array.hpp>
#include <typeinfo>
#include <sstream>


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static std::string make_iface_added_rule (const std::string& service,
                                              const std::string& opath)
    {
        std::stringstream rule;
        rule << "type='signal',sender='" << service << "',path='" << opath
             << "',interface='org.freedesktop.DBus.ObjectManager',member='InterfacesAdded'";
        return rule.str ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static std::string make_iface_removed_rule (const std::string& service,
                                                const std::string& opath)
    {
        std::stringstream rule;
        rule << "type='signal',sender='" << service << "',path='" << opath
             << "',interface='org.freedesktop.DBus.ObjectManager',member='InterfacesRemoved'";
        return rule.str ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    org_freedesktop_DBus_ObjectManager::org_freedesktop_DBus_ObjectManager (Connection& connection)
        : MessageHandler (connection)
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static retvalue<managed_objects_t> handle_get_managed_objects_result (Message& reply)
    {
        retvalue<managed_objects_t> retval;

        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        dbus_array dict;
        if (!reply.get_args(&dict, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }

        try {
            for (auto& entry : dict) {
                auto& de = dynamic_cast<dbus_dict_entry&> (entry);
                std::string opath = de.key().str ();
                auto& ifaces = dynamic_cast<dbus_array&> (de.value());

                std::map<std::string, Properties> iface_map;
                for (auto& iface : ifaces) {
                    auto& ie = dynamic_cast<dbus_dict_entry&> (iface);
                    iface_map.emplace (ie.key().str(), dynamic_cast<dbus_array&>(ie.value()));
                }
                retval.get().emplace (opath, iface_map);
            }
        }
        catch (std::bad_cast& bc) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<managed_objects_t> org_freedesktop_DBus_ObjectManager::get_managed_objects (
            const std::string& service,
            const std::string& object_path)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
        auto reply = conn.send_and_wait (msg);
        return handle_get_managed_objects_result (reply);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_ObjectManager::get_managed_objects (
            const std::string& service,
            const std::string& object_path,
            std::function<void (retvalue<managed_objects_t>& result)> callback)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
        if (!callback) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [callback](Message& reply)
                {
                    auto retval = handle_get_managed_objects_result (reply);
                    callback (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_ObjectManager::set_interfaces_added_callback (
            const std::string& service,
            const std::string& opath,
            iface_added_cb cb)
    {
        if (cb == nullptr)
            return remove_interfaces_added_callback (service, opath);

        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(opath.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service, [this, opath, cb](retvalue<std::string>& bus_name)
            {
                // Called from worker thread
                if (bus_name.err())
                    return;

                std::lock_guard<std::mutex> lock (iface_mutex);
                auto key = std::make_pair (bus_name.get(), opath);
                auto entry = iface_added_callbacks.find (key);
                if (entry != iface_added_callbacks.end()) {
                    entry->second = cb;
                }else{
                    iface_added_callbacks.emplace (key, cb);
                    // Add rule
                    add_match_rule (make_iface_added_rule(bus_name.get(), opath));
                }
            });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_ObjectManager::set_interfaces_removed_callback (
            const std::string& service,
            const std::string& opath,
            iface_removed_cb cb)
    {
        if (cb == nullptr)
            return remove_interfaces_removed_callback (service, opath);

        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(opath.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service, [this, opath, cb](retvalue<std::string>& bus_name)
            {
                // Called from worker thread
                if (bus_name.err())
                    return;

                std::lock_guard<std::mutex> lock (iface_mutex);
                auto key = std::make_pair (bus_name.get(), opath);
                auto entry = iface_removed_callbacks.find (key);
                if (entry != iface_removed_callbacks.end()) {
                    entry->second = cb;
                }else{
                    iface_removed_callbacks.emplace (key, cb);
                    // Add rule
                    add_match_rule (make_iface_removed_rule(bus_name.get(), opath));
                }
            });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_ObjectManager::remove_interfaces_added_callback (
            const std::string& service,
            const std::string& opath)
    {
        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(opath.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service, [this, opath](retvalue<std::string>& bus_name)
            {
                // Called from worker thread
                if (bus_name.err())
                    return;

                std::lock_guard<std::mutex> lock (iface_mutex);
                auto key = std::make_pair (bus_name.get(), opath);
                auto entry = iface_added_callbacks.find (key);
                if (entry != iface_added_callbacks.end()) {
                    iface_added_callbacks.erase (entry);
                    // Remove rule
                    remove_match_rule (make_iface_added_rule(bus_name.get(),
                                                             opath));
                }
            });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_ObjectManager::remove_interfaces_removed_callback (
            const std::string& service,
            const std::string& opath)
    {
        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(opath.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service, [this, opath](retvalue<std::string>& bus_name)
            {
                // Called from worker thread
                if (bus_name.err())
                    return;

                std::lock_guard<std::mutex> lock (iface_mutex);
                auto key = std::make_pair (bus_name.get(), opath);
                auto entry = iface_removed_callbacks.find (key);
                if (entry != iface_removed_callbacks.end()) {
                    iface_removed_callbacks.erase (entry);
                    // Remove rule
                    remove_match_rule (make_iface_removed_rule(bus_name.get(),
                                                               opath));
                }
            });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool org_freedesktop_DBus_ObjectManager::on_signal (Message& msg)
    {
        if (msg.interface() != "org.freedesktop.DBus.ObjectManager")
            return false;

        if (msg.name() == "InterfacesAdded") {
            std::lock_guard<std::mutex> lock (iface_mutex);
            auto key = std::make_pair (msg.sender(), msg.path());
            auto entry = iface_added_callbacks.find (key);
            if (entry != iface_added_callbacks.end()) {
                auto cb = entry->second;
                iface_mutex.unlock ();
                handle_added_ifaces (msg, cb);
            }
        }
        else if (msg.name() == "InterfacesRemoved") {
            std::lock_guard<std::mutex> lock (iface_mutex);
            auto key = std::make_pair (msg.sender(), msg.path());
            auto entry = iface_removed_callbacks.find (key);
            if (entry != iface_removed_callbacks.end()) {
                auto cb = entry->second;
                iface_mutex.unlock ();
                handle_removed_ifaces (msg, cb);
            }
        }

        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus_ObjectManager::handle_added_ifaces (Message& msg, iface_added_cb cb)
    {
        try {
            dbus_basic opath;
            dbus_array ifaces;
            if (!msg.get_args(&opath, &ifaces, nullptr))
                return;

            std::map<std::string, Properties> if_prop;
            for (auto& entry : ifaces) {
                auto& de = dynamic_cast<dbus_dict_entry&> (entry);
                if_prop.emplace (de.key().str(), Properties(de.value()));
            }

            // Call callback
            cb (opath.str(), if_prop);
        }
        catch (std::bad_cast& bc) {
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus_ObjectManager::handle_removed_ifaces (Message& msg, iface_removed_cb cb)
    {
        dbus_basic opath;
        dbus_array ifaces;
        if (!msg.get_args(&opath, &ifaces, nullptr))
            return;

        std::vector<std::string> iface_vect;
        for (size_t i=0; i<ifaces.size(); ++i)
            iface_vect.emplace_back (ifaces[i].str());

        // Call callback
        cb (opath.str(), iface_vect);
    }


}
