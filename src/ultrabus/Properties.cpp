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
#include <ultrabus/Properties.hpp>
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <typeinfo>


//#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", \
                                     (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif


namespace ultrabus {


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties::Properties ()
        : props ("{sv}")
    {
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties::Properties (const Properties& p)
    {
        props = p.props;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties::Properties (Properties&& p)
    {
        props = std::forward<dbus_array> (p.props);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties::Properties (const dbus_type& dict)
    {
        if (dict.signature() == "a{sv}")
            props = dict;
        else
            props.clear ("{sv}");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties::Properties (dbus_type&& dict)
    {
        if (dict.signature() == "a{sv}")
            props = std::forward<dbus_type> (dict);
        else
            props.clear ("{sv}");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties& Properties::operator= (const Properties& p)
    {
        if (this != &p)
            props = p.props;
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties& Properties::operator= (Properties&& p)
    {
        if (this != &p)
            props = std::forward<dbus_type> (p.props);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties& Properties::operator= (const dbus_array& p)
    {
        if (p.signature() == "a{sv}")
            props = p;
        else
            throw std::bad_cast ();
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Properties& Properties::operator= (dbus_array&& p)
    {
        if (p.signature() == "a{sv}")
            props = std::forward<dbus_type> (p);
        else
            throw std::bad_cast ();
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool Properties::is_properties () const
    {
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::size_t Properties::size () const
    {
        return props.size ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool Properties::empty () const
    {
        return props.empty ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::pair<std::string, dbus_type&> Properties::operator[] (std::size_t i)
    {
        dbus_dict_entry& entry = dynamic_cast<dbus_dict_entry&> (props[i]);
        dbus_basic& name = entry.key ();
        dbus_variant& variant = dynamic_cast<dbus_variant&> (entry.value());
        return std::pair<std::string, dbus_type&> (name.str(), variant.value());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& Properties::operator[] (const std::string& property_name)
    {
        for (auto& entry : props) {
            dbus_dict_entry& p = dynamic_cast<dbus_dict_entry&> (entry);
            dbus_basic& name = p.key ();
            if (name.str() != property_name)
                continue;

            dbus_variant& variant = dynamic_cast<dbus_variant&> (p.value());
            return variant.value ();
        }

        // Didn't find the property
        throw std::out_of_range ("ultrabus::Properties[] - property not found");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Properties::get (const std::string& property_name, dbus_type& value)
    {
        for (auto& entry : props) {
            dbus_dict_entry& p = dynamic_cast<dbus_dict_entry&> (entry);
            dbus_basic& name = p.key ();
            if (name.str() != property_name)
                continue;

            dbus_variant& variant = dynamic_cast<dbus_variant&> (p.value());
            dbus_type& val = variant.value ();

            // Property name found, check DBus type mismatch
            //
            if (!val.is_basic() || !value.is_basic()) {
                if (val.type_code() != value.type_code()) {
                    // Can't assign a non-basic DBus type to a different DBus type
                    return -1;
                }
            }
            value = val;
            return 0;
        }

        // Didn't find the property
        return -1;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Properties::set (const std::string& property, const dbus_type& value)
    {
        dbus_variant* prop_val = nullptr;

        // Find the property (if any)
        //
        for (auto& prop : props) {
            dbus_dict_entry& p = dynamic_cast<dbus_dict_entry&> (prop);
            dbus_basic& name = p.key ();
            if (name.str() == property) {
                prop_val = dynamic_cast<dbus_variant*> (&p.value());
                break;
            }
        }

        // Set the property, or add a new one if not found
        //
        if (prop_val == nullptr) {
            if (value.is_variant())
                props.add (dbus_dict_entry(dbus_basic(property), value));
            else
                props.add (dbus_dict_entry(dbus_basic(property), dbus_variant(value)));
        }else{
            if (value.is_variant())
                *prop_val = dynamic_cast<const dbus_variant&> (value);
            else
                *prop_val = dbus_variant (value);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Properties::remove (const std::string& property)
    {
        for (size_t i=0; i<props.size(); ++i) {
            dbus_dict_entry& p = dynamic_cast<dbus_dict_entry&> (props[i]);
            dbus_basic& name = p.key ();
            if (name.str() != property) {
                props.remove (i);
                break;
            }
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Properties::clear ()
    {
        props.clear ("{sv}");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Properties::reset (const dbus_type& dict)
    {
        if (dict.signature() != "a{sv}") {
            props.clear ("{sv}");
            return -1;
        }
        props = dict;
        return 0;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Properties::reset (dbus_type&& dict)
    {
        if (dict.signature() != "a{sv}") {
            props.clear ("{sv}");
            return -1;
        }
        props = std::forward<dbus_type> (dict);
        return 0;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_array& Properties::data ()
    {
        if (props.signature() != "a{sv}")
            props.clear ("{sv}");
        return props;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const std::string Properties::str () const
    {
        return props.str ();
    }

}
