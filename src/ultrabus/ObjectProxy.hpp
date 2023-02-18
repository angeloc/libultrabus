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
#ifndef ULTRABUS_OBJECTPROXY_HPP
#define ULTRABUS_OBJECTPROXY_HPP

#include <ultrabus/Connection.hpp>
#include <ultrabus/Message.hpp>
#include <ultrabus/MessageHandler.hpp>
#include <ultrabus/dbus_type_base.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_array.hpp>
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/dbus_struct.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <ultrabus/Properties.hpp>
#include <functional>
#include <string>
#include <mutex>
#include <map>
#include <dbus/dbus.h>


namespace ultrabus {

    /**
     * Proxy class for communicating with dbus objects.
     */
    class ObjectProxy : public MessageHandler {
    public:
        /**
         * Callback for signals sent to the object proxy.
         * @param sig_msg The signal message.
         */
        using sig_cb = std::function<void (Message& sig_msg)>;

        /**
         * Constructor.
         * @param connection A DBus connection object.
         * @param service The service that owns the object.
         * @param object_path The object we want to communicate with.
         * @param default_interface The default DBus interface to use
         *                          when calling methods on the object.
         *                          Or an empty string if no default
         *                          interface should be set.
         * @param msg_timeout A timeout value in milliseconds used
         *                    when sending messages on the bus with
         *                    this proxy class.
         *                    DBUS_TIMEOUT_USE_DEFAULT means that a
         *                    default timeout value will be used by
         *                    the underlaying dbus library (libdbus-1).
         * @throw std::invalid_argument If the name of the service,
         *                              object path, or interface
         *                              is not a valid name.
         */
        ObjectProxy (Connection& connection,
                     const std::string& service,
                     const std::string& object_path,
                     const std::string& default_interface="",
                     const int msg_timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Destructor.
         */
        virtual ~ObjectProxy () = default;

        /**
         * Get the name of the service.
         */
        const std::string& service () const;

        /**
         * Get the name of the object path.
         */
        const std::string& path () const;

        /**
         * Add a callback to be called when specific signals arrives from this object.
         * If a callback is added for the same interface/signal twice,
         * the first registered callback is replaced with the new.
         * There can only be one callback for each interface/signal on this object.
         * @param interface The interface implementing the specific signal.
         *                  If an empty string, any interface emitting the
         *                  specific signal triggers the callback.
         * @param signal The name of the signal. If an empty string,
         *               any signal from this DBus object with the
         *               (possibly) specified interface triggers the callback.
         * @param callback The callback to be called when signals arrive.<br/>
         *                 If this parameter is <code>nullptr</code>,
         *                 it will have the same effect as calling method
         *                 <code>remove_signal_callback()</code>.
         * @return 0 on success. -1 if the interface or signal name is an invalid name.
         */
        int add_signal_callback (const std::string& interface,
                                 const std::string& signal,
                                 sig_cb callback);

        /**
         * Remove a previously added signal callback.
         */
        void remove_signal_callback (const std::string& interface,
                                     const std::string& signal);

        /**
         * Remove all signal callbacks.
         */
        void clear_signal_callbacks ();

        /**
         * Send a message and wait for a reply.
         */
        Message send_msg (Message& msg);

        /**
         * Call a method on the object wait for a result.
         * @param name The name of the method.
         */
        Message call (const std::string& name)
        {
            return send_msg_impl (Message(target, opath, def_iface, name));
        }

        /**
         * Call a method on the object wait for a result.
         * @param name The name of the method.
         * @param param The first parameter of the method.
         * @param params Optional additional parameters.
         */
        template<typename T, typename... Targs>
        Message call (const std::string& name,
                      const T& param,
                      Targs... params)
            {
                Message msg (target, opath, def_iface, name);
                msg.append_arg (param, params...);
                return send_msg_impl (msg);
            }

        /**
         * Call a method on the object wait for a result.
         * @param interface The method interface.
         * @param name The name of the method.
         */
        Message call_iface (const std::string& interface,
                            const std::string& name)
        {
            return send_msg_impl (Message(target, opath, interface, name));
        }

        /**
         * Call a method on the object wait for a result.
         * @param interface The method interface.
         * @param name The name of the method.
         * @param param The first parameter of the method.
         * @param params Optional additional parameters.
         */
        template<typename T, typename... Targs>
        Message call_iface (const std::string& interface,
                            const std::string& name,
                            const T& param,
                            Targs... params)
            {
                Message msg (target, opath, interface, name);
                msg.append_arg (param, params...);
                return send_msg_impl (msg);
            }

        /**
         * Get the timeout used when sending messages on the DBus
         * using this proxy instance.
         * @return A timeout value in milliseconds.
         *         DBUS_TIMEOUT_USE_DEFAULT(-1) means that a default
         *         timeout value is used by the underlaying
         *         dbus library (libdbus-1).
         */
        int msg_timeout ();

        /**
         * Set the timeout used when sending messages on the DBus
         * using this proxy instance.
         * @param milliseconds A timeout value in milliseconds.
         *                     DBUS_TIMEOUT_USE_DEFAULT means that a default
         *                     timeout value is used by the underlaying
         *                     dbus library (libdbus-1).
         */
        void msg_timeout (int milliseconds);


    protected:
        virtual bool on_signal (Message &msg);


    private:
        std::string target;
        std::string opath;
        std::string def_iface;
        int timeout;
        std::mutex cb_mutex;
        std::map<std::pair<std::string, std::string>, sig_cb> callbacks;
        Message send_msg_impl (const Message& msg);
        bool on_signal_impl (const std::string& interface,
                             const std::string& signal_name,
                             Message &msg);
    };


}
#endif
