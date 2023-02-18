/*
 * Copyright (C) 2017,2021,2023 Dan Arrhenius <dan@ultramarin.se>
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
#ifndef ULTRABUS_CALLBACKMESSAGEHANDLER_HPP
#define ULTRABUS_CALLBACKMESSAGEHANDLER_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/MessageHandler.hpp>
#include <functional>


namespace ultrabus {


    /**
     * Handle incomming DBus messages in callbacks.
     */
    class CallbackMessageHandler : public MessageHandler{
    public:
        /**
         * Callback function for received messages.
         * @param msg The received message.
         * @return <code>true</code> if the message was handled by
         *         the callback. If not, return <code>false</code>.
         */
        using msg_cb_t = std::function<bool (Message& msg)>;

        /**
         * Constructor.
         * @param connection A DBus connection object.
         * @param method_call_cb Callback to be called on incoming DBus method calls.
         * @param signal_cb Callback to be called on incoming DBus signals.
         */
        CallbackMessageHandler (Connection& connection,
                                msg_cb_t method_call_cb = nullptr,
                                msg_cb_t signal_cb = nullptr);

        /**
         * Destructor.
         */
        virtual ~CallbackMessageHandler () = default;

        /**
         * Set a callback to be called for incomming
         * method calls on the message bus.
         *
         * <b>Note!</b><br/>
         * If a callback is installed using method
         * <code>set_message_cb()</code>, all messages will be
         * handled in that callback and this callback will never
         * be called.
         *
         * @param callback The callback to be called for incomming
         *                 method call messages, or <code>nullptr</code>
         *                 to remove the callback.
         */
        void set_method_call_cb (msg_cb_t callback);

        /**
         * Set a callback to be called for incomming
         * signals on the message bus.
         *
         * <b>Note!</b><br/>
         * If a callback is installed using method
         * <code>set_message_cb()</code>, all messages will be
         * handled in that callback and this callback will never
         * be called.
         *
         * @param callback The callback to be called for incomming
         *                 signals, or <code>nullptr</code>
         *                 to remove the callback.
         */
        void set_signal_cb (msg_cb_t callback);

        /**
         * Set a callback to be called for all
         * incomming messages on the message bus.
         *
         * <b>Note!</b><br/>
         * If a callback is installed using this method,
         * all messages will be handled in that callback and any
         * callback installed using methods <code>set_method_call_cb()</code>
         * or <code>set_signal_cb()</code> will never be called.
         *
         * @param callback The callback to be called for incomming
         *                 messages, or <code>nullptr</code>
         *                 to remove the callback.
         */
        void set_message_cb (msg_cb_t callback);


    protected:
        virtual bool on_method_call (Message& msg);
        virtual bool on_signal (Message& msg);
        virtual bool on_message (Message& msg);


    private:
        msg_cb_t on_method_call_cb;
        msg_cb_t on_signal_cb;
        msg_cb_t on_message_cb;
    };

}



#endif
