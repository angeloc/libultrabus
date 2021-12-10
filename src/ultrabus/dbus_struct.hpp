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
#ifndef ULTRABUS_DBUS_STRUCT_HPP
#define ULTRABUS_DBUS_STRUCT_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <vector>
#include <memory>
#include <sys/types.h>

namespace ultrabus {


    /**
     * Wrapper for a DBus struct type.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#container-types rel="noopener noreferrer" target="_blank">DBus Container Types at dbus.freedesktop.org</a>
     */
    class dbus_struct : public dbus_type {
    public:
        dbus_struct ();                                /**< Default constructor. Creates an empty struct. */
        virtual ~dbus_struct () = default;             /**< Destructor. */
        dbus_struct (const dbus_struct& s);            /**< Copy constructor. */
        dbus_struct (dbus_struct&& s);                 /**< Move constructor. */

        dbus_struct (const dbus_type& s);              /**< Copy constructor. */
        dbus_struct (dbus_type&& s);                   /**< Move constructor. */

        dbus_struct& operator= (const dbus_struct& s); /**< Assignment operator. */
        dbus_struct& operator= (dbus_struct&& s);      /**< Move operator. */

        virtual bool is_struct () const;               /**< Return true since this is a struct type. */
        virtual int type_code () const;                /**< Return the DBus type code, 'r'. */

        size_t size () const;                          /**< Return the number of members in the struct. */
        void add (const dbus_type& t);                 /**< Add a member to the struct. */
        void remove (size_t n);                        /**< Remove the n:th member in the struct.
                                                            @throw std::out_of_range if <code>n</code> is out of range. */
        dbus_type& operator[] (size_t n);              /**< Return a reference to
                                                            the n:th member in the struct.
                                                            @throw std::out_of_range if <code>n</code> is out of range. */
        virtual const std::string str () const;        /**< Return a string representation of the object. */

    protected:
        virtual void copy (const dbus_type& obj);
        virtual void move (dbus_type&& obj);

    private:
        std::vector<dbus_type_ptr> elements;
    };

    /**
     * An alias of a shared pointer to a dbus_struct class.
     * @see dbus_struct
     */
    using dbus_struct_ptr = std::shared_ptr<dbus_struct>;

    /**
     * Cast an dbus_type pointer to an dbus_struct pointer.
     * @return A shared pointer to an dbus_struct object or a nullptr.
     */
    static inline dbus_struct_ptr dbus_type_to_dbus_struct (const dbus_type_ptr& ptr) {
        return ptr!=nullptr ? std::dynamic_pointer_cast<dbus_struct>(ptr) : nullptr;
    }



}


#endif
