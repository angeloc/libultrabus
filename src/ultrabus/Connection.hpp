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
#ifndef ULTRABUS_CONNECTION_HPP
#define ULTRABUS_CONNECTION_HPP

#include <ultrabus/Message.hpp>
#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <dbus/dbus.h>
#include <iomultiplex.hpp>


namespace ultrabus {


    /**
     * A DBus connection.
     */
    class Connection {
    public:
        /**
         * Default constructor.
         */
        Connection ();

        /**
         * Constructor.
         */
        Connection (iomultiplex::iohandler_base& io_handler);

        /**
         * Destructor.
         */
        ~Connection ();

        /**
         * Connect and register to a well known bus.
         * @return 0 on success, -1 on failure.
         */
        int connect (const DBusBusType type=DBUS_BUS_SESSION,
                     const bool private_connection=false,
                     const bool exit_on_disconnect=true);

        /**
         * Connect and register to a specific bus address.
         * @return 0 on success, -1 on failure.
         */
        int connect (const std::string& bus_address,
                     const bool private_connection=false,
                     const bool exit_on_disconnect=true);

        /**
         * Return true if connected to a bus.
         */
        bool is_connected ();

        /**
         * Disconnect from the bus.
         */
        void disconnect ();

        /**
         * Send a message on the bus.
         * @param msg The DBus message to send.
         * @param reply_cb A callback called when a message reply is received.
         * @param timeout The maximum time in milliseconds to wait for a message reply.
         * @return 0 on success, -1 on failure.
         */
        int send (const Message& msg,
                  std::function<void (ultrabus::Message&)> reply_cb,
                  int timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Send a message on the bus without caring about a message reply.
         * @param msg The DBus message to send.
         * @return 0 on success, -1 on failure.
         */
        int send (const Message& msg);

        /**
         * Send a message on the bus and wait for a reply.
         * @param msg The DBus message to send.
         * @param timeout The maximum time in milliseconds to wait for a message reply.
         * @return A message reply.
         */
        Message send_and_wait (const Message& msg, int timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Return get DBus connection object.
         * Use this method if you're using the DBus api calls directly.
         */
        DBusConnection* handle () {
            return conn;
        }

        /**
         * Return the iohandler_base used by the connection object.
         */
        iomultiplex::iohandler_base& io_handler () {
            return *ioh;
        }


    private:
        DBusConnection* conn;
        std::string conn_addr;
        bool private_connection;
        std::thread worker_thread;

        std::mutex pending_mutex;
        std::map<DBusPendingCall*, std::function<void (Message&)>> pending_messages;

        iomultiplex::iohandler_base* ioh;
        bool internal_io_handler;

        std::mutex wt_mutex;
        iomultiplex::TimerSet* timer_set;
        std::map<DBusWatch*, iomultiplex::FdConnection> bus_io;
        std::map<DBusTimeout*, long> bus_timeout;

        void start_message_dispatcher ();
        void dbus_watch_rx_ready_cb (iomultiplex::io_result_t& ior, DBusWatch* watch);
        void dbus_watch_tx_ready_cb (iomultiplex::io_result_t& ior, DBusWatch* watch);

        static void pending_msg_callback (DBusPendingCall* pending, void* user_data);
        static void dbus_dispatch_status_cb (DBusConnection* c, DBusDispatchStatus status, void* data);

        static dbus_bool_t dbus_add_watch_cb (DBusWatch* watch, void* data);
        static void dbus_remove_watch_cb (DBusWatch* watch, void* data);
        static void dbus_toggled_watch_cb (DBusWatch* watch, void* data);

        static dbus_bool_t dbus_add_timeout_cb (DBusTimeout* timeout, void* data);
        static void dbus_remove_timeout_cb (DBusTimeout* timeout, void* data);
        static void dbus_toggled_timeout_cb (DBusTimeout* timeout, void* data);
    };


}


#endif
