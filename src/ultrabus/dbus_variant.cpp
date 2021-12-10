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
#include <ultrabus/utils.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <stdexcept>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_VARIANT_TRACE

#ifdef ENABLE_DBUS_VARIANT_TRACE
#  include <cstdio>
#  define DBUS_VARIANT_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_VARIANT_TRACE(format, ...)
#endif


namespace ultrabus {


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant::dbus_variant ()
        : dbus_type (DBUS_TYPE_VARIANT_AS_STRING)
    {
        DBUS_VARIANT_TRACE ("dbus_variant - Default constructor");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant::dbus_variant (const dbus_variant& v)
        : dbus_type (DBUS_TYPE_VARIANT_AS_STRING)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::dbus_variant(const dbus_variant&) - "
                            "copy constructor");
        copy (v);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant::dbus_variant (dbus_variant&& v)
        : dbus_type (DBUS_TYPE_VARIANT_AS_STRING)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::dbus_variant(dbus_variant&&) - "
                            "move constructor");
        move (std::forward<dbus_variant>(v));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant::dbus_variant (const dbus_type& v)
        : dbus_type (DBUS_TYPE_VARIANT_AS_STRING)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::dbus_variant(const dbus_type&) - "
                            "copy constructor");
        copy (v);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant::dbus_variant (dbus_type&& v)
        : dbus_type (DBUS_TYPE_VARIANT_AS_STRING)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::dbus_variant(dbus_type&&) - "
                            "move constructor");
        move (std::forward<dbus_type>(v));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant& dbus_variant::operator= (const dbus_variant& v)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::operator=(const dbus_variant&) - "
                            "assignment operator");
        if (&v != this)
            copy (v);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_variant& dbus_variant::operator= (dbus_variant&& v)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::operator=(dbus_variant&&) - "
                            "move operator");
        if (&v != this)
            move (std::forward<dbus_variant>(v));
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_variant::is_variant () const
    {
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_variant::type_code () const
    {
        return DBUS_TYPE_VARIANT;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& dbus_variant::value ()
    {
        if (val == nullptr)
            throw std::logic_error ("dbus_variant value not initialized");
        return *val;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_variant::value (const dbus_type& new_value)
    {
        if (new_value.is_variant()) {
            const dbus_variant& rval = dynamic_cast<const dbus_variant&> (new_value);
            value (const_cast<dbus_variant&>(rval).value());
        }else{
            val = clone_dbus_type (new_value);
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_variant::value (dbus_type&& new_value)
    {
        if (new_value.is_variant()) {
            dbus_variant& v = dynamic_cast<dbus_variant&> (new_value);
            val = std::move (v.val);
        }else{
            val = clone_dbus_type (std::forward<dbus_type>(new_value));
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_variant::value (const dbus_basic& val)
    {
        this->val.reset (new dbus_basic(val));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_variant::value (dbus_basic&& val)
    {
        this->val.reset (new dbus_basic(std::forward<dbus_basic>(val)));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const std::string dbus_variant::str () const
    {
        return val==nullptr ? "" : val->str();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_variant::copy (const dbus_type& obj)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::copy(const dbus_type& obj) - obj: %s",
                            obj.str().c_str());

        if (obj.is_variant()) {
            const dbus_variant& rhs = dynamic_cast<const dbus_variant&> (obj);
            val = clone_dbus_type (*rhs.val);
        }else{
            val = clone_dbus_type (obj);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_variant::move (dbus_type&& obj)
    {
        DBUS_VARIANT_TRACE ("dbus_variant::move(dbus_type&& obj) - obj: %s",
                            obj.str().c_str());

        if (obj.is_variant()) {
            dbus_variant&& rhs = dynamic_cast<dbus_variant&&> (obj);
            val = std::move (rhs.val);
        }else{
            val = clone_dbus_type (obj);
        }
    }


}
