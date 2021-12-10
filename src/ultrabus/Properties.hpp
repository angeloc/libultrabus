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
#ifndef ULTRABUS_PROPERTIES_HPP
#define ULTRABUS_PROPERTIES_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type_base.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_array.hpp>
#include <memory>


namespace ultrabus {


    /**
     * A wrapper class to handle DBus properties, a dbus_array object with signature "a{sv}" (DICT<STRING,VARIANT>).
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-properties rel="noopener noreferrer" target="_blank">org.freedesktop.DBus.Properties at dbus.freedesktop.org</a>
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#container-types rel="noopener noreferrer" target="_blank">DBus Container Types at dbus.freedesktop.org</a>
     */
    class Properties : public dbus_type_base {
    public:
        /**
         * Construct an empty properties object.
         */
        Properties ();

        /**
         * Copy contructor.
         */
        Properties (const Properties& properties);

        /**
         * Move contructor.
         */
        Properties (Properties&& properties);

        /**
         * Construct a Properties object with the contents of a DICT<STRING,VARIANT>.
         * @param dict A DBus array with signature 'a{sv}' (DICT<STRING,VARIANT>) to copy.
         *             If the parameter has another signature an empty Properties object will be created.
         */
        Properties (const dbus_type& dict);

        /**
         * Construct a Properties object with the contents of a DICT<STRING,VARIANT>.
         * @param dict A DBus array with signature 'a{sv}' (DICT<STRING,VARIANT>) to move.
         *             If the parameter has another signature an empty Properties object will be created.
         */
        Properties (dbus_type&& dict);

        /**
         * Assignment operator.
         */
        Properties& operator= (const Properties& p);

        /**
         * Assignment(move) operator.
         */
        Properties& operator= (Properties&& p);

        /**
         * Assignment operator.
         */
        Properties& operator= (const dbus_array& p);

        /**
         * Assignment operator.
         */
        Properties& operator= (dbus_array&& p);

        /**
         * Always returns <code>true</code>.
         */
        virtual bool is_properties () const;

        /**
         * Return the number of properties.
         */
        std::size_t size () const;

        /**
         * Check if the number of properties is zero.
         */
        bool empty () const;

        /**
         * Return the property name and value on the n'th property in the list of properties.
         * @throw std::out_of_range
         */
        std::pair<std::string, dbus_type&> operator[] (std::size_t i);

        /**
         * Return a reference to a property with a specific name.
         * @throw std::out_of_range if no such property exists.
         */
        dbus_type& operator[] (const std::string& property);

        /**
         * Get the value of a named property.
         * If the property doesn't exist, -1 will be returned.
         * @param property The name of the property value we want.
         * @param value A reference to a dbus_type object where to store the property value.
         *              This object must be of the same DBus type as the property value.
         * @return 0 on success. -1 if the property wasn't found or if the property is
         *                       of a different DBus type than the parameter <code>value</code>.
         */
        int get (const std::string& property, dbus_type& value);

        /**
         * Set the value of a property. If the property doesn't exist a new property is added.
         * @param property The name of the property we want to set/add.
         * @param value The value of the property.
         */
        void set (const std::string& property, const dbus_type& value);

        /**
         * Set the value of a property. If the property doesn't exist a new property is added.
         * @param property The name of the property we want to set/add.
         * @param value The value of the property.
         */
        void set (const std::string& property, const std::string& value) {
            set (property, dbus_basic(value));
        }

        /**
         * Set the value of a property. If the property doesn't exist a new property is added.
         * @param property The name of the property we want to set/add.
         * @param value The value of the property.
         */
        void set (const std::string& property, const char* value) {
            set (property, dbus_basic(value));
        }

        /**
         * Set the value of a property. If the property doesn't exist a new property is added.
         * @param property The name of the property we want to set/add.
         * @param value The value of the property.
         */
        void set (const std::string& property, const int32_t value) {
            set (property, dbus_basic(value));
        }

        /**
         * Set the value of a property. If the property doesn't exist a new property is added.
         * @param property The name of the property we want to set/add.
         * @param value The value of the property.
         */
        void set (const std::string& property, const bool value) {
            set (property, dbus_basic(value));
        }

        /**
         * Remove a property. If the property doesn't exist nothing is done.
         * @param property The name of the property we want to remove.
         */
        void remove (const std::string& property);

        /**
         * Remove all properties.
         */
        void clear ();

        /**
         * Replace the contents of this property array with a copy of another.
         * @param dict The DBus array object to copy.
         * @return 0 on success. -1 if <code>dict</code> is not a DBus array
         *         with signature 'a{sv}'.
         */
        int reset (const dbus_type& dict);

        /**
         * Move the contents of another DBus property object to this object.
         * @param dict The DBus array object to move.
         * @return 0 on success. -1 if <code>dict</code> is not a DBus array
         *         with signature 'a{sv}'.
         */
        int reset (dbus_type&& dict);

        /**
         * Return a reference to the underlaying dbus_array object containing the properties.
         * @return An dbus_array object containing the properties.
         */
        dbus_array& data ();

        virtual const std::string str () const; /**< Return the basic value as a string. */


    private:
        dbus_array props;
    };

    /**
     * An alias of a shared pointer to a Properties class.
     * @see Properties
     */
    using properties_ptr = std::shared_ptr<Properties>;



}


#endif
