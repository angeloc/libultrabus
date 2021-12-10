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
#include <ultrabus/dbus_basic.hpp>
#include <sstream>
#include <stdexcept>
#include <dbus/dbus.h>


//#define ENABLE_DBUS_BASIC_TRACE

#ifdef ENABLE_DBUS_BASIC_TRACE
#  include <cstdio>
#  define DBUS_BASIC_TRACE(format, ...) fprintf(stderr, format "\n", ## __VA_ARGS__)
#else
#  define DBUS_BASIC_TRACE(format, ...)
#endif


namespace ultrabus {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool operator< (const dbus_basic& lhs, const dbus_basic& rhs)
    {
        if (lhs.sig==DBUS_TYPE_STRING_AS_STRING ||
            lhs.sig==DBUS_TYPE_OBJECT_PATH_AS_STRING ||
            lhs.sig==DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            return lhs.str_val < rhs.str_val;
        }
        else if (lhs.sig == DBUS_TYPE_BYTE_AS_STRING)
            return lhs.val.byt < rhs.val.byt;
        else if (lhs.sig == DBUS_TYPE_BOOLEAN_AS_STRING)
            return lhs.val.bool_val < rhs.val.bool_val;
        else if (lhs.sig == DBUS_TYPE_INT16_AS_STRING)
            return lhs.val.i16 < rhs.val.i16;
        else if (lhs.sig == DBUS_TYPE_UINT16_AS_STRING)
            return lhs.val.u16 < rhs.val.u16;
        else if (lhs.sig == DBUS_TYPE_INT32_AS_STRING)
            return lhs.val.i32 < rhs.val.i32;
        else if (lhs.sig == DBUS_TYPE_UINT32_AS_STRING)
            return lhs.val.u32 < rhs.val.u32;
        else if (lhs.sig == DBUS_TYPE_INT64_AS_STRING)
            return lhs.val.i64 < rhs.val.i64;
        else if (lhs.sig == DBUS_TYPE_UINT64_AS_STRING)
            return lhs.val.u64 < rhs.val.u64;
        else if (lhs.sig == DBUS_TYPE_DOUBLE_AS_STRING)
            return lhs.val.dbl < rhs.val.dbl;
        else if (lhs.sig == DBUS_TYPE_UNIX_FD_AS_STRING)
            return lhs.val.fd < rhs.val.fd;
        else
            return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic ()
        : dbus_type (DBUS_TYPE_INT32_AS_STRING)
    {
        DBUS_BASIC_TRACE ("dbus_basic - Default constructor");
        val.u64 = 0LL;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const std::string& signature, const DBusBasicValue& value)
        : dbus_type (signature)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const std::string&, const DBusBasicValue&) - Constructor");
        val = value;
        if (sig==DBUS_TYPE_STRING_AS_STRING ||
            sig==DBUS_TYPE_OBJECT_PATH_AS_STRING ||
            sig==DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            str_val = std::string (val.str);
            val.str = const_cast<char*> (str_val.c_str());
        }else{
            str_val = "";
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const dbus_basic& mb)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const dbus_basic&) - copy constructor");
        copy (mb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (dbus_basic&& mb)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const dbus_basic&& mb) - move constructor");
        move (std::forward<dbus_basic>(mb));
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const dbus_type& mb)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const dbus_type& mb) - copy constructor");
        copy (mb);
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (dbus_type&& mb)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const dbus_type&& mb) - move constructor");
        move (std::forward<dbus_type>(mb));
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const uint8_t value)
        : dbus_type (DBUS_TYPE_BYTE_AS_STRING),
          str_val {""}
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const uint8_t value) - constructor");
        val.byt = value;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const int16_t value)
        : dbus_type (DBUS_TYPE_INT16_AS_STRING),
          str_val {""}
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const int16_t) - constructor");
        val.i16 = value;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    dbus_basic::dbus_basic (const uint16_t value)
        : dbus_type (DBUS_TYPE_UINT16_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const uint16_t) - constructor");
        val.u16 = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const int32_t value)
        : dbus_type (DBUS_TYPE_INT32_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const int32_t) - constructor");
        val.i32 = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const uint32_t value)
        : dbus_type (DBUS_TYPE_UINT32_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const uint32_t) - constructor");
        val.u32 = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const bool value)
        : dbus_type (DBUS_TYPE_BOOLEAN_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const bool) - constructor");
        val.bool_val = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const int64_t value)
        : dbus_type (DBUS_TYPE_INT64_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const int64_t) - constructor");
        val.i64 = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const uint64_t value)
        : dbus_type (DBUS_TYPE_UINT64_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const uint64_t) - constructor");
        val.u64 = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const double value)
        : dbus_type (DBUS_TYPE_DOUBLE_AS_STRING),
          str_val ("")
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const double) - constructor");
        val.dbl = value;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const std::string& value, int str_type)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const std::string&, int) - constructor");
        if (str_type == DBUS_TYPE_OBJECT_PATH)
            sig = DBUS_TYPE_OBJECT_PATH_AS_STRING;
        else if (str_type == DBUS_TYPE_SIGNATURE)
            sig = DBUS_TYPE_SIGNATURE_AS_STRING;
        else
            sig = DBUS_TYPE_STRING_AS_STRING;
        str_val = value;
        val.str = const_cast<char*> (str_val.c_str());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic::dbus_basic (const char* value, int str_type)
    {
        DBUS_BASIC_TRACE ("dbus_basic::dbus_basic(const char*, int) - constructor");
        if (str_type == DBUS_TYPE_OBJECT_PATH)
            sig = DBUS_TYPE_OBJECT_PATH_AS_STRING;
        else if (str_type == DBUS_TYPE_SIGNATURE)
            sig = DBUS_TYPE_SIGNATURE_AS_STRING;
        else
            sig = DBUS_TYPE_STRING_AS_STRING;
        str_val = std::string (value);
        val.str = const_cast<char*> (str_val.c_str());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::operator= (const dbus_basic& obj)
    {
        DBUS_BASIC_TRACE ("dbus_basic::operator=(const dbus_basic&) - assignment operator");
        if (&obj != this)
            copy (obj);
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::operator= (dbus_basic&& obj)
    {
        DBUS_BASIC_TRACE ("dbus_basic::operator=(dbus_basic&&) - move operator");
        if (&obj != this)
            move (std::forward<dbus_basic>(obj));
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_basic::operator== (const dbus_basic& mb)
    {
        if (sig != mb.sig)
            return false;

        if (sig==DBUS_TYPE_STRING_AS_STRING ||
            sig==DBUS_TYPE_OBJECT_PATH_AS_STRING ||
            sig==DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            return str_val == mb.str_val;
        }
        else if (sig==DBUS_TYPE_DOUBLE_AS_STRING) {
            return val.dbl == mb.val.dbl;
        }
        return val.u64 == mb.val.u64;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_basic::is_basic () const
    {
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_basic::type_code () const
    {
        return static_cast<int> (sig[0]);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint8_t dbus_basic::byt () const
    {
        return val.byt;
    }
    dbus_basic& dbus_basic::byt (uint8_t value)
    {
        sig     = DBUS_TYPE_BYTE_AS_STRING;
        val.byt = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int16_t dbus_basic::i16 () const
    {
        return val.i16;
    }
    dbus_basic& dbus_basic::i16 (int16_t value)
    {
        sig     = DBUS_TYPE_INT16_AS_STRING;
        val.i16 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint16_t dbus_basic::u16 () const
    {
        return val.u16;
    }
    dbus_basic& dbus_basic::u16 (uint16_t value)
    {
        sig     = DBUS_TYPE_UINT16_AS_STRING;
        val.u16 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int32_t dbus_basic::i32 () const
    {
        return val.i32;
    }
    dbus_basic& dbus_basic::i32 (int32_t value)
    {
        sig     = DBUS_TYPE_INT32_AS_STRING;
        val.i32 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint32_t dbus_basic::u32 () const
    {
        return val.u32;
    }
    dbus_basic& dbus_basic::u32 (uint32_t value)
    {
        sig     = DBUS_TYPE_UINT32_AS_STRING;
        val.u32 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool dbus_basic::boolean () const
    {
        return static_cast<bool> (val.bool_val);
    }
    dbus_basic& dbus_basic::boolean (bool value)
    {
        sig          = DBUS_TYPE_BOOLEAN_AS_STRING;
        val.bool_val = value;
        str_val      = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int64_t dbus_basic::i64 () const
    {
        return val.i64;
    }
    dbus_basic& dbus_basic::i64 (int64_t value)
    {
        sig     = DBUS_TYPE_INT64_AS_STRING;
        val.i64 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint64_t dbus_basic::u64 () const
    {
        return val.u64;
    }
    dbus_basic& dbus_basic::u64 (uint64_t value)
    {
        sig     = DBUS_TYPE_UINT64_AS_STRING;
        val.u64 = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    double dbus_basic::dbl () const
    {
        return val.dbl;
    }
    dbus_basic& dbus_basic::dbl (double value)
    {
        sig     = DBUS_TYPE_DOUBLE_AS_STRING;
        val.dbl = value;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const std::string dbus_basic::str () const
    {
        std::stringstream ss;

        if (sig==DBUS_TYPE_STRING_AS_STRING ||
            sig==DBUS_TYPE_OBJECT_PATH_AS_STRING ||
            sig==DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            ss << str_val;
        }
        else if (sig == DBUS_TYPE_BYTE_AS_STRING)
            ss << val.byt;
        else if (sig == DBUS_TYPE_BOOLEAN_AS_STRING)
            ss << (val.bool_val ? "true" : "false");
        else if (sig == DBUS_TYPE_INT16_AS_STRING)
            ss << val.i16;
        else if (sig == DBUS_TYPE_UINT16_AS_STRING)
            ss << val.u16;
        else if (sig == DBUS_TYPE_INT32_AS_STRING)
            ss << val.i32;
        else if (sig == DBUS_TYPE_UINT32_AS_STRING)
            ss << val.u32;
        else if (sig == DBUS_TYPE_INT64_AS_STRING)
            ss << val.i64;
        else if (sig == DBUS_TYPE_UINT64_AS_STRING)
            ss << val.u64;
        else if (sig == DBUS_TYPE_DOUBLE_AS_STRING)
            ss << val.dbl;
        else if (sig == DBUS_TYPE_UNIX_FD_AS_STRING)
            ss << val.fd;

        return ss.str ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::str (const std::string& value, int str_type)
    {
        sig     = str_type;
        str_val = value;
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::str (std::string&& value, int str_type)
    {
        sig     = str_type;
        str_val = std::forward<std::string> (value);
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::fd (const int file_desc)
    {
        sig     = DBUS_TYPE_UNIX_FD_AS_STRING;
        val.fd  = file_desc;
        str_val = "";
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int dbus_basic::fd () const
    {
        return val.fd;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::operator= (const char* value)
    {
        if (sig != DBUS_TYPE_STRING_AS_STRING      &&
            sig != DBUS_TYPE_OBJECT_PATH_AS_STRING &&
            sig != DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            sig = DBUS_TYPE_BYTE_AS_STRING;
        }
        str_val = std::string (value);
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::operator= (const std::string& value)
    {
        if (sig != DBUS_TYPE_STRING_AS_STRING      &&
            sig != DBUS_TYPE_OBJECT_PATH_AS_STRING &&
            sig != DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            sig = DBUS_TYPE_BYTE_AS_STRING;
        }
        str_val = value;
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::operator= (std::string&& value)
    {
        if (sig != DBUS_TYPE_STRING_AS_STRING      &&
            sig != DBUS_TYPE_OBJECT_PATH_AS_STRING &&
            sig != DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            sig = DBUS_TYPE_BYTE_AS_STRING;
        }
        str_val = std::forward<std::string> (value);
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::set_opath (const std::string& value)
    {
        sig     = DBUS_TYPE_OBJECT_PATH_AS_STRING;
        str_val = value;
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_basic& dbus_basic::set_sig (const std::string& value)
    {
        sig     = DBUS_TYPE_SIGNATURE_AS_STRING;
        str_val = value;
        val.str = const_cast<char*> (str_val.c_str());
        return *this;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_basic::copy (const dbus_type& obj)
    {
        DBUS_BASIC_TRACE ("dbus_basic::copy(const dbus_type& obj) - obj: %s",
                          obj.str().c_str());

        if (!obj.is_basic()) {
            std::stringstream ss;
            ss << "Can't assign a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_basic.";
            throw std::invalid_argument (ss.str());
        }
        const auto& b = dynamic_cast<const dbus_basic&> (obj);
        sig = b.sig;
        val = b.val;
        if (sig==DBUS_TYPE_STRING_AS_STRING ||
            sig==DBUS_TYPE_OBJECT_PATH_AS_STRING ||
            sig==DBUS_TYPE_SIGNATURE_AS_STRING)
        {
            str_val = b.str_val;
            val.str = const_cast<char*> (str_val.c_str());
        }else{
            str_val = "";
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void dbus_basic::move (dbus_type&& obj)
    {
        DBUS_BASIC_TRACE ("dbus_basic::move(dbus_type&& obj) - obj: %s",
                          obj.str().c_str());

        if (!obj.is_basic()) {
            std::stringstream ss;
            ss << "Can't move a dbus_type with signature '"
               << obj.signature()
               << "' to a dbus_basic.";
            throw std::invalid_argument (ss.str());
        }
        auto&& b = dynamic_cast<dbus_basic&&> (obj);
        sig      = std::move (b.sig);
        str_val  = std::move (b.str_val);
        val      = b.val;

        b.sig     = DBUS_TYPE_INT32_AS_STRING;
        b.val.u64 = 0LL;
        b.str_val = "";
    }


}
