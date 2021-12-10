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
#ifndef ULTRABUS_DBUS_TYPE_HPP
#define ULTRABUS_DBUS_TYPE_HPP

#include <ultrabus/dbus_type_base.hpp>
#include <string>
#include <memory>

namespace ultrabus {


    /**
     * This class is the base class of all DBus data types.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#type-system rel="noopener noreferrer" target="_blank">DBus Type System at dbus.freedesktop.org</a>
     */
    class dbus_type : public dbus_type_base {
    public:
        dbus_type () = default;                    /**< Default constructor. Creates an invalid DBus type. */
        virtual ~dbus_type () = default;           /**< Default destructor. */

        dbus_type& operator= (const dbus_type& t); /**< Assignment operator. */
        dbus_type& operator= (dbus_type&& t);      /**< Move operator. */
        std::string signature () const;            /**< Return the DBus signature of the type. */

    protected:
        explicit dbus_type (const std::string& signature); /**< Construct a DBus type and set a specific signature. */
        virtual void copy (const dbus_type& obj);  /**< Copy another DBus data object. */
        virtual void move (dbus_type&& obj);       /**< Move another DBus data object to this one. */

        std::string sig; /**< DBus signature. */
    };


    /**
     * An alias of a shared pointer to a dbus_type class.
     * @see dbus_type
     */
    using dbus_type_ptr = std::shared_ptr<dbus_type>;


}

#endif
