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
#include <ultrabus/dbus_struct.hpp>
#include <stdexcept>
#include <sstream>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_STRUCT_TRACE

#ifdef ENABLE_DBUS_STRUCT_TRACE
#  include <cstdio>
#  define DBUS_STRUCT_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_STRUCT_TRACE(format, ...)
#endif


namespace ultrabus {


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct::dbus_struct ()
        : dbus_type (DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                     DBUS_STRUCT_END_CHAR_AS_STRING)
    {
        DBUS_STRUCT_TRACE ("dbus_struct - Default constructor");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct::dbus_struct (const dbus_struct& s)
        : dbus_type (DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                     DBUS_STRUCT_END_CHAR_AS_STRING)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::dbus_struct(const dbus_struct&) - copy constructor");
        copy (s);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct::dbus_struct (dbus_struct&& s)
        : dbus_type (DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                     DBUS_STRUCT_END_CHAR_AS_STRING)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::dbus_struct(dbus_struct&&) - move constructor");
        move (std::forward<dbus_struct>(s));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct::dbus_struct (const dbus_type& s)
        : dbus_type (DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                     DBUS_STRUCT_END_CHAR_AS_STRING)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::dbus_struct(const dbus_type&) - copy constructor");
        copy (s);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct::dbus_struct (dbus_type&& s)
        : dbus_type (DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                     DBUS_STRUCT_END_CHAR_AS_STRING)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::dbus_struct(dbus_type&) - move constructor");
        move (std::forward<dbus_type>(s));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct& dbus_struct::operator= (const dbus_struct& s)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::::operator=(const dbus_type&) - assignment operator");
        if (&s != this)
            copy (s);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_struct& dbus_struct::operator= (dbus_struct&& s)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::::operator=(dbus_type&&) - move operator");
        if (&s != this)
            move (std::forward<dbus_struct>(s));
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_struct::add (const dbus_type& t)
    {
        auto element = clone_dbus_type (t);
        if (element != nullptr) {
            elements.push_back (element);
            sig.pop_back ();
            sig.append (element->signature());
            sig.append (DBUS_STRUCT_END_CHAR_AS_STRING);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_struct::remove (size_t n)
    {
        auto i = elements.begin ();
        i += n;
        if (i == elements.end())
            throw std::out_of_range ("ultrabus::dbus_struct::remove - index out of bounds");
        i->reset ();
        elements.erase (i);
        sig = DBUS_STRUCT_BEGIN_CHAR_AS_STRING;
        for (auto& e : elements)
            sig.append (e->signature());
        sig.append (DBUS_STRUCT_END_CHAR_AS_STRING);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_struct::is_struct () const
    {
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_struct::type_code () const
    {
        return DBUS_TYPE_STRUCT;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    size_t dbus_struct::size () const
    {
        return elements.size ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& dbus_struct::operator[] (size_t n)
    {
        if (n >= elements.size())
            throw std::out_of_range ("ultrabus::dbus_struct[] - index out of bounds");
        return *elements[n];
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const std::string dbus_struct::str () const
    {
        std::stringstream ss;
        ss << '(';
        for (auto i=elements.begin(); i!=elements.end();) {
            ss << (*i)->str ();
            if (++i != elements.end())
                ss << ',';
        }
        ss << ')';
        return ss.str ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_struct::copy (const dbus_type& obj)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::copy(const dbus_type& obj) - obj: %s",
                           obj.str().c_str());

        if (!obj.is_struct()) {
            std::stringstream ss;
            ss << "Can't assign a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_struct.";
            throw std::invalid_argument (ss.str());
        }
        const dbus_struct& rhs = dynamic_cast<const dbus_struct&> (obj);
        sig = DBUS_STRUCT_BEGIN_CHAR_AS_STRING DBUS_STRUCT_END_CHAR_AS_STRING;
        // Do a deep copy of the elements.
        elements.clear ();
        for (auto& element : rhs.elements) {
            add (*element);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_struct::move (dbus_type&& obj)
    {
        DBUS_STRUCT_TRACE ("dbus_struct::move(dbus_type&& obj) - obj: %s",
                           obj.str().c_str());

        if (!obj.is_dict_entry()) {
            std::stringstream ss;
            ss << "Can't move a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_struct.";
            throw std::invalid_argument (ss.str());
        }
        dbus_struct&& rhs {dynamic_cast<dbus_struct&&>(obj)};
        sig = std::move (rhs.sig);
        elements = std::move (rhs.elements);
        rhs.sig = DBUS_STRUCT_BEGIN_CHAR_AS_STRING DBUS_STRUCT_END_CHAR_AS_STRING;
    }


}
