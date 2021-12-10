/*
 * Copyright (C) 2015-2017,2021 Dan Arrhenius <dan@ultramarin.se>
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
#ifndef ULTRABUS_MESSAGE_HPP
#define ULTRABUS_MESSAGE_HPP

#include <ultrabus/dbus_type_base.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_array.hpp>
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/dbus_struct.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <ultrabus/Properties.hpp>
#include <string>
#include <dbus/dbus.h>


namespace ultrabus {


    /**
     * DBus message.
     * This class represents a DBus message.
     * This includes methods calls, method replies, signals, and error messages.
     */
    class Message {
    public:
        /**
         * Default constructor. This constructs an empty non-defined invalid message.
         */
        Message ();

        /**
         * Create a method call message.
         * @param destination The service that will receive the message.
         * @param path The destination object path of the message.
         * @param iface The interface of the method to invoke.
         * @param method The method to invoke.
         */
        Message (const std::string& destination,
                 const std::string& path,
                 const std::string& iface,
                 const std::string& method);

        /**
         * Create a Message object from a DBusMessage pointer.
         * @param message The DBus message object.
         *                The original DBusMessage is not copied but its reference counter
         *                is increased during the lifetime of this object.
         */
        explicit Message (DBusMessage* message);

        /**
         * Create a signal message.
         * @param path The path of the object emitting the signal.
         * @param iface The interface the signal is emitted from.
         * @param name The name of the signal.
         */
        Message (const std::string& path,
                 const std::string& iface,
                 const std::string& name);

        /**
         * Create a message reply message.
         * @param message The DBus message object being replied to.
         * @param is_error True if this is an error response.
         * @param name The name of the error is this is an error message.
         * @param error_message The error description.
         */
        Message (DBusMessage* message,
                 const bool is_error,
                 const std::string& error_name="",
                 const std::string& error_message="");

        /**
         * Create a message reply message.
         * @param message The DBus message object being replied to.
         * @param is_error True if this is an error response.
         * @param name The name of the error is this is an error message.
         * @param error_message The error description.
         */
        Message (Message& message,
                 const bool is_error,
                 const std::string& error_name="",
                 const std::string& error_message="")
            : Message(message.handle(),
                      is_error,
                      error_name,
                      error_message)
            {
            }

        /**
         * Copy constructor.
         * Create a copy of another message.
         * @param message The message to copy.
         */
        Message (const Message& message);

        /**
         * Move constructor.
         * @param message The message to move.
         */
        Message (Message&& message);

        /**
         * Destructor.
         */
        ~Message ();

        /**
         * Add an argument to the message.
         * @param arg The argument to add.
         */
        template<typename T>
        Message& operator<< (const T& arg) {
            append_arg (arg);
            return *this;
        }

        /**
         * Assignment operator.
         * @param message The message to copy.
         */
        Message& operator= (const Message& message);

        /**
         * Move operator.
         * @param message The message to move.
         */
        Message& operator= (Message&& message);

        /**
         * Return the underlaying DBusMessage handle.
         * This is used of there is a need to call functions in the low-level DBus library.
         * @return A pointer to a DBusMessage structure.
         * @see https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html
         */
        DBusMessage* handle ();

        /**
         * Add an argument to the message.
         * @param arg The first argument to add.
         * @param args Optional number of arguments to add.
         */
        template<typename T, typename... Targs>
        void append_arg (const T& arg, Targs... args)
        {
            append_arg_impl (arg, args...);
        }

        /**
         * Return the message arguments.
         * @return A vector of shared pointers to the message arguments.
         */
        std::vector<dbus_type_ptr> arguments ();

        /**
         * Get arguments from the message.
         * Supply a list of pointers to different dbus types that will
         * be filled with the values of the message arguments.
         * <br/><b>Important!</b> the last argument to this method must
         * be <code>nullptr</code> to indicate that the list of arguments
         * to fill has ended.
         */
        bool get_args (dbus_type* arg, ...);

        /**
         * Return the DBus message type.
         * @return The DBus message type.
         * @see https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga41cace31999105137772b6257ea540f9
         */
        int type () const;

        /**
         * Check if this is an invalid DBus message.
         */
        bool is_invalid () const {
            return type() == DBUS_MESSAGE_TYPE_INVALID;
        }

        /**
         * Check if this is a method call.
         */
        bool is_method_call () const {
            return type() == DBUS_MESSAGE_TYPE_METHOD_CALL;
        }

        /**
         * Check if this is a method return.
         */
        bool is_method_return () const {
            return type() == DBUS_MESSAGE_TYPE_METHOD_RETURN;
        }

        /**
         * Check if this is an error message.
         */
        bool is_error () const {
            return type() == DBUS_MESSAGE_TYPE_ERROR;
        }

        /**
         * Check if this is a signal.
         */
        bool is_signal () const {
            return type() == DBUS_MESSAGE_TYPE_SIGNAL;
        }

        /**
         * Get the destination of the message.
         * @return The detsination of the message.
         */
        std::string destination () const;

        /**
         * Set the destination of the message.
         * @param destination The destination of the message.
         */
        void destination (const std::string& bus_name);

        /**
         * Get the object path of the message.
         * @return The object path the message is sent to.
         */
        std::string path () const;

        /**
         * Set the object path of the message.
         * @param path The object path the message is sent to.
         */
        void path (const std::string& path);

        /**
         * Get the interface the method belongs to.
         * @return The interface the method belongs to.
         */
        std::string interface () const;

        /**
         * Set the interface the method belongs to.
         * @param iface The interface the method belongs to.
         */
        void interface (const std::string& iface);

        /**
         * Return the name of the message.
         * @return the name of the message.
         */
        std::string name () const;

        /**
         * Set the name of the message.
         * @param msg_name The name of the message.
         */
        void name (const std::string& msg_name);

        /**
         * Get the error name if this message is an error message.
         * This method is irrelevant if the message isn't an error message.
         * @return The error name.
         */
        std::string error_name () const;

        /**
         * Set the error name if this message is an error message.
         * This method is irrelevant if the message isn't an error message.
         * @param name The name of the error.
         */
        void error_name (const std::string& name);

        /**
         * Return the error description, if any.
         * The error message is the first argument of the message if that argument is a string.
         * This method is irrelevant if the message isn't an error message.
         * @return The error description is in the "s" parameter in the error message.
         */
        std::string error_msg () const;

        /**
         * Get the unique name of the connection which originated the message.
         * @return The name of the connection which originated the message.
         */
        std::string sender () const;

        /**
         * Get the serial number of the message.
         * @return A message serial number or 0 if there isn't a serial number.
         */
        uint32_t serial () const;

        /**
         * If this is a message reply, return the serial number of the message this is a replyt to.
         * @return A message serial number or 0 if there isn't a serial number.
         */
        uint32_t reply_serial () const;

        /**
         * Get a string return value.
         * This will return the first return value as a string.
         * If no such return value exists it will return an empty string.
         * @return A string return value, or an empty string.
         */
        std::string get_string_ret () const;

        /**
         * Return the DBus signature of the message.
         * @return The DBus signature of the message.
         */
        std::string signature () const;

        /**
         * Increase message reference counter.
         */
        void inc_ref ();

        /**
         * Decrease message reference counter.
         */
        void dec_ref ();

        /**
         * Return a string representation of the message with its parameters for debugging purposes.
         * @return A string describing the message.
         */
        std::string describe () const;


    private:
        DBusMessage* msg_handle;

        void append_dbus_type_base (const dbus_type_base& arg);
        inline void append_arg_impl () {
            ;
        }
        template<typename T, typename... Targs>
        void append_arg_impl (const T& arg, Targs... args) {
            append_dbus_type_base (dbus_basic((T)arg));
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_type_base& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_type& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_basic& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_array& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_dict_entry& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_struct& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const dbus_variant& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
        template<typename... Targs>
        void append_arg_impl (const Properties& arg, Targs... args) {
            append_dbus_type_base (arg);
            append_arg_impl (args...);
        }
    };


}

#endif
