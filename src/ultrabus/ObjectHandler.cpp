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
#include <ultrabus/ObjectHandler.hpp>
#include <cstring>


#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif


namespace ultrabus {



    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    ObjectHandler::ObjectHandler (Connection& connection)
        : conn (connection)
    {
        // Initialize function pointers in DBusObjectPathVTable
        auto* vtable = dynamic_cast<DBusObjectPathVTable*> (this);
        memset (vtable, 0, sizeof(*vtable));
        unregister_function = dbus_on_unregister;
        message_function    = dbus_on_message;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    ObjectHandler::~ObjectHandler ()
    {
        std::lock_guard<std::mutex> lock (opaths_lock);
        for (auto& opath : opaths)
            dbus_connection_unregister_object_path (conn.handle(), opath.c_str());
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int ObjectHandler::register_opath (const std::string& opath, bool fallback)
    {
        std::lock_guard<std::mutex> lock (opaths_lock);
        if (opaths.find(opath) != opaths.end())
            return 0; // Already added

        bool result;
        if (fallback) {
            result = dbus_connection_try_register_fallback (conn.handle(),
                                                            opath.c_str(),
                                                            this,
                                                            this,
                                                            nullptr);
        }else{
            result = dbus_connection_try_register_object_path (conn.handle(),
                                                               opath.c_str(),
                                                               this,
                                                               this,
                                                               nullptr);
        }
        if (!result)
            return -1;

        opaths.emplace (opath);

        return 0;
    }


    //--------------------------------------------------------------------------
    // Static method
    //--------------------------------------------------------------------------
    bool ObjectHandler::on_message (Message& msg)
    {
        return false;
    }


    //--------------------------------------------------------------------------
    // Static method
    //--------------------------------------------------------------------------
    void ObjectHandler::dbus_on_unregister (DBusConnection* connection,
                                            void* user_data)
    {
        //auto* self = static_cast<ObjectHandler*> (user_data);
    }


    //--------------------------------------------------------------------------
    // Static method
    //--------------------------------------------------------------------------
    DBusHandlerResult ObjectHandler::dbus_on_message (DBusConnection* connection,
                                                      DBusMessage* message,
                                                      void* user_data)
    {
        auto* self = static_cast<ObjectHandler*> (user_data);
        Message msg (message);

        return self->on_message(msg) ?
            DBUS_HANDLER_RESULT_HANDLED : DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }


}
