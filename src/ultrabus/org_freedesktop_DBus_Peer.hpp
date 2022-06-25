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
#ifndef ULTRABUS_ORG_FREEDESKTOP_DBUS_PEER_HPP
#define ULTRABUS_ORG_FREEDESKTOP_DBUS_PEER_HPP

#include <ultrabus/Connection.hpp>
#include <ultrabus/retvalue.hpp>
#include <string>
#include <functional>


namespace ultrabus {

    /**
     * Proxy class to use methods in standard DBus interface <code>org.freedesktop.DBus.Peer</code>.
     *
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-peer
     *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.Peer</a>
     */
    class org_freedesktop_DBus_Peer {
    public:
        /**
         * Constructor.
         * @param connection A DBus connection.
         * @param msg_timeout A timeout value in milliseconds used
         *                    when sending messages on the bus using
         *                    this instance.
         *                    DBUS_TIMEOUT_USE_DEFAULT means that a
         *                    default timeout value will be used by
         *                    the underlaying dbus library (libdbus-1).
         */
        org_freedesktop_DBus_Peer (Connection& connection,
                                   const int msg_timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Ping a service on the message bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>ping</code> method instead.
         *
         * @param service A bus name.
         * @return The number of microseconds it took
         *         to send the ping and get a reply.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<unsigned> ping (const std::string& service);

        /**
         * Asynchronous call to ping a service on the message bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param callback This callback is called when a reply
         *                 to the ping message is received.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<unsigned> ping (const std::string& service,
                                 std::function<void (retvalue<unsigned>& result)> callback);

        /**
         * Get the machine id of a service on the message bus.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_machine_id</code> method instead.
         *
         * @param service A bus name.
         * @return The machine id if the service.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        retvalue<std::string> get_machine_id (const std::string& service);

        /**
         * Asynchronous call to get the machine id of a service on the message bus.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param callback This callback is called when a result
         *                 is received on the message bus.
         *                 <br/>The parameter to the callback is the
         *                 same as the return value from the
         *                 corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         */
        int get_machine_id (const std::string& service,
                            std::function<void (retvalue<std::string>& result)> callback);

        /**
         * Get the timeout used when sending messages on the DBus using instance.
         * @return A timeout value in milliseconds.
         *         DBUS_TIMEOUT_USE_DEFAULT(-1) means that a default
         *         timeout value is used by the underlaying
         *         dbus library (libdbus-1).
         */
        int msg_timeout () {
            return timeout;
        }

        /**
         * Set the timeout used when sending messages on the DBus using this instance.
         * @param millieconds A timeout value in milliseconds.
         *                    DBUS_TIMEOUT_USE_DEFAULT means that a default
         *                    timeout value is used by the underlaying
         *                    dbus library (libdbus-1).
         */
        void msg_timeout (int milliseconds) {
            timeout = milliseconds;
        }


    private:
        Connection& conn;
        int timeout;
    };


}
#endif
