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
#ifndef ULTRABUS_DBUS_TYPE_BASE_HPP
#define ULTRABUS_DBUS_TYPE_BASE_HPP

#include <string>

namespace ultrabus {

    /**
     * Base class of the wrapper classes for the DBus data types and the Properties helper class.
     */
    class dbus_type_base {
    public:
        dbus_type_base () = default;          /**< Default constructor. */
        virtual ~dbus_type_base () = default; /**< Default destructor. */
        virtual bool is_basic ()      const;  /**< true if this is a DBus basic type. */
        virtual bool is_struct ()     const;  /**< true if this is a DBus struct type. */
        virtual bool is_array ()      const;  /**< true if this is a DBus array type. */
        virtual bool is_variant ()    const;  /**< true if this is a DBus variant type. */
        virtual bool is_dict_entry () const;  /**< true if this is a DBus dict_entry type. */
        virtual bool is_properties () const;  /**< true if this is a wrapper to handle DBus properties. */
        /**
         * Return the DBus type code.
         * In case of this base class and additional subclasses that doesn't represent
         * a DBus type, DBUS_TYPE_INVALID will be returned.
         * @return A DBus type code.
         * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#type-system rel="noopener noreferrer" target="_blank">DBus Type System at dbus.freedesktop.org</a>
         */
        virtual int type_code () const;
        virtual const std::string str () const = 0; /**< Return a string representation of the object. */
    };

}

#endif
