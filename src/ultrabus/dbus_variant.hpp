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
#ifndef ULTRABUS_DBUS_VARIANT_HPP
#define ULTRABUS_DBUS_VARIANT_HPP

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <memory>

namespace ultrabus {


    /**
     * DBus variant data type.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#container-types rel="noopener noreferrer" target="_blank">DBus Container Types at dbus.freedesktop.org</a>
     */
    class dbus_variant : public dbus_type {
    public:
        /**
         * Default constructor.
         * Creates a variant without a defined value.
         */
        dbus_variant ();
        virtual ~dbus_variant () = default;              /**< Destructor. */
        dbus_variant (const dbus_variant& v);            /**< Copy constructor. */
        dbus_variant (dbus_variant&& v);                 /**< Move constructor. */

        dbus_variant (const dbus_type& v);               /**< Copy constructor. */
        dbus_variant (dbus_type&& v);                    /**< Move constructor. */

        dbus_variant& operator= (const dbus_variant& v); /**< Assignment operator. */
        dbus_variant& operator= (dbus_variant&& v);      /**< Move operator. */

        virtual bool is_variant () const;                /**< Return true since this i a variant type. */
        virtual int type_code () const;                  /**< Return the DBus type code. */
        dbus_type& value ();                             /**< Return a reference to the value of the variant.
                                                              @throw std::logic_error if no value is set. */
        void value (const dbus_type& value);             /**< Set the value of the variant. */
        void value (dbus_type&& value);                  /**< Set the value of the variant. */
        void value (const dbus_basic& value);            /**< Set the value of the variant to a basic value. */
        void value (dbus_basic&& value);                 /**< Set the value of the variant to a basic value. */
        virtual const std::string str () const;          /**< Return the basic value as a string. */

    protected:
        virtual void copy (const dbus_type& obj);
        virtual void move (dbus_type&& obj);

    private:
        dbus_type_ptr val;
    };

    /**
     * An alias of a shared pointer to a dbus_variant class.
     * @see dbus_variant
     */
    using dbus_variant_ptr = std::shared_ptr<dbus_variant>;

    /**
     * Cast an dbus_type pointer to an dbus_variant pointer.
     * @return A shared pointer to an dbus_variant object or a nullptr.
     */
    static inline dbus_variant_ptr dbus_type_to_dbus_variant (const dbus_type_ptr& ptr) {
        return ptr!=nullptr ? std::dynamic_pointer_cast<dbus_variant>(ptr) : nullptr;
    }



}


#endif
