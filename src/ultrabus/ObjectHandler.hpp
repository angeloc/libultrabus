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
#ifndef ULTRABUS_OBJECTHANDLER_HPP
#define ULTRABUS_OBJECTHANDLER_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/Connection.hpp>
#include <ultrabus/Message.hpp>
#include <string>
#include <mutex>
#include <set>
#include <dbus/dbus.h>


namespace ultrabus {


    /**
     * Base class for DBus object handlers.
     */
    class ObjectHandler : public DBusObjectPathVTable {
    public:
        /**
         * Constructor.
         * @param connection A DBus connection object.
         */
        ObjectHandler (Connection& connection);

        /**
         * Destructor.
         * Remove all added match rules and unregister this handler from the DBus.
         */
        virtual ~ObjectHandler ();

        /**
         * Register an object path to be handled by this instance.
         * @param opath The object path to register.
         * @param fallback If <code>true</code>, the <code>on_message()</code>
         *                 will be called for all objects in this subdirectory.
         *                 If <code>false</code>, the <code>on_message()</code>
         *                 will be called for this specific object path.
         * @return 0 on success, -1 on failure.
         */
        int register_opath (const std::string& opath, bool fallback=false);


    protected:
        Connection& conn; /**< Reference to a Connection object. */

        /**
         * Called on incoming messages.
         */
        virtual bool on_message (Message& msg);


    private:
        std::set<std::string> opaths;
        std::mutex opaths_lock;

        static void dbus_on_unregister (DBusConnection* connection, void* user_data);
        static DBusHandlerResult dbus_on_message (DBusConnection* connection, DBusMessage* message, void* user_data);
    };

}



#endif
