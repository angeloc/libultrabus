/*
 * Copyright (C) 2017,2021,2023 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/utils.hpp>
#include <stdexcept>
#include <sstream>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_DICT_ENTRY_TRACE

#ifdef ENABLE_DBUS_DICT_ENTRY_TRACE
#  include <cstdio>
#  define DBUS_DICT_ENTRY_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_DICT_ENTRY_TRACE(format, ...)
#endif


namespace ultrabus {

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry ()
        : dbus_type (DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                     DBUS_DICT_ENTRY_END_CHAR_AS_STRING)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry - Default constructor");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (const dbus_dict_entry& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(const dbus_dict_entry&) - copy constructor");
        copy (de);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (dbus_dict_entry&& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(dbus_dict_entry&&) - move constructor");
        move (std::forward<dbus_dict_entry>(de));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (const dbus_type& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(const dbus_type&) - copy constructor");
        copy (de);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (dbus_type&& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(dbus_type&&) - move constructor");
        move (std::forward<dbus_type>(de));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (const dbus_basic& key, const dbus_type& value)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(const dbus_basic&, const dbus_type&) - constructor");
        set (key, value);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry::dbus_dict_entry (const dbus_basic& key, const dbus_basic& value)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::dbus_dict_entry(const dbus_basic&, const dbus_basic&) - constructor");
        set (key, value);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry& dbus_dict_entry::operator= (const dbus_dict_entry& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::operator=(const dbus_dict_entry&) - assignment operator");
        if (&de == this)
            return *this;
        copy (de);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_dict_entry& dbus_dict_entry::operator= (dbus_dict_entry&& de)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::operator=(dbus_dict_entry&&) - move operator");
        if (&de != this)
            move (std::forward<dbus_dict_entry>(de));
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_dict_entry::is_dict_entry () const
    {
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_dict_entry::type_code () const
    {
        return DBUS_TYPE_DICT_ENTRY;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::set (const dbus_basic& key, const dbus_type& value)
    {
        dict_key = std::make_shared<dbus_basic> (key);
        this->value (value);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::set (const dbus_basic& key, const dbus_basic& value)
    {
        set (key, dynamic_cast<const dbus_type&>(value));
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string dbus_dict_entry::key_signature ()
    {
        return dict_key==nullptr ? "" : dict_key->signature();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string dbus_dict_entry::value_signature ()
    {
        return dict_value==nullptr ? "" : dict_value->signature();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_dict_entry::key ()
    {
        return *dict_key;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::key (const dbus_basic& key)
    {
        dict_key = std::make_shared<dbus_basic> (key);
        sig = std::string(DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING) +
            key_signature() + value_signature() +
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::key (dbus_basic&& key)
    {
        dict_key = std::make_shared<dbus_basic> (std::forward<dbus_basic>(key));
        sig = std::string(DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING) +
            key_signature() + value_signature() +
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type& dbus_dict_entry::value ()
    {
        return *dict_value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::value (const dbus_type& value)
    {
        dict_value = clone_dbus_type (value);
        sig = std::string(DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING) +
            key_signature() + value_signature() +
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void dbus_dict_entry::value (dbus_type&& value)
    {
        dict_value = clone_dbus_type (std::forward<dbus_type>(value));
        sig = std::string(DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING) +
            key_signature() + value_signature() +
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const std::string dbus_dict_entry::str () const
    {
        std::stringstream ss;
        ss << '(' << dict_key->str() << ',' << dict_value->str() << ')';
        return ss.str ();
    }


    /*-----------------------------------------------------------------------
     * Make a deep copy another dbus_dict_entry object.
     * @param obj The dbus_type object to copy.
     *            If <code>obj</code> is not a reference to a
     *            dbus_dict_entry object, en exception is thrown.
     */
    void dbus_dict_entry::copy (const dbus_type& obj)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::copy(const dbus_type& obj) - obj: %s",
                               obj.str().c_str());

        if (!obj.is_dict_entry()) {
            std::stringstream ss;
            ss << "Can't assign a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_dict_entry.";
            throw std::invalid_argument (ss.str());
        }

        const dbus_dict_entry& rhs {dynamic_cast<const dbus_dict_entry&>(obj)};
        if (rhs.dict_key != nullptr)
            dict_key = std::make_shared<dbus_basic> (*rhs.dict_key);
        else
            dict_key = nullptr;
        if (rhs.dict_value != nullptr)
            dict_value = clone_dbus_type (*rhs.dict_value);
        else
            dict_value = nullptr;
        sig = std::string(DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING) +
            (dict_key==nullptr   ? "" : dict_key->signature()) +
            (dict_value==nullptr ? "" : dict_value->signature()) +
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


    /*-----------------------------------------------------------------------
     * Move another dbus_dict_entry object to this instance.
     * @param obj The dbus_type object to move.
     *            If <code>obj</code> is not a reference to a
     *            dbus_dict_entry object, en exception is thrown.
     */
    void dbus_dict_entry::move (dbus_type&& obj)
    {
        DBUS_DICT_ENTRY_TRACE ("dbus_dict_entry::move(dbus_type&& obj) - obj: %s",
                               obj.str().c_str());

        if (!obj.is_dict_entry()) {
            std::stringstream ss;
            ss << "Can't move a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_dict_entry.";
            throw std::invalid_argument (ss.str());
        }
        dbus_dict_entry&& rhs {dynamic_cast<dbus_dict_entry&&>(obj)};
        dict_key   = std::move (rhs.dict_key);
        dict_value = std::move (rhs.dict_value);
        sig        = std::move (rhs.sig);
        rhs.sig    = DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING DBUS_DICT_ENTRY_END_CHAR_AS_STRING;
    }


}
