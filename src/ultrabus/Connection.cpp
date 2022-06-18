/*
 * Copyright (C) 2017,2021,2022 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/Connection.hpp>
#include <condition_variable>


#if 0
#  include <cstdio>
#  define DBG_LOG(format, ...) fprintf(stderr, "%s:%s:%d: " format "\n", __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define DBG_LOG(format, ...)
#endif


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Connection::Connection ()
        : conn {nullptr},
          private_connection {false},
          ioh (new iomultiplex::default_iohandler(SIGRTMIN)),
          internal_io_handler {true},
          timer_set (new iomultiplex::TimerSet(*ioh))
    {
        dbus_threads_init_default ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Connection::Connection (iomultiplex::iohandler_base& io_handler)
        : conn {nullptr},
          private_connection {false},
          ioh (&io_handler),
          internal_io_handler {false},
          timer_set (new iomultiplex::TimerSet(*ioh))
    {
        dbus_threads_init_default ();
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    Connection::~Connection ()
    {
        disconnect ();
        delete timer_set;
        if (internal_io_handler)
            delete ioh;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int Connection::connect (const DBusBusType type,
                             const bool private_connection,
                             const bool exit_on_disconnect)
    {
        if (is_connected()) {
            // TBD
            return -1;
        }

        conn_addr = "";
        this->private_connection = private_connection;

        // Connect to the bus
        //
        if (private_connection)
            conn = dbus_bus_get_private (type, nullptr);
        else
            conn = dbus_bus_get (type, nullptr);
        if (!conn)
            return -1;

        dbus_connection_set_exit_on_disconnect (conn, exit_on_disconnect);

        start_message_dispatcher ();
        return 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int Connection::connect (const std::string& bus_address,
                             const bool private_connection,
                             const bool exit_on_disconnect)
    {
        if (is_connected()) {
            // TBD
            return -1;
        }

        this->private_connection = private_connection;

        // Connect to the bus
        //
        conn_addr = bus_address;
        if (private_connection)
            conn = dbus_connection_open_private (conn_addr.c_str(), nullptr);
        else
            conn = dbus_connection_open (conn_addr.c_str(), nullptr);
        if (!conn) {
            conn_addr = "";
            return -1;
        }

        dbus_connection_set_exit_on_disconnect (conn, exit_on_disconnect);

        // Register the connection with the bus
        //
        if (!dbus_bus_register(conn, nullptr)) {
            if (private_connection)
                dbus_connection_close (conn);
            dbus_connection_unref (conn);
            conn_addr = "";
            conn = nullptr;
            return -1;
        }

        start_message_dispatcher ();
        return 0;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool Connection::is_connected ()
    {
        return conn!=nullptr && dbus_connection_get_is_connected(conn)==TRUE;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::disconnect ()
    {
        if (!is_connected())
            return;

        if (private_connection)
            dbus_connection_close (conn);
        else
            dbus_connection_unref (conn);

        pending_mutex.lock ();
        for (auto& e : pending_messages)
            dbus_pending_call_unref (e.first);
        pending_messages.clear ();
        pending_mutex.unlock ();

        {
            std::lock_guard<std::mutex> lock (wt_mutex);
            bus_io.clear ();
            timer_set->clear ();
            bus_timeout.clear ();
        }
        if (internal_io_handler) {
            ioh->stop ();
            ioh->join ();
        }

        conn = nullptr;
        conn_addr = "";
        private_connection = false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Connection::send (const Message& msg,
                          std::function<void (ultrabus::Message&)> reply_cb,
                          int timeout)
    {
        if (!reply_cb)
            return send (msg);

        bool result;
        DBusPendingCall* pending = nullptr;
        std::lock_guard<std::mutex> lock (pending_mutex);
        result = dbus_connection_send_with_reply (conn,
                                                  const_cast<Message&>(msg).handle(),
                                                  &pending,
                                                  timeout);
        if (!result || !pending)
            return -1;

        pending_messages.emplace (pending, reply_cb);
        dbus_pending_call_set_notify (pending, pending_msg_callback, this, nullptr);
        return 0;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Connection::send (const Message& msg)
    {
        uint32_t serial = 0;
        if (dbus_connection_send(conn,
                                 const_cast<Message&>(msg).handle(),
                                 &serial))
        {
            return 0;
        }else{
            return -1;
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message Connection::send_and_wait (const Message& msg, int timeout)
    {
        std::condition_variable cv;
        std::mutex m;
        volatile bool got_reply = false;
        Message reply;

        auto result = send (msg, [&](Message& r){
                                     std::unique_lock<std::mutex> lock (m);
                                     reply = std::move (r);
                                     got_reply = true;
                                     cv.notify_one ();
                                 }, timeout);
        if (result) {
            // Return an error response
            return Message (const_cast<Message&>(msg), true,
                            "ENOMEM", "Unable to allocate memory for message");
        }
        std::unique_lock<std::mutex> lock (m);
        while (!got_reply)
            cv.wait (lock, [&got_reply]{return got_reply;});
        return reply;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::pending_msg_callback (DBusPendingCall* pending, void* user_data)
    {
        Connection* self = static_cast<Connection*> (user_data);
        self->pending_mutex.lock ();

        auto entry = self->pending_messages.find (pending);
        if (entry == self->pending_messages.end()) {
            self->pending_mutex.unlock ();
            return;
        }
        auto callback = entry->second;
        self->pending_messages.erase (entry);
        self->pending_mutex.unlock ();

        if (callback) {
            Message reply (dbus_pending_call_steal_reply(pending));
            reply.dec_ref (); // ref count increased in Message constructor
            callback (reply);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::start_message_dispatcher ()
    {
        if (!conn)
            return;

        if (internal_io_handler)
            ioh->run (true); // Start I/O worker thread

        dbus_connection_set_dispatch_status_function (conn,
                                                      dbus_dispatch_status_cb,
                                                      this,
                                                      nullptr);
        dbus_connection_set_watch_functions (conn,
                                             dbus_add_watch_cb,
                                             dbus_remove_watch_cb,
                                             dbus_toggled_watch_cb,
                                             this,
                                             nullptr);
        dbus_connection_set_timeout_functions (conn,
                                               dbus_add_timeout_cb,
                                               dbus_remove_timeout_cb,
                                               dbus_toggled_timeout_cb,
                                               this,
                                               nullptr);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_dispatch_status_cb (DBusConnection* c, DBusDispatchStatus status, void* data)
    {
        switch (status) {
        case DBUS_DISPATCH_DATA_REMAINS:
            DBG_LOG ("Status: DBUS_DISPATCH_DATA_REMAINS");
            break;
        case DBUS_DISPATCH_COMPLETE:
            DBG_LOG ("Status: DBUS_DISPATCH_COMPLETE");
            break;
        case DBUS_DISPATCH_NEED_MEMORY:
            DBG_LOG ("Status: DBUS_DISPATCH_NEED_MEMORY");
            break;
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_watch_rx_ready_cb (iomultiplex::io_result_t& ior, DBusWatch* watch)
    {
        DBG_LOG ("RX ready");

        dbus_watch_handle (watch, DBUS_WATCH_READABLE);
        while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS)
            ;

        std::lock_guard<std::mutex> lock (wt_mutex);
        if (bus_io.find(watch) == bus_io.end())
            return; // Watch removed in the dispatch function

        auto flags = dbus_watch_get_flags (watch);
        bool enabled = dbus_watch_get_enabled (watch);
        if (enabled && (flags & DBUS_WATCH_READABLE)) {
            ior.conn.wait_for_rx ([this, watch](iomultiplex::io_result_t& ior)->bool{
                                      if (!ior.errnum)
                                          dbus_watch_rx_ready_cb (ior, watch);
                                      return false;
                                  });
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_watch_tx_ready_cb (iomultiplex::io_result_t& ior, DBusWatch* watch)
    {
        DBG_LOG ("TX ready");

        dbus_watch_handle (watch, DBUS_WATCH_WRITABLE);

        std::lock_guard<std::mutex> lock (wt_mutex);
        if (bus_io.find(watch) == bus_io.end())
            return; // Watch removed in the watch_handle function

        auto flags = dbus_watch_get_flags (watch);
        bool enabled = dbus_watch_get_enabled (watch);
        if (enabled && (flags&DBUS_WATCH_WRITABLE)) {
            ior.conn.wait_for_tx ([this, watch](iomultiplex::io_result_t& ior)->bool{
                                      if (!ior.errnum)
                                          dbus_watch_tx_ready_cb (ior, watch);
                                      return false;
                                  });
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_bool_t Connection::dbus_add_watch_cb (DBusWatch* watch, void* data)
    {
        DBG_LOG ("Add watch");

        int fd = dbus_watch_get_unix_fd (watch);
        if (fd < 0)
            return true;

        Connection* self = static_cast<Connection*> (data);
        std::lock_guard<std::mutex> lock (self->wt_mutex);

        auto entry = self->bus_io.find (watch);
        if (entry == self->bus_io.end())
            entry = self->bus_io.emplace(watch, iomultiplex::FdConnection(*self->ioh, fd, true)).first;
        iomultiplex::FdConnection& fdc = entry->second;

        if (dbus_watch_get_enabled(watch)) {
            auto flags = dbus_watch_get_flags (watch);
            if (flags & DBUS_WATCH_READABLE) {
                fdc.wait_for_rx ([self, watch](iomultiplex::io_result_t& ior)->bool{
                                     if (!ior.errnum)
                                         self->dbus_watch_rx_ready_cb (ior, watch);
                                     return false;
                                 });
            }
            if (flags & DBUS_WATCH_WRITABLE) {
                fdc.wait_for_tx ([self, watch](iomultiplex::io_result_t& ior)->bool{
                                     if (!ior.errnum)
                                         self->dbus_watch_tx_ready_cb (ior, watch);
                                     return false;
                                 });
            }
        }

        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_remove_watch_cb (DBusWatch* watch, void* data)
    {
        DBG_LOG ("Remove watch");

        Connection* self = static_cast<Connection*> (data);

        std::lock_guard<std::mutex> lock (self->wt_mutex);
        auto entry = self->bus_io.find (watch);
        if (entry != self->bus_io.end())
            self->bus_io.erase (entry);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_toggled_watch_cb (DBusWatch* watch, void* data)
    {
        DBG_LOG ("Toggle watch");

        Connection* self = static_cast<Connection*> (data);

        std::lock_guard<std::mutex> lock (self->wt_mutex);
        auto entry = self->bus_io.find (watch);
        if (entry == self->bus_io.end())
            return;
        iomultiplex::FdConnection& fdc = entry->second;

        bool enabled = dbus_watch_get_enabled (watch);
        if (enabled) {
            auto flags = dbus_watch_get_flags (watch);
            if (flags & DBUS_WATCH_READABLE) {
                fdc.wait_for_rx ([self, watch](iomultiplex::io_result_t& ior)->bool{
                                     if (!ior.errnum)
                                         self->dbus_watch_rx_ready_cb (ior, watch);
                                     return false;
                                 });
            }
            if (flags & DBUS_WATCH_WRITABLE) {
                fdc.wait_for_tx ([self, watch](iomultiplex::io_result_t& ior)->bool{
                                     if (!ior.errnum)
                                         self->dbus_watch_tx_ready_cb (ior, watch);
                                     return false;
                                 });
            }
        }else{
            fdc.cancel ();
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_bool_t Connection::dbus_add_timeout_cb (DBusTimeout* timeout, void* data)
    {
        DBG_LOG ("Add timer");
        Connection* self = static_cast<Connection*> (data);
        std::lock_guard<std::mutex> lock (self->wt_mutex);

        auto entry = self->bus_timeout.find (timeout);
        if (entry == self->bus_timeout.end()) {
            entry = self->bus_timeout.emplace(timeout, -1).first;
        }
        long& timer_id = entry->second;

        if (dbus_timeout_get_enabled(timeout)) {
            if (timer_id >= 0)
                self->timer_set->cancel (timer_id);
            auto interval = dbus_timeout_get_interval (timeout);
            if (interval > 0) {
                DBG_LOG ("Set timer: %d", interval);
                timer_id = self->timer_set->set (interval,
                                                 interval,
                                                 [self, timeout](iomultiplex::TimerSet& ts,
                                                                 long timer_id)
                {
                    // Timer expiration callback
                    DBG_LOG ("timed out");
                    dbus_timeout_handle (timeout);
                    while (dbus_connection_dispatch(self->conn) == DBUS_DISPATCH_DATA_REMAINS)
                        ;
                });
            }
        }else{
            if (timer_id >= 0)
                self->timer_set->cancel (timer_id);
            DBG_LOG ("Cancel timer");
        }

        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_remove_timeout_cb (DBusTimeout* timeout, void* data)
    {
        DBG_LOG ("Remove timer");
        Connection* self = static_cast<Connection*> (data);

        std::lock_guard<std::mutex> lock (self->wt_mutex);
        auto entry = self->bus_timeout.find (timeout);
        if (entry != self->bus_timeout.end()) {
            long& timer_id = entry->second;
            if (timer_id >= 0)
                self->timer_set->cancel (timer_id);
            self->bus_timeout.erase (entry);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Connection::dbus_toggled_timeout_cb (DBusTimeout* timeout, void* data)
    {
        DBG_LOG ("Toggle timer");
        Connection* self = static_cast<Connection*> (data);

        std::lock_guard<std::mutex> lock (self->wt_mutex);
        auto entry = self->bus_timeout.find (timeout);
        if (entry == self->bus_timeout.end())
            return;
        long& timer_id = entry->second;

        if (dbus_timeout_get_enabled(timeout)) {
            auto interval = dbus_timeout_get_interval (timeout);
            if (interval > 0) {
                DBG_LOG ("Set timer: %d", interval);
                if (timer_id >= 0)
                    self->timer_set->cancel (timer_id);
                timer_id = self->timer_set->set (interval,
                                                 interval,
                                                 [self, timeout](iomultiplex::TimerSet& ts,
                                                                 long timer_id)
                {
                    // Timer expiration callback
                    DBG_LOG ("timed out");
                    dbus_timeout_handle (timeout);
                    while (dbus_connection_dispatch(self->conn) == DBUS_DISPATCH_DATA_REMAINS)
                        ;
                });
            }
        }else{
            DBG_LOG ("Cancel timer");
            if (timer_id >= 0)
                self->timer_set->cancel (timer_id);
        }
    }


}
