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
#include <ultrabus/dbus_type_base.hpp>
#include <dbus/dbus.h>


namespace ultrabus {

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_basic () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_struct () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_array () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_variant () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_dict_entry () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_type_base::is_properties () const
    {
        return false;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_type_base::type_code () const
    {
        return DBUS_TYPE_INVALID;
    }


}
