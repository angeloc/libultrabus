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
#include <ultrabus/org_freedesktop_DBus_Introspectable.hpp>

namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    org_freedesktop_DBus_Introspectable::org_freedesktop_DBus_Introspectable (Connection& connection)
        : conn (connection)
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    retvalue<std::string> org_freedesktop_DBus_Introspectable::introspect (const std::string& service,
                                                                           const std::string& object_path)
    {
        retvalue<std::string> retval;

        Message msg (service, object_path, DBUS_INTERFACE_INTROSPECTABLE, "Introspect");

        auto reply = conn.send_and_wait (msg);
        if (reply.is_error()) {
            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
            return retval;
        }
        dbus_basic reply_arg;
        if (!reply.get_args(&reply_arg, nullptr)) {
            retval.err (-1, "Invalid message reply argument");
            return retval;
        }
        retval = reply_arg.str ();
        return retval;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int org_freedesktop_DBus_Introspectable::introspect (const std::string& service,
                                                         const std::string& object_path,
                                                         std::function<void (retvalue<std::string>& result)> callback)
    {
        Message msg (service, object_path, DBUS_INTERFACE_INTROSPECTABLE, "Introspect");
        if (!callback) {
            return conn.send (msg);
        }else{
            return conn.send (msg, [callback](Message& reply)
                {
                    retvalue<std::string> retval;
                    dbus_basic data;
                    if (reply.is_error() || !reply.get_args(&data, nullptr)) {
                        if (reply.is_error())
                            retval.err (-1, reply.error_name() + std::string(": ") + reply.error_msg());
                        else
                            retval.err (-1, "Invalid message reply argument");
                    }else{
                        // We have a result
                        retval = data.str ();
                    }
                    callback (retval);
                });
        }
    }


}
