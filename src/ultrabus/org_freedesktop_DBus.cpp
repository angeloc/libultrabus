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
#include <ultrabus/org_freedesktop_DBus.hpp>
#include <typeinfo>

#include <iostream>


//#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  include <sys/types.h>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif



namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    org_freedesktop_DBus::org_freedesktop_DBus (Connection& connection)
        : MessageHandler (connection)
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static void handle_str_reply (Message& reply, retvalue<std::string>& retval)
    {
        dbus_basic reply_arg;

        if (reply.is_error())
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
        else if (!reply.get_args(&reply_arg, nullptr))
            retval.err (-1, "Invalid message reply argument");
        else
            retval = reply_arg.str ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static void handle_u32_reply (Message& reply, retvalue<uint32_t>& retval)
    {
        dbus_basic reply_arg;

        if (reply.is_error())
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
        else if (!reply.get_args(&reply_arg, nullptr))
            retval.err (-1, "Invalid message reply argument");
        else
            retval = reply_arg.u32 ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static void handle_boolean_reply (Message& reply, retvalue<bool>& retval)
    {
        dbus_basic reply_arg;

        if (reply.is_error())
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
        else if (!reply.get_args(&reply_arg, nullptr))
            retval.err (-1, "Invalid message reply argument");
        else
            retval = reply_arg.boolean ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static retvalue<int> sync_call_method_reply_with_void (Connection& conn,
                                                           Message& msg)
    {
        retvalue<int> retval (0);
        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval = -1;
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
        }
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static int async_call_method_reply_with_void (Connection& conn,
                                                  Message& msg,
                                                  std::function<void (retvalue<int>& retval)>& cb)
    {
        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    retvalue<int> retval (0);
                    if (reply.is_error()) {
                        retval = -1;
                        retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                    }
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static retvalue<uint32_t> sync_call_method_reply_with_u32 (Connection& conn,
                                                               Message& msg)
    {
        retvalue<uint32_t> retval (0);
        auto reply = conn.send_and_wait (msg);
        handle_u32_reply (reply, retval);
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static int async_call_method_reply_with_u32 (Connection& conn,
                                                 Message& msg,
                                                 std::function<void (retvalue<uint32_t>& retval)>& cb)
    {
        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    retvalue<uint32_t> retval;
                    handle_u32_reply (reply, retval);
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static retvalue<std::string> sync_call_method_reply_with_str (Connection& conn,
                                                                  Message& msg)
    {
        retvalue<std::string> retval;
        auto reply = conn.send_and_wait (msg);
        handle_str_reply (reply, retval);
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    static int async_call_method_reply_with_str (Connection& conn,
                                                 Message& msg,
                                                 std::function<void (retvalue<std::string>& retval)>& cb)
    {
        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    retvalue<std::string> retval;
                    handle_str_reply (reply, retval);
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::string> org_freedesktop_DBus::hello ()
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "Hello");
        return sync_call_method_reply_with_str (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::hello (std::function<void (retvalue<std::string>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "Hello");
        return async_call_method_reply_with_str (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<uint32_t> org_freedesktop_DBus::request_name (const std::string bus_name,
                                                           uint32_t flags)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "RequestName");
        msg.append_arg (bus_name, flags);
        return sync_call_method_reply_with_u32 (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::request_name (const std::string bus_name,
                                            uint32_t flags,
                                            std::function<void (retvalue<uint32_t>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "RequestName");
        msg.append_arg (bus_name, flags);
        return async_call_method_reply_with_u32 (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<uint32_t> org_freedesktop_DBus::release_name (const std::string bus_name)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ReleaseName");
        msg << bus_name;
        return sync_call_method_reply_with_u32 (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::release_name (const std::string bus_name,
                                            std::function<void (retvalue<uint32_t>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ReleaseName");
        msg << bus_name;
        return async_call_method_reply_with_u32 (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::vector<std::string>> org_freedesktop_DBus::list_queued_owners (const std::string& bus_name)
    {
        retvalue<std::vector<std::string>> retval;
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListQueuedOwners");
        msg << bus_name;

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        dbus_array names;
        if (!reply.get_args(&names, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }
        for (auto& name : names)
            retval.get().emplace_back (name.str());

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::list_queued_owners (const std::string& bus_name,
                                                  std::function<void (retvalue<std::vector<std::string>>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListQueuedOwners");
        msg << bus_name;

        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    dbus_array names;
                    retvalue<std::vector<std::string>> retval;
                    if (reply.is_error())
                        retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                    else if (!reply.get_args(&names, nullptr))
                        retval.err (-1, "Invalid message reply argument");
                    else for (auto& name : names)
                             retval.get().emplace_back (name.str());
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::set<std::string>> org_freedesktop_DBus::list_names ()
    {
        retvalue<std::set<std::string>> retval;
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListNames");

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        dbus_array names;
        if (!reply.get_args(&names, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }
        for (auto& name : names)
            retval.get().emplace (name.str());

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::list_names (std::function<void (retvalue<std::set<std::string>>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListNames");

        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    dbus_array names;
                    retvalue<std::set<std::string>> retval;
                    if (reply.is_error())
                        retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                    else if (!reply.get_args(&names, nullptr))
                        retval.err (-1, "Invalid message reply argument");
                    else for (auto& name : names)
                             retval.get().emplace (name.str());
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::set<std::string>> org_freedesktop_DBus::list_activatable_names ()
    {
        retvalue<std::set<std::string>> retval;
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListActivatableNames");

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        dbus_array names;
        if (!reply.get_args(&names, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }
        for (auto& name : names)
            retval.get().emplace (name.str());

        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::list_activatable_names (std::function<void (retvalue<std::set<std::string>>& retval)> cb)
    {
        retvalue<std::set<std::string>> retval;
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "ListActivatableNames");

        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    dbus_array names;
                    retvalue<std::set<std::string>> retval;
                    if (reply.is_error())
                        retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                    else if (!reply.get_args(&names, nullptr))
                        retval.err (-1, "Invalid message reply argument");
                    else for (auto& name : names)
                             retval.get().emplace (name.str());
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<bool> org_freedesktop_DBus::name_has_owner (const std::string bus_name)
    {
        retvalue<bool> retval;
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "NameHasOwner");
        msg << bus_name;

        auto reply = conn.send_and_wait (msg);
        handle_boolean_reply (reply, retval);
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::name_has_owner (const std::string bus_name,
                                              std::function<void (retvalue<bool>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "NameHasOwner");
        msg << bus_name;

        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    retvalue<bool> retval;
                    handle_boolean_reply (reply, retval);
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<uint32_t> org_freedesktop_DBus::start_service_by_name (const std::string service,
                                                                    uint32_t flags)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "StartServiceByName");
        msg.append_arg (service, flags);
        return sync_call_method_reply_with_u32 (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::start_service_by_name (const std::string service,
                                                     uint32_t flags,
                                                     std::function<void (retvalue<uint32_t>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "StartServiceByName");
        msg.append_arg (service, flags);
        return async_call_method_reply_with_u32 (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<int> org_freedesktop_DBus::update_activation_environment (const std::map<std::string, std::string>& env)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "UpdateActivationEnvironment");

        dbus_array dict ("{ss}");
        for (auto& e : env)
            dict.add (dbus_dict_entry(e.first, e.second));
        msg << dict;

        return sync_call_method_reply_with_void (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::update_activation_environment (const std::map<std::string, std::string>& env,
                                                             std::function<void (retvalue<int>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "UpdateActivationEnvironment");

        dbus_array dict ("{ss}");
        for (auto& e : env)
            dict.add (dbus_dict_entry(e.first, e.second));
        msg << dict;

        return async_call_method_reply_with_void (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::string> org_freedesktop_DBus::get_name_owner (const std::string bus_name)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetNameOwner");
        msg << bus_name;
        return sync_call_method_reply_with_str (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::get_name_owner (const std::string bus_name,
                                              std::function<void (retvalue<std::string>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetNameOwner");
        msg << bus_name;
        return async_call_method_reply_with_str (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<uint32_t> org_freedesktop_DBus::get_connection_unix_user (const std::string service)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionUnixUser");
        msg << service;
        return sync_call_method_reply_with_u32 (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::get_connection_unix_user (const std::string service,
                                                        std::function<void (retvalue<uint32_t>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionUnixUser");
        msg << service;
        return async_call_method_reply_with_u32 (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<uint32_t> org_freedesktop_DBus::get_connection_unix_process_id (const std::string service)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionUnixProcessID");
        msg << service;
        return sync_call_method_reply_with_u32 (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::get_connection_unix_process_id (const std::string service,
                                                              std::function<void (retvalue<uint32_t>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionUnixProcessID");
        msg << service;
        return async_call_method_reply_with_u32 (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::map<std::string, dbus_variant>> org_freedesktop_DBus::get_connection_credentials (
            const std::string service)
    {
        retvalue<std::map<std::string, dbus_variant>> retval;

        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionCredentials");
        msg << service;

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }

        dbus_array reply_arg;
        if (!reply.get_args(&reply_arg, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }

        try {
            for (auto& entry : reply_arg) {
                auto& de = dynamic_cast<dbus_dict_entry&> (entry);
                retval.get().emplace (de.key().str(), de.value());
            }
        }
        catch (std::bad_cast& bc) {
            retval.get().clear ();
            retval.err (-1, "Invalid message reply argument");
        }
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::get_connection_credentials (
            const std::string service,
            std::function<void (retvalue<std::map<std::string, dbus_variant>>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetConnectionCredentials");
        msg << service;
        if (cb == nullptr) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [cb](Message& reply)
                {
                    retvalue<std::map<std::string, dbus_variant>> retval;
                    dbus_array reply_arg;

                    if (reply.is_error()) {
                        retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                    }
                    else if (!reply.get_args(&reply_arg, nullptr)) {
                        retval.err (-1, "Invalid message reply argument");
                    }
                    else {
                        try {
                            for (auto& entry : reply_arg) {
                                auto& de = dynamic_cast<dbus_dict_entry&> (entry);
                                retval.get().emplace (de.key().str(), de.value());
                            }
                        }
                        catch (std::bad_cast& bc) {
                            retval.get().clear ();
                            retval.err (-1, "Invalid message reply argument");
                        }
                    }
                    cb (retval);
                });
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<int> org_freedesktop_DBus::add_match (const std::string rule)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "AddMatch");
        msg << rule;
        return sync_call_method_reply_with_void (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::add_match (const std::string rule,
                                         std::function<void (retvalue<int>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "AddMatch");
        msg << rule;
        return async_call_method_reply_with_void (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<int> org_freedesktop_DBus::remove_match (const std::string rule)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "RemoveMatch");
        msg << rule;
        return sync_call_method_reply_with_void (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::remove_match (const std::string rule,
                                            std::function<void (retvalue<int>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "RemoveMatch");
        msg << rule;
        return async_call_method_reply_with_void (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::string> org_freedesktop_DBus::get_id (void)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetId");
        return sync_call_method_reply_with_str (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::get_id (std::function<void (retvalue<std::string>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetId");
        return async_call_method_reply_with_str (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<int> org_freedesktop_DBus::become_monitor (std::list<std::string> rules)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "BecomeMonitor");
        dbus_array rules_arg ("s");

        for (auto& r : rules)
            rules_arg << dbus_basic(r);
        msg << rules_arg;
        msg << 0;

        return sync_call_method_reply_with_void (conn, msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus::become_monitor (std::list<std::string> rules,
                                              std::function<void (retvalue<int>& retval)> cb)
    {
        Message msg (DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "BecomeMonitor");
        dbus_array rules_arg ("s");

        for (auto& r : rules)
            rules_arg << dbus_basic(r);
        msg << rules_arg;
        msg << 0;

        return async_call_method_reply_with_void (conn, msg, cb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus::set_name_owner_changed_cb (name_owner_changed_cb_t callback)
    {
        static const std::string rule =
            "type='signal'"
            ",sender='" DBUS_SERVICE_DBUS "'"
            ",path='" DBUS_PATH_DBUS "'"
            ",interface='" DBUS_INTERFACE_DBUS "'"
            ",member='NameOwnerChanged'";

        std::lock_guard<std::mutex> lock (cb_mutex);

        if (callback) {
            // Set callback
            if (!name_owner_changed_cb)
                add_match (rule, nullptr);
        }else{
            // Remove callback
            if (name_owner_changed_cb)
                remove_match (rule, nullptr);
        }
        name_owner_changed_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus::set_name_lost_cb (name_cb_t callback)
    {
        static const std::string rule =
            "type='signal'"
            ",sender='" DBUS_SERVICE_DBUS "'"
            ",path='" DBUS_PATH_DBUS "'"
            ",interface='" DBUS_INTERFACE_DBUS "'"
            ",member='NameLost'";

        std::lock_guard<std::mutex> lock (cb_mutex);

        if (callback) {
            // Set callback
            if (!name_lost_cb)
                add_match (rule, nullptr);
        }else{
            // Remove callback
            if (name_lost_cb)
                remove_match (rule, nullptr);
        }
        name_lost_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus::set_name_acquired_cb (name_cb_t callback)
    {
        static const std::string rule =
            "type='signal'"
            ",sender='" DBUS_SERVICE_DBUS "'"
            ",path='" DBUS_PATH_DBUS "'"
            ",interface='" DBUS_INTERFACE_DBUS "'"
            ",member='NameAcquired'";

        std::lock_guard<std::mutex> lock (cb_mutex);

        if (callback) {
            // Set callback
            if (!name_acquired_cb)
                add_match (rule, nullptr);
        }else{
            // Remove callback
            if (name_acquired_cb)
                remove_match (rule, nullptr);
        }
        name_acquired_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool org_freedesktop_DBus::on_signal (Message& msg)
    {
        if (msg.interface() != DBUS_INTERFACE_DBUS ||
            msg.path() != DBUS_PATH_DBUS)
        {
            return false;
        }

        std::unique_lock<std::mutex> cb_lock (cb_mutex);

        if (!name_owner_changed_cb && !name_lost_cb && !name_acquired_cb)
            return false;

        if (unique_bus_name.empty()) {
            // Fetch the unique bus name of 'org.freedesktop.DBus' before calling on_signal_impl
            get_name_owner (DBUS_SERVICE_DBUS, [this, msg](retvalue<std::string>& retval)
                {
                    // Callback called from worker thread
                    std::unique_lock<std::mutex> cb_lock (cb_mutex);
                    unique_bus_name = retval.get ();
                    on_signal_impl (const_cast<Message&>(msg), cb_lock);
                });
        }else{
            on_signal_impl (msg, cb_lock);
        }

        return false;
    }


    //--------------------------------------------------------------------------
    // cb_mutex is locked
    //--------------------------------------------------------------------------
    void org_freedesktop_DBus::on_signal_impl (Message& msg,
                                               std::unique_lock<std::mutex>& cb_lock)
    {
        if (msg.sender() != unique_bus_name)
            return;

        if (msg.name() == "NameOwnerChanged" && name_owner_changed_cb) {
            dbus_basic name;
            dbus_basic old_owner;
            dbus_basic new_owner;
            if (msg.get_args(&name, &old_owner, &new_owner, nullptr)) {
                auto cb = name_owner_changed_cb;
                cb_lock.unlock ();
                cb (name.str(), old_owner.str(), new_owner.str());
            }
        }
        else if (msg.name() == "NameLost" && name_lost_cb) {
            dbus_basic name;
            if (msg.get_args(&name, nullptr)) {
                auto cb = name_lost_cb;
                cb_lock.unlock ();
                cb (name.str());
            }
        }
        else if (msg.name() == "NameAcquired" && name_acquired_cb) {
            dbus_basic name;
            if (msg.get_args(&name, nullptr)) {
                auto cb = name_acquired_cb;
                cb_lock.unlock ();
                cb (name.str());
            }
        }
    }


}
