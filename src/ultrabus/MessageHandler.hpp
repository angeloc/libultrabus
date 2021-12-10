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
#ifndef ULTRABUS_MESSAGEHANDLER_HPP
#define ULTRABUS_MESSAGEHANDLER_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/Message.hpp>
#include <ultrabus/Connection.hpp>
#include <dbus/dbus.h>
#include <string>
#include <mutex>
#include <set>


namespace ultrabus {


    /**
     * Base class for DBus message handlers.
     */
    class MessageHandler {
    public:
        /**
         * Constructor.
         * @param connection A DBus connection object.
         */
        MessageHandler (Connection& connection);

        /**
         * Destructor.
         * Remove all added match rules and unregister this handler from the DBus.
         */
        virtual ~MessageHandler ();

        /**
         * Adds a match rule to match messages going through the message bus.
         * @param rule The match rule to add.
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        void add_match_rule (const std::string& rule);

        /**
         * Removes a previously-added match rule.
         * @param rule The match rule to remove.
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Match Rules</a>
         */
        void remove_match_rule (const std::string& rule);


    protected:
        Connection& conn; /**< Reference to a Connection object. */

        /**
         * This method is called by method dispatch_msg on incoming method calls.
         * The dispatch_msg method will not call this method if the destination
         * is not this bus connection regardless of installed message filters.
         * @param msg The incoming method call.
         * @return true if the message was handled,
         *         false if not.
         */
        virtual bool on_method_call (Message& msg);

        /**
         * This method is called by method dispatch_msg on incoming signal.
         * @param msg The incoming signal.
         * @return true if the message was handled,
         *         false if not.
         */
        virtual bool on_signal (Message& msg);

        /**
         * This method is called on all incoming messages.
         * The default implementation is to call method
         * dispatch_msg wich in turn calls on_method_call
         * or on_signal depending on the message type.
         * @return true if the message was handled,
         *         false if not.
         */
        virtual bool on_message (Message& msg);

        /**
         * Take a message and call on_method_call or
         * on_signal depending on the type of message.
         * If the message isn't a method call or signal
         * it simply returns false.
         * @param msg The incoming message to dispatch.
         * @return true if the message was handled,
         *         false if not.
         */
        bool dispatch_msg (Message& msg);


    private:
        static DBusHandlerResult static_dbus_handler (
                DBusConnection* dbconn,
                DBusMessage* dbmsg,
                void* user_data);

        std::mutex match_rule_mutex;
        std::set<std::string> match_rules;
    };

}



#endif
