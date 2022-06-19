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
#include <iostream>
#include <unistd.h>
#include <ultrabus.hpp>


//
// Simple example demonstrating a DBus service implementation.
//
// The service will aqcuire the bus name: se.ultramarin.ultrabus.example_service
// It installs an object path of /se/ultramarin/ultrabus/example_service, where
// it implements 4 method calls:
//
//     Interface: org.freedesktop.DBus.Introspectable
//     Method:    introspect (out STRING xml_data)
//
//     Interface: se.ultramarin.ultrabus.example_service
//     Method:    Echo (in ..., out ...)
//     This method sends back incoming parameters to the caller.
//
//     Interface: se.ultramarin.ultrabus.example_service
//     Method:    Log (in STRING log_message)
//     This method sends a log message that the example service prints to standard output.
//
//     Interface: se.ultramarin.ultrabus.example_service
//     Method:    Quit ()
//     This method call signals the example service to stop execution.
//


namespace ubus = ultrabus;
using namespace std;


static constexpr const char* service_name       = "se.ultramarin.ultrabus.example_service";
static constexpr const char* iface_name         = "se.ultramarin.ultrabus.example_service";
static constexpr const char* object_root        = "/se/ultramarin/ultrabus/example_service";
static constexpr const char* service_error_name = "se.ultramarin.ultrabus.Error";

static constexpr const char* introspect_data =
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
    " \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"/se/ultramarin/ultrabus/example_service\">\n"
    "  <interface name=\"" DBUS_INTERFACE_INTROSPECTABLE "\">\n"
    "    <method name=\"Introspect\">\n"
    "      <arg name=\"data\" type=\"s\" direction=\"out\"/>\n"
    "    </method>\n"
    "  </interface>\n"
    "  <interface name=\"se.ultramarin.ultrabus.example_service\">\n"
    "    <method name=\"Echo\">\n"
    "    </method>\n"
    "    <method name=\"Log\">\n"
    "      <arg name=\"message\" type=\"s\" direction=\"in\"/>\n"
    "    </method>\n"
    "    <method name=\"Quit\">\n"
    "    </method>\n"
    "  </interface>\n"
    "</node>\n";


static bool handle_method_call (ubus::Connection& conn, ubus::Message& msg, bool& quit);
static ubus::Message handle_introspect (ubus::Message& msg);
static ubus::Message handle_echo (ubus::Message& msg);
static ubus::Message handle_log (ubus::Message& msg);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    bool quit = false;

    //
    // Create a DBus connection object and connect to the session bus
    //
    ubus::Connection conn;
    if (conn.connect()) {
        cerr << "Unable to connect DBus" << endl;
        exit (1);
    }

    //
    // Request bus name 'se.ultramarin.ultrabus.example_service'
    //
    ubus::org_freedesktop_DBus dbus (conn);
    auto result = dbus.request_name (service_name);
    if (result.err()) {
        cerr << result.what() << endl;
        exit (1);
    }

    //
    // Register a handler for object path '/se/ultramarin/ultrabus/example_service'
    //
    ubus::CallbackObjectHandler oh (conn);
    // First, set a callback that will handle messages sent to the registered object path
    oh.set_message_cb ([&](ubus::Message& msg)->bool
        {
            return handle_method_call (conn, msg, quit);
        });
    // Register the object path
    oh.register_opath (object_root);

    //
    // Service is up and running
    //
    cout << "Service " << service_name << " started." << endl;
    while (!quit)
        usleep (20000);
    cout << "Stopping service " << service_name << endl;

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool handle_method_call (ubus::Connection& conn, ubus::Message& msg, bool& quit)
{
    ubus::Message reply;
    auto iface = msg.interface ();

    if (msg.name() == "Introspect") {
        if (!iface.empty() && iface != DBUS_INTERFACE_INTROSPECTABLE)
            reply = ubus::Message (msg, true, service_error_name, "Invalid interface/method");
        else
            reply = handle_introspect (msg);
    }else{
        if (!iface.empty() && iface != iface_name) {
            reply = ubus::Message (msg, true, service_error_name, "Invalid interface/method");
        }else{
            if (msg.name() == "Echo") {
                reply = handle_echo (msg);
            }
            else if (msg.name() == "Log") {
                reply = handle_log (msg);
            }
            else if (msg.name() == "Quit") {
                reply = ubus::Message (msg, false);
                quit = true;
            }
            else{
                reply = ubus::Message (msg, true, service_error_name, "No such method");
            }
        }
    }
    conn.send (reply);
    return true;
}


//------------------------------------------------------------------------------
// Send back introspect data
//------------------------------------------------------------------------------
static ubus::Message handle_introspect (ubus::Message& msg)
{
    ubus::Message reply (msg, false);
    reply << introspect_data;
    return reply;
}


//------------------------------------------------------------------------------
// Send back a reply with the same arguments as the input arguments
//------------------------------------------------------------------------------
static ubus::Message handle_echo (ubus::Message& msg)
{
    // Echo back arguments
    ubus::Message reply (msg, false);
    auto args = msg.arguments ();
    for (auto& arg : args)
        reply << *arg;

    return reply;
}


//------------------------------------------------------------------------------
// Print a message on standard output of this example service
//------------------------------------------------------------------------------
static ubus::Message handle_log (ubus::Message& msg)
{
    ubus::Message reply;
    ubus::dbus_basic log_message;
    if (!msg.get_args(&log_message, nullptr) || log_message.type_code() != DBUS_TYPE_STRING) {
        reply = ubus::Message (msg, true, service_error_name, "Invalid parameter, expecting s");
    }else{
        reply = ubus::Message (msg, false);
        cout << "Message from " << msg.sender() << ": " << log_message.str() << endl;
    }

    return reply;
}
