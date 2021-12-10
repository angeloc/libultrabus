/*
 * Copyright (C) 2017,2021 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/MessageHandler.hpp>
#include <system_error>
#include <cerrno>

//#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif


namespace ultrabus {



    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    MessageHandler::MessageHandler (Connection& connection)
        : conn (connection)
    {
        if (!dbus_connection_add_filter(conn.handle(),
                                        static_dbus_handler,
                                        this,
                                        nullptr))
        {
            throw std::system_error (ENOMEM, std::generic_category());
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    MessageHandler::~MessageHandler ()
    {
        dbus_connection_remove_filter (conn.handle(),
                                       static_dbus_handler,
                                       this);

        std::lock_guard<std::mutex> lock (match_rule_mutex);
        for (auto& rule : match_rules)
            dbus_bus_remove_match (conn.handle(), rule.c_str(), nullptr);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void MessageHandler::add_match_rule (const std::string& rule)
    {
        std::lock_guard<std::mutex> lock (match_rule_mutex);

        if (match_rules.find(rule) != match_rules.end())
            return;

        TRACE ("Add match rule: %s", rule.c_str());
        if (conn.io_handler().same_context() == false) {
            dbus_bus_add_match (conn.handle(), rule.c_str(), nullptr);
        }else{
            Message msg ("org.freedesktop.DBus",
                         "/org/freedesktop/DBus",
                         "org.freedesktop.DBus",
                         "AddMatch");
            msg << rule;
            conn.send (msg);
        }
        match_rules.emplace (rule);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void MessageHandler::remove_match_rule (const std::string& rule)
    {
        std::lock_guard<std::mutex> lock (match_rule_mutex);

        auto i = match_rules.find (rule);
        if (i == match_rules.end())
            return;

        TRACE ("Remove match rule: %s", rule.c_str());
        if (conn.io_handler().same_context() == false) {
            dbus_bus_remove_match (conn.handle(), rule.c_str(), nullptr);
        }else{
            Message msg ("org.freedesktop.DBus",
                         "/org/freedesktop/DBus",
                         "org.freedesktop.DBus",
                         "RemoveMatch");
            msg << rule;
            conn.send (msg);
        }
        match_rules.erase (i);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool MessageHandler::on_method_call (Message& msg)
    {
        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool MessageHandler::on_signal (Message& msg)
    {
        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool MessageHandler::on_message (Message& msg)
    {
        return dispatch_msg (msg);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool MessageHandler::dispatch_msg (Message& msg)
    {
        if (msg.is_method_call())
            return on_method_call (msg);
        else if (msg.is_signal())
            return on_signal (msg);
        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    DBusHandlerResult MessageHandler::static_dbus_handler (
            DBusConnection* dbconn,
            DBusMessage* dbmsg,
            void* user_data)
    {
        MessageHandler* handler {static_cast<MessageHandler*>(user_data)};
        Message msg (dbmsg);
        return handler->on_message(msg) ?
            DBUS_HANDLER_RESULT_HANDLED :
            DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }


}
