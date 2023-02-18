/*
 * Copyright (C) 2021-2023 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/ObjectProxy.hpp>
#include <stdexcept>
#include <sstream>


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    ObjectProxy::ObjectProxy (Connection& connection,
                              const std::string& service,
                              const std::string& object_path,
                              const std::string& default_interface,
                              const int msg_timeout)
        : MessageHandler (connection),
          target (service),
          opath (object_path),
          def_iface (default_interface),
          timeout (msg_timeout)
    {
        DBusError err;
        dbus_error_init (&err);

        dbus_validate_bus_name (service.c_str(), &err);
        if (!dbus_error_is_set(&err))
            dbus_validate_path (object_path.c_str(), &err);
        if (!default_interface.empty() && !dbus_error_is_set(&err))
            dbus_validate_interface (default_interface.c_str(), &err);

        if (dbus_error_is_set(&err)) {
            //std::string err_name (err.name ? err.name : "");
            std::string err_msg (err.message ? err.message : "");
            dbus_error_free (&err);
            throw std::invalid_argument (err_msg);
        }
        dbus_error_free (&err);

        if (timeout < 0)
            timeout = DBUS_TIMEOUT_USE_DEFAULT;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const std::string& ObjectProxy::service () const
    {
        return target;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const std::string& ObjectProxy::path () const
    {
        return opath;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int ObjectProxy::add_signal_callback (const std::string& iface,
                                          const std::string& signal,
                                          sig_cb callback)
    {
        if (!iface.empty() && !dbus_validate_interface(iface.c_str(), nullptr))
            return -1;
        if (!signal.empty() && !dbus_validate_member(signal.c_str(), nullptr))
            return -1;
        if (!callback) {
            remove_signal_callback (iface, signal);
            return 0;
        }

        std::stringstream rule;
        rule << "type='signal',sender='" << target
             << "',path='" << opath << "'";
        if (!iface.empty())
            rule << ",interface='" << iface << "'";
        if (!signal.empty())
            rule << ",member='" << signal << "'";

        std::lock_guard<std::mutex> lock (cb_mutex);
        callbacks.emplace (std::make_pair(iface, signal), callback);
        add_match_rule (rule.str());
        return 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void ObjectProxy::remove_signal_callback (const std::string& iface,
                                              const std::string& signal)
    {
        std::lock_guard<std::mutex> lock (cb_mutex);
        auto key = std::make_pair (iface, signal);
        auto entry = callbacks.find (key);
        if (entry == callbacks.end())
            return;

        callbacks.erase (entry);

        std::stringstream rule;
        rule << "type='signal',sender='" << target
             << "',path='" << opath << "'";
        if (!iface.empty())
            rule << ",interface='" << iface << "'";
        if (!signal.empty())
            rule << ",member='" << signal << "'";

        remove_match_rule (rule.str());
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void ObjectProxy::clear_signal_callbacks ()
    {
        std::lock_guard<std::mutex> lock (cb_mutex);
        for (auto& i : callbacks) {
            auto& iface = i.first.first;
            auto& signal = i.first.second;

            std::stringstream rule;
            rule << "type='signal',sender='" << target
                 << "',path='" << opath << "'";
            if (!iface.empty())
                rule << ",interface='" << iface << "'";
            if (!signal.empty())
                rule << ",member='" << signal << "'";

            remove_match_rule (rule.str());
        }
        callbacks.clear ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Message ObjectProxy::send_msg (Message& msg)
    {
        msg.destination (target);
        msg.path (opath);
        return send_msg_impl (msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Message ObjectProxy::send_msg_impl (const Message& msg)
    {
        return conn.send_and_wait (msg, timeout);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool ObjectProxy::on_signal (Message &msg)
    {
        if (msg.path() != opath)
            return false;

        bool retval = false;
        auto interface = msg.interface ();
        auto signal_name = msg.name ();

        // Find callback mapped to a specific interface and a specific signal name
        if (on_signal_impl(interface, signal_name, msg))
            retval = true;

        // Find callback mapped to any interface and a specific signal name
        if (on_signal_impl("", signal_name, msg))
            retval = true;

        // Find callback mapped to a specific interface and any signal name
        if (on_signal_impl(interface, "", msg))
            retval = true;

        // Find callback mapped to any interface and any signal name
        if (on_signal_impl("", "", msg))
            retval = true;

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool ObjectProxy::on_signal_impl (const std::string& interface,
                                      const std::string& signal_name,
                                      Message &msg)
    {
        bool retval = false;

        std::unique_lock<std::mutex> cb_lock (cb_mutex);
        auto key = std::make_pair (interface, signal_name);
        auto entry = callbacks.find (key);
        if (entry != callbacks.end()) {
            auto cb = entry->second;
            cb_lock.unlock ();
            cb (msg);
            retval = true;
        }
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int ObjectProxy::msg_timeout ()
    {
        return timeout;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void ObjectProxy::msg_timeout (int milliseconds)
    {
        timeout = milliseconds;
    }


}
