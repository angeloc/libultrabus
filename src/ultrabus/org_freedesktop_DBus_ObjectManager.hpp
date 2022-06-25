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
#ifndef ULTRABUS_ORG_FREEDESKTOP_DBUS_OBJECTMANAGER_HPP
#define ULTRABUS_ORG_FREEDESKTOP_DBUS_OBJECTMANAGER_HPP

#include <ultrabus/MessageHandler.hpp>
#include <ultrabus/Connection.hpp>
#include <ultrabus/Properties.hpp>
#include <ultrabus/retvalue.hpp>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <map>


namespace ultrabus {


    /**
     * A map of object paths, mapping interfaces, mapping properties.
     * map<object_path, map<interface, properties>>.<br/>
     * - Object/path/1
     *   - Interface.1
     *     - Property_1
     *     - Property_2
     *   - Interface.2
     *     - Property_1
     * - Object/path/2
     *   - Interface.1
     *     - Property_1
     * - ...
     */
    using managed_objects_t = std::map<std::string, std::map<std::string, Properties>>;


    /**
     * Proxy class for using the standard DBus interface <code>org.freedesktop.DBus.ObjectManager</code>.
     *
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-objectmanager
     *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.ObjectManager</a>
     */
    class org_freedesktop_DBus_ObjectManager : public MessageHandler {
    public:

        /**
         * Callback called when new a object is added
         * or when an object gains one of more interfaces.
         * @param opath The object path of the added/changed object.
         * @param ifaces A map with interface names and their properties.
         */
        using iface_added_cb = std::function<void (const std::string& opath,
                                                   std::map<std::string, Properties>& ifaces)>;

        /**
         * Callback called when an object is
         * removed or loses one or more interfaces.
         * @param opath The object that is removed or has lost interfaces.
         * @param ifaces The removed interfaces.
         */
        using iface_removed_cb = std::function<void (const std::string& opath,
                                                     std::vector<std::string>& ifaces)>;

        /**
         * Constructor.
         * Constructs an object used for calling methods in the standard
         * DBUs interface <code>org.freedesktop.DBus.ObjectManager</code>.
         * @param connection A DBus connection.
         * @param msg_timeout A timeout value in milliseconds used
         *                    when sending messages on the bus using
         *                    this instance.
         *                    DBUS_TIMEOUT_USE_DEFAULT means that a
         *                    default timeout value will be used by
         *                    the underlaying dbus library (libdbus-1).
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-objectmanager
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.ObjectManager</a>
         */
        org_freedesktop_DBus_ObjectManager (Connection& connection,
                                            const int msg_timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Destructor.
         */
        virtual ~org_freedesktop_DBus_ObjectManager () = default;

        /**
         * Get all sub-objects and properties of an object in a service.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_managed_objects</code> method instead.
         *
         * @param service A bus name.
         * @param object_path The root of the object sub-tree we want to probe.
         * @return A map of object paths and their respective interfaces and properties.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        retvalue<managed_objects_t> get_managed_objects (const std::string& service,
                                                         const std::string& object_path="/");

        /**
         * Asynchronous call to get all sub-objects and
         * properties of an object in a service.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param object_path The root of the object sub-tree we want to probe.
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
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        int get_managed_objects (const std::string& service,
                                 const std::string& object_path,
                                 std::function<void (retvalue<managed_objects_t>& result)> callback);

        /**
         * Set a callback that will be called when new a object is
         * added or when an object gains one of more interfaces.
         * If a callback is already set for this service and object path,
         * it will be replaced with this callback.
         *
         * @param service The bus name of the service managing the objects.
         * @param object_path The path of the parent object we want
         *                    to receive infomtation about.
         * @param callback A callback function. If this is a
         *                 <code>nullptr</code>, the callback
         *                 will be removed if previously set.
         * @return 0 if the callback was successfully set.
         *         -1 on error or if the service or object path is invalid.
         *
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        int set_interfaces_added_callback (const std::string& service,
                                           const std::string& object_path,
                                           iface_added_cb callback);

        /**
         * Set a callback that will be called when an object
         * is removed or loses one or more interfaces.
         * If a callback is already set for this service and object path,
         * it will be replaced with this callback.
         *
         * @param service The bus name of the service managing the objects.
         * @param object_path The path of the parent object we want
         *                    to receive infomtation about.
         * @param callback A callback function. If this is a
         *                 <code>nullptr</code>, the callback
         *                 will be removed if previously set.
         * @return 0 if the callback was successfully set.
         *         -1 on error or if the service or object path is invalid.
         *
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         */
        int set_interfaces_removed_callback (const std::string& service,
                                             const std::string& object_path,
                                             iface_removed_cb cb);

        /**
         * Remove a callback that was set with method
         * <code>set_interfaces_added_callback</code>.
         * @param service The bus name of the service managing the objects.
         * @param object_path The path of the parent object we
         *                    are receiving infomtation about.
         * @return 0 if the callback was successfully removed.
         *         -1 on error or if the service or object path is invalid.
         * @see set_interfaces_added_callback
         */
        int remove_interfaces_added_callback (const std::string& service,
                                              const std::string& object_path);

        /**
         * Remove a callback that was set with method
         * <code>set_interfaces_removed_callback</code>.
         * @param service The bus name of the service managing the objects.
         * @param object_path The path of the parent object we
         *                    are receiving infomtation about.
         * @return 0 if the callback was successfully removed.
         *         -1 on error or if the service or object path is invalid.
         * @see set_interfaces_removed_callback
         */
        int remove_interfaces_removed_callback (const std::string& service,
                                                const std::string& object_path);

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


    protected:
        virtual bool on_signal (Message& msg);


    private:
        int timeout;
        std::mutex iface_mutex;
        std::map<std::pair<std::string, std::string>, iface_added_cb>   iface_added_callbacks;
        std::map<std::pair<std::string, std::string>, iface_removed_cb> iface_removed_callbacks;

        void handle_added_ifaces (Message& msg, iface_added_cb cb);
        void handle_removed_ifaces (Message& msg, iface_removed_cb cb);
    };


}
#endif
