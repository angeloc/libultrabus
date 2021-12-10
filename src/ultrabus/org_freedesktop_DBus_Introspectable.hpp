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
#ifndef ULTRABUS_ORG_FREEDESKTOP_DBUS_INTROSPECTABLE_HPP
#define ULTRABUS_ORG_FREEDESKTOP_DBUS_INTROSPECTABLE_HPP

#include <ultrabus/Connection.hpp>
#include <ultrabus/retvalue.hpp>
#include <string>
#include <functional>


namespace ultrabus {

    /**
     * Proxy class to use methods in standard DBus interface <code>org.freedesktop.DBus.Introspectable</code>.
     *
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-introspectable
     *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.Introspectable</a>
     */
    class org_freedesktop_DBus_Introspectable {
    public:
        /**
         * Constructor.
         * @param connection A DBus connection.
         */
        org_freedesktop_DBus_Introspectable (Connection& connection);

        /**
         * Get introspect data of an object in a DBus service.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>introspect</code> method instead.
         *
         * @param service A bus name.
         * @param object_path Path to the object we want to inspect.
         * @return XML introspect data.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#introspection-format
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Introspection Data Format</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        retvalue<std::string> introspect (const std::string& service,
                                          const std::string& object_path="/");

        /**
         * Asynchronous call to get introspect data of an object in a DBus service.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param object_path Path to the object we want to inspect.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#introspection-format
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Introspection Data Format</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        int introspect (const std::string& service,
                        const std::string& object_path,
                        std::function<void (retvalue<std::string>& result)> callback);


    private:
        Connection& conn;
    };


}
#endif
