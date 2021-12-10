/*
 * Copyright (C) 2021 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/org_freedesktop_DBus_Properties.hpp>
#include <ultrabus/org_freedesktop_DBus.hpp>
#include <typeinfo>
#include <sstream>

//#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", \
                                     (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    org_freedesktop_DBus_Properties::org_freedesktop_DBus_Properties (Connection& connection)
        : MessageHandler (connection)
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static retvalue<Properties> handle_get_all_result (Message& reply)
    {
        retvalue<Properties> retval;

        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }

        dbus_array props;
        if (reply.get_args(&props, nullptr))
            retval.get() = std::move (props);
        else
            retval.err (-1, "Invalid message reply argument");

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<Properties> org_freedesktop_DBus_Properties::get_all (
            const std::string& service,
            const std::string& object_path,
            const std::string& interface)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "GetAll");
        msg.append_arg (interface);

        auto reply = conn.send_and_wait (msg);
        return handle_get_all_result (reply);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Properties::get_all (const std::string& service,
                                                  const std::string& object_path,
                                                  const std::string& interface,
                                                  std::function<void (retvalue<Properties>& retval)> cb)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "GetAll");
        msg.append_arg (interface);

        if (!cb)
            return conn.send (msg);
        else
            return conn.send (msg, [cb](Message& reply){
                                       auto retval = handle_get_all_result (reply);
                                       cb (retval);
                                   });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<dbus_variant> org_freedesktop_DBus_Properties::get (const std::string& service,
                                                                 const std::string& object_path,
                                                                 const std::string& interface,
                                                                 const std::string& property_name)
    {
        retvalue<dbus_variant> retval;
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "Get");
        msg.append_arg (interface, property_name);

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        if (!reply.get_args(&retval.get(), nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Properties::get (const std::string& service,
                                              const std::string& object_path,
                                              const std::string& interface,
                                              const std::string& property_name,
                                              std::function<void (retvalue<dbus_variant>& retval)> cb)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "Get");
        msg.append_arg (interface, property_name);

        if (!cb)
            return conn.send (msg);
        else
            return conn.send (msg, [cb](Message& reply){
                                       retvalue<dbus_variant> retval;
                                       if (reply.is_error()) {
                                           retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                                       }
                                       else if (!reply.get_args(&retval.get(), nullptr)) {
                                           retval.err (-1, "Invalid message reply argument");
                                       }
                                       cb (retval);
                                   });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<int> org_freedesktop_DBus_Properties::set_impl (const std::string& service,
                                                             const std::string& object_path,
                                                             const std::string& interface,
                                                             const std::string& property_name,
                                                             const dbus_variant& value)
    {
        retvalue<int> retval (0);
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "Set");
        msg.append_arg (interface, property_name, value);

        TRACE ("%s,%s,%s,%s (%s): %s",
               service.c_str(),
               object_path.c_str(),
               interface.c_str(),
               property_name.c_str(),
               const_cast<dbus_variant&>(value).value().signature().c_str(),
               value.str().c_str());

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval = -1;
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Properties::set_impl_async (const std::string& service,
                                                         const std::string& object_path,
                                                         const std::string& interface,
                                                         const std::string& property_name,
                                                         const dbus_variant& value,
                                                         std::function<void (retvalue<int>& result)> cb)
    {
        Message msg (service, object_path, "org.freedesktop.DBus.Properties", "Set");
        msg.append_arg (interface, property_name, value);

        if (!cb)
            return conn.send (msg);
        else
            return conn.send (msg, [cb](Message& reply){
                                       retvalue<int> retval (0);
                                       if (reply.is_error()) {
                                           retval = -1;
                                           retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                                       }
                                       cb (retval);
                                   });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static std::string make_props_changed_rule (const std::string& bus_name,
                                                const std::string& object_path)
    {
        std::stringstream rule;
        rule << "type='signal'"
             << ",sender='" << bus_name << "'"
             << ",path='" << object_path << "'"
             << ",interface='org.freedesktop.DBus.Properties'"
             << ",member='PropertiesChanged'";
        return rule.str ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Properties::add_properties_changed_cb (const std::string& service,
                                                                    const std::string& object_path,
                                                                    properties_changed_cb callback)
    {
        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(object_path.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service,
                                    [this, object_path, callback](retvalue<std::string>& bus_name){
                                        // Called from worker thread
                                        if (bus_name.err())
                                            return;

                                        std::lock_guard<std::mutex> lock (props_changed_mutex);

                                        auto key = std::make_pair (bus_name.get(), object_path);
                                        auto entry = props_changed_callbacks.find (key);

                                        if (callback) {
                                            // Add/set callback
                                            if (entry == props_changed_callbacks.end()) {
                                                props_changed_callbacks.emplace (key, callback);
                                                add_match_rule (make_props_changed_rule(bus_name.get(),
                                                                                        object_path));
                                            }else{
                                                entry->second = callback;
                                            }
                                        }else{
                                            // callback is nullptr, Remove callback
                                            if (entry != props_changed_callbacks.end()) {
                                                props_changed_callbacks.erase (entry);
                                                remove_match_rule (make_props_changed_rule(bus_name.get(),
                                                                                           object_path));
                                            }
                                        }
                                    });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Properties::remove_properties_changed_cb (const std::string& service,
                                                                       const std::string& object_path)
    {
        // Sanity check
        if (!dbus_validate_bus_name(service.c_str(), nullptr) ||
            !dbus_validate_path(object_path.c_str(), nullptr))
        {
            return -1;
        }

        // Get the unique bus name for the service
        org_freedesktop_DBus dbus (conn);
        return dbus.get_name_owner (service,
                                    [this, object_path](retvalue<std::string>& bus_name){
                                        // Called from worker thread
                                        if (bus_name.err())
                                            return;
                                        std::lock_guard<std::mutex> lock (props_changed_mutex);

                                        auto key = std::make_pair (bus_name.get(), object_path);
                                        auto entry = props_changed_callbacks.find (key);
                                        if (entry != props_changed_callbacks.end()) {
                                            props_changed_callbacks.erase (entry);
                                            remove_match_rule (make_props_changed_rule(bus_name.get(),
                                                                                       object_path));
                                        }
                                    });
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool org_freedesktop_DBus_Properties::on_signal (Message& msg)
    {
        if (msg.interface() != "org.freedesktop.DBus.Properties" ||
            msg.name() != "PropertiesChanged")
        {
            return false;
        }

        std::lock_guard<std::mutex> lock (props_changed_mutex);
        auto key = std::make_pair (msg.sender(), msg.path());
        auto entry = props_changed_callbacks.find (key);
        if (entry != props_changed_callbacks.end() && entry->second) {
            auto cb = entry->second;

            try {
                dbus_basic iface_name;
                dbus_array dbus_changed_props;
                dbus_array dbus_invalidated_props;
                if (!msg.get_args(&iface_name,
                                  &dbus_changed_props,
                                  &dbus_invalidated_props,
                                  nullptr))
                {
                    // Invalid message parameters
                    return false;
                }
                Properties changed_props (dbus_changed_props);
                std::set<std::string> invalidated_props;

                for (auto& entry : dbus_invalidated_props)
                    invalidated_props.emplace (entry.str());

                props_changed_mutex.unlock ();
                cb (iface_name.str(), changed_props, invalidated_props);
            }
            catch (std::bad_cast& bc) {
                ; // Invalid message parameters
            }
        }


        return false;
    }


}
