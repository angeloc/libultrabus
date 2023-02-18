/*
 * Copyright (C) 2021-2023 Dan Arrhenius <dan@ultramarin.se>
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
#ifndef ULTRABUS_ORG_FREEDESKTOP_DBUS_PROPERTIES_HPP
#define ULTRABUS_ORG_FREEDESKTOP_DBUS_PROPERTIES_HPP

#include <ultrabus/Connection.hpp>
#include <ultrabus/MessageHandler.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <ultrabus/Properties.hpp>
#include <ultrabus/retvalue.hpp>
#include <functional>
#include <string>
#include <map>
#include <set>


namespace ultrabus {

    /**
     * Proxy class to use methods in standard DBus interface <code>org.freedesktop.DBus.Properties</code>.
     *
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-properties
     *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.Properties</a>
     */
    class org_freedesktop_DBus_Properties : public MessageHandler {
    public:
        /**
         * Callback called when properties of a DBus object have changed.
         * @param interface The interface of the changed properties.
         * @param changed_properties Properties that have changed.
         * @param invalidated_properties Names of properties that are invalidated.
         */
        using properties_changed_cb = std::function<void (const std::string& interface,
                                                          Properties& changed_properties,
                                                          std::set<std::string>& invalidated_properties)>;

        /**
         * Constructor.
         * Constructs an object used for calling methods in the standard
         * DBUs interface <code>org.freedesktop.DBus.Properties</code>.
         * @param connection A DBus connection.
         * @param msg_timeout A timeout value in milliseconds used
         *                    when sending messages on the bus using
         *                    this instance.
         *                    DBUS_TIMEOUT_USE_DEFAULT means that a
         *                    default timeout value will be used by
         *                    the underlaying dbus library (libdbus-1).
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-properties
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - org.freedesktop.DBus.Properties</a>
         */
        org_freedesktop_DBus_Properties (Connection& connection,
                                         const int msg_timeout=DBUS_TIMEOUT_USE_DEFAULT);

        /**
         * Destructor.
         * Clean up resources. All callbacks added with method
         * <code>add_properties_changed_cb()</code> are removed.
         */
        virtual ~org_freedesktop_DBus_Properties () = default;

        /**
         * Get all properties of a DBus object.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get_all</code> method instead.
         *
         * @param service A bus name.
         * @param object_path The object owning the properties.
         * @param interface The interface the porperties belong to.
         * @return All properties of the specified DBus object.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        retvalue<Properties> get_all (const std::string& service,
                                      const std::string& object_path,
                                      const std::string& interface);

        /**
         * Asynchronous call to get all properties of a DBus object.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param object_path The object owning the properties.
         * @param interface The interface the porperties belong to.
         * @param cb This callback is called when a result
         *           is received on the message bus.
         *           <br/>The parameter to the callback is the
         *           same as the return value from the
         *           corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        int get_all (const std::string& service,
                     const std::string& object_path,
                     const std::string& interface,
                     std::function<void (retvalue<Properties>& retval)> cb);

        /**
         * Get the value of a property of a DBus object.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>get</code> method instead.
         *
         * @param service A bus name.
         * @param object_path The object owning the property.
         * @param interface The interface the porperty belong to.
         * @param property_name The name of the property.
         * @return A property value.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        retvalue<dbus_variant> get (const std::string& service,
                                    const std::string& object_path,
                                    const std::string& interface,
                                    const std::string& property_name);

        /**
         * Asynchronous call to get the value of a property of a DBus object.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param object_path The object owning the property.
         * @param interface The interface the porperty belong to.
         * @param property_name The name of the property.
         * @param cb This callback is called when a result
         *           is received on the message bus.
         *           <br/>The parameter to the callback is the
         *           same as the return value from the
         *           corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        int get (const std::string& service,
                 const std::string& object_path,
                 const std::string& interface,
                 const std::string& property_name,
                 std::function<void (retvalue<dbus_variant>& retval)> cb);

        /**
         * Set a property of a DBus object.
         *
         * <i>Note:</i> Do not call this method from within
         * callback functions in libultrabus, it will cause a deadlock.
         * Use the asynchronous <code>set</code> method instead.
         *
         * @param service A bus name.
         * @param object_path The object owning the property.
         * @param interface The interface the porperty belong to.
         * @param property_name The name of the property.
         * @param value The value of the property.
         * @return A retvalue object with an int value of 0 on success, and -1 on error.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        template<typename T>
        retvalue<int> set (const std::string& service,
                           const std::string& object_path,
                           const std::string& interface,
                           const std::string& property_name,
                           const T& value)
        {
            return set_impl (service, object_path, interface, property_name, value);
        }

        /**
         * Asynchronous call to set a property of a DBus object.
         *
         * This method queues a message on the message bus and returns immediately,
         * the result is handled in a callback function.
         * <br/>This method can safely be called from
         * within other callback functions in libultrabus.
         *
         * @param service A bus name.
         * @param object_path The object owning the property.
         * @param interface The interface the porperty belong to.
         * @param property_name The name of the property.
         * @param value The value of the property.
         * @param cb This callback is called when a result
         *           is received on the message bus.
         *           <br/>The parameter to the callback is the
         *           same as the return value from the
         *           corresponding synchronous method.
         * @return 0 if the message was queued on the message bus,
         *         -1 if failing to queue the message.
         *
         * @see retvalue
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-names
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Message Bus Names</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Valid Object Paths</a>
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface
         *       rel="noopener noreferrer" target="_blank">D-Bus Specification - Interface names</a>
         */
        template<typename T>
        int set (const std::string& service,
                 const std::string& object_path,
                 const std::string& interface,
                 const std::string& property_name,
                 const T& value,
                 std::function<void (retvalue<int>& retval)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name, value, cb);
        }

        /**
         * Set a callback to be called when the properties of a DBus object changes.
         * @param service A bus name.
         * @param object_path The object owning the properties we wish to monitor.
         * @param callback This callback will be called when properties changes.
         * @see remove_properties_changed_cb
         */
        int add_properties_changed_cb (const std::string& service,
                                       const std::string& object_path,
                                       properties_changed_cb callback);

        /**
         * Remove a property changed callback.
         * @param service A bus name.
         * @param object_path The object owning the properties we are monitoring.
         * @see add_properties_changed_cb
         */
        int remove_properties_changed_cb (const std::string& service,
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
         * @param milliseconds A timeout value in milliseconds.
         *                     DBUS_TIMEOUT_USE_DEFAULT means that a default
         *                     timeout value is used by the underlaying
         *                     dbus library (libdbus-1).
         */
        void msg_timeout (int milliseconds) {
            timeout = milliseconds;
        }


    protected:
        virtual bool on_signal (Message& msg);


    private:
        int timeout;

        //                 bus_name     opath         callback
        std::map<std::pair<std::string, std::string>, properties_changed_cb> props_changed_callbacks;
        std::mutex props_changed_mutex;


        template<typename T>
        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const T& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(dbus_basic((const T&)value)));
        }

        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const dbus_array& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(value));
        }

        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const dbus_dict_entry& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(value));
        }

        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const dbus_struct& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(value));
        }

        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const dbus_type& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(value));
        }

        retvalue<int> set_impl (const std::string& service, const std::string& object_path,
                                const std::string& interface, const std::string& property_name, const Properties& value)
        {
            return set_impl (service, object_path, interface, property_name, dbus_variant(const_cast<Properties&>(value).data()));
        }

        retvalue<int> set_impl (const std::string& service,
                                const std::string& object_path,
                                const std::string& interface,
                                const std::string& property_name,
                                const dbus_variant& value);




        template<typename T>
        int set_impl_async (const std::string& service,
                            const std::string& object_path,
                            const std::string& interface,
                            const std::string& property_name,
                            const T& value,
                            std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service,
                                   object_path,
                                   interface,
                                   property_name,
                                   dbus_variant(dbus_basic((const T&)value)),
                                   cb);
        }

        int set_impl_async (const std::string& service, const std::string& object_path,
                            const std::string& interface, const std::string& property_name,
                            const dbus_array& value, std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name, dbus_variant(value), cb);
        }

        int set_impl_async (const std::string& service, const std::string& object_path,
                            const std::string& interface, const std::string& property_name,
                            const dbus_dict_entry& value, std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name, dbus_variant(value), cb);
        }

        int set_impl_async (const std::string& service, const std::string& object_path,
                            const std::string& interface, const std::string& property_name,
                            const dbus_struct& value, std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name, dbus_variant(value), cb);
        }

        int set_impl_async (const std::string& service, const std::string& object_path,
                            const std::string& interface, const std::string& property_name,
                            const dbus_type& value, std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name, dbus_variant(value), cb);
        }

        int set_impl_async (const std::string& service, const std::string& object_path,
                            const std::string& interface, const std::string& property_name,
                            const Properties& value, std::function<void (retvalue<int>& result)> cb)
        {
            return set_impl_async (service, object_path, interface, property_name,
                                   dbus_variant(const_cast<Properties&>(value).data()), cb);
        }

        int set_impl_async (const std::string& service,
                            const std::string& object_path,
                            const std::string& interface,
                            const std::string& property_name,
                            const dbus_variant& value,
                            std::function<void (retvalue<int>& result)> cb);
    };


}
#endif
