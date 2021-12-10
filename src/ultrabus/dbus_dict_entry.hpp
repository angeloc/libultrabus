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
#ifndef ULTRABUS_DBUS_DICT_ENTRY_HPP
#define ULTRABUS_DBUS_DICT_ENTRY_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <string>
#include <memory>

namespace ultrabus {


    /**
     * Dict entry type.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#container-types rel="noopener noreferrer" target="_blank">DBus Container Types at dbus.freedesktop.org</a>
     */
    class dbus_dict_entry : public dbus_type {
    public:
        /**
         * Default constructor.
         * Creates a dict entry without a defined key or value.
         */
        dbus_dict_entry ();
        virtual ~dbus_dict_entry () = default; /**< Destructor. */
        dbus_dict_entry (const dbus_dict_entry& dict_entry); /**< Copy constructor. */
        dbus_dict_entry (dbus_dict_entry&& dict_entry);      /**< Move constructor. */

        dbus_dict_entry (const dbus_type& dict_entry); /**< Copy constructor. */
        dbus_dict_entry (dbus_type&& dict_entry);      /**< Move constructor. */

        /**
         * Constructor.
         * Set a value for a given key.
         */
        dbus_dict_entry (const dbus_basic& key, const dbus_type& value);

        /**
         * Constructor.
         * Set a basic value for a given key.
         */
        explicit dbus_dict_entry (const dbus_basic& key, const dbus_basic& value);

        dbus_dict_entry& operator= (const dbus_dict_entry& obj); /**< Assignment operator. */
        dbus_dict_entry& operator= (dbus_dict_entry&& obj); /**< Move operator. */

        virtual bool is_dict_entry () const; /**< Return true since this is a dict type. */
        virtual int type_code () const; /**< Return the DBus type code. */
        void set (const dbus_basic& key, const dbus_type& value); /**< Set a value for a given key. */
        void set (const dbus_basic& key, const dbus_basic& value); /**< Set a basic value for a given key. */
        std::string key_signature (); /**< Return the signature of the key. */
        std::string value_signature (); /**< Return the signature of the values. */

        dbus_basic& key (); /**< Return a pointer to the key of the dict entry. */
        void key (const dbus_basic& key); /**< Set the value of the key. */
        void key (dbus_basic&& key); /**< Set the value of the key. */
        dbus_type& value (); /**< Return a pointer to the value of the dict entry. */
        void value (const dbus_type& value); /**< Set the value of the dict entry. */
        void value (dbus_type&& value); /**< Set the value of the dict entry. */
        virtual const std::string str () const; /**< Return the basic value as a string. */

    protected:
        virtual void copy (const dbus_type& rhs);
        virtual void move (dbus_type&& rhs);

        dbus_basic_ptr dict_key;
        dbus_type_ptr dict_value;
    };

    /**
     * An alias of a shared pointer to a dbus_dict_entry class.
     * @see dbus_dict_entry
     */
    using dbus_dict_entry_ptr = std::shared_ptr<dbus_dict_entry>;

    /**
     * Cast an dbus_type pointer to an dbus_dict_entry pointer.
     * @return A shared pointer to an dbus_dict_entry object or a nullptr.
     */
    static inline dbus_dict_entry_ptr dbus_type_to_dbus_dict_entry (const dbus_type_ptr& ptr) {
        return ptr!=nullptr ? std::dynamic_pointer_cast<dbus_dict_entry>(ptr) : nullptr;
    }


}


#endif
