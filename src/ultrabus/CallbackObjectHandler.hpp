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
#ifndef ULTRABUS_CALLBACKOBJECTHANDLER_HPP
#define ULTRABUS_CALLBACKOBJECTHANDLER_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/ObjectHandler.hpp>


namespace ultrabus {


    /**
     * An object handler that handles DBus method calls in a callback.
     */
    class CallbackObjectHandler : public ObjectHandler {
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
         */
        CallbackObjectHandler (Connection& connection);

        /**
         * Destructor.
         * Remove all added match rules and unregister this handler from the DBus.
         */
        virtual ~CallbackObjectHandler ();

        /**
         * Set a callback to be called for incomming method calls.
         * This will replace any existing callback.
         * @param callback The callback to be called for incomming
         *                 messages, or <code>nullptr</code> to
         *                 remove the callback.
         */
        void set_message_cb (msg_cb_t callback);


    protected:
        /**
         * Called on incoming messages.
         */
        virtual bool on_message (Message& msg);


    private:
        msg_cb_t on_message_cb;
    };

}



#endif
