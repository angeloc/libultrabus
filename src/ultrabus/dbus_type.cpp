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
#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_TYPE_TRACE

#ifdef ENABLE_DBUS_TYPE_TRACE
#  include <cstdio>
#  define DBUS_TYPE_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_TYPE_TRACE(format, ...)
#endif


namespace ultrabus {


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type::dbus_type (const std::string& signature)
        : sig {signature}
    {
        DBUS_TYPE_TRACE ("dbus_type::dbus_type(const std::string&) - Constructor");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& dbus_type::operator= (const dbus_type& rhs)
    {
        DBUS_TYPE_TRACE ("dbus_type::operator=(const dbus_type&) - asignment operator");
        if (this != &rhs)
            copy (rhs);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& dbus_type::operator= (dbus_type&& rhs)
    {
        DBUS_TYPE_TRACE ("dbus_type::operator=(dbus_type&&) - move operator");
        if (this != &rhs)
            move (std::forward<dbus_type&&>(rhs));
        return *this;
    }

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string dbus_type::signature () const
    {
        return sig;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_type::copy (const dbus_type& obj)
    {
        DBUS_TYPE_TRACE ("dbus_type::copy - Copy obj: %s", obj.str().c_str());
        sig = obj.sig;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_type::move (dbus_type&& obj)
    {
        DBUS_TYPE_TRACE ("dbus_type::move(dbus_type&& obj) - obj: %s",
                         obj.str().c_str());

        sig = std::move (obj.sig);
        obj.sig = "";
    }


}
