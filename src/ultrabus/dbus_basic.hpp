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
#ifndef ULTRABUS_DBUS_BASIC_HPP
#define ULTRABUS_DBUS_BASIC_HPP

#include <sys/types.h>
#include <ultrabus/dbus_type.hpp>
#include <string>
#include <dbus/dbus.h>

namespace ultrabus {

    /**
     * Wrapper for a DBus basic type.
     * @see <a href=https://dbus.freedesktop.org/doc/dbus-specification.html#basic-types rel="noopener noreferrer" target="_blank">DBus Basic Types at dbus.freedesktop.org</a>
     */
    class dbus_basic : public dbus_type {
    public:
        dbus_basic (); /**< Default constructor. Default is a signed 32 bit integer with value 0. */
        virtual ~dbus_basic () = default; /**< Default destructor. */

        dbus_basic (const dbus_basic& obj); /**< Copy constructor. @param obj The object copy. */
        dbus_basic (dbus_basic&& obj);      /**< Move constructor. @param obj The object to move. */

        dbus_basic (const dbus_type& obj); /**< Copy constructor. @param obj The object to copy. */
        dbus_basic (dbus_type&& obj);      /**< Move constructor. @param obj The object to move. */

        /**
         * Constructor.
         * Construct a DBus basic type with a specific signature and value.
         * @param signature The DBus type signature.
         * @param value The value of the DBus type.
         */
        dbus_basic (const std::string& signature, const DBusBasicValue& value);

        dbus_basic (const uint8_t value);    /**< Contruct a DBus BYTE type. */
        dbus_basic (const int16_t value);    /**< Contruct a DBus INT16 type. */
        dbus_basic (const uint16_t value);   /**< Contruct a DBus UINT16 type. */
        dbus_basic (const int32_t value);    /**< Contruct a DBus INT32 type. */
        dbus_basic (const uint32_t value);   /**< Contruct a DBus UINT32 type. */
        dbus_basic (const bool value);       /**< Contruct a DBus BOOLEAN type. */
        dbus_basic (const int64_t value);    /**< Contruct a DBus INT64 type. */
        dbus_basic (const uint64_t value);   /**< Contruct a DBus UINT64 type. */
        dbus_basic (const double value);     /**< Contruct a DBus DOUBLE type. */
        /**
           Construct a DBus string, object path or signature type.
            @param value The string value.
            @param str_type The type of DBus string.
                            One of DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE.
                            Default is DBUS_TYPE_STRING.
         */
        dbus_basic (const std::string& value, int str_type=DBUS_TYPE_STRING);
        /**
           Construct a DBus string, object path or signature type.
            @param value The string value.
            @param str_type The type of DBus string.
                            One of DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE.
                            Default is DBUS_TYPE_STRING.
         */
        dbus_basic (const char* value, int str_type=DBUS_TYPE_STRING);

        dbus_basic& operator= (const dbus_basic& t); /**< Assignment operator. */
        dbus_basic& operator= (dbus_basic&& t); /**< Move operator. */

        bool operator== (const dbus_basic& b); /**< Comparison operator. */
        virtual bool is_basic () const; /**< Return true since this is a basic DBus type. */
        virtual int type_code () const; /**< Return the DBus type code. */

        uint8_t byt () const;           /**< Return the basic value as a byte. */
        dbus_basic& byt (uint8_t val);  /**< Assign a BYTE value to the basic type. */

        int16_t i16 () const;           /**< Return the basic value as a signed 16 bit integer. */
        dbus_basic& i16 (int16_t val);  /**< Assign a INT16 value to the basic type. */

        uint16_t u16 () const;          /**< Return the basic value as an unsigned 16 bit integer. */
        dbus_basic& u16 (uint16_t val); /**< Assign a UINT16 value to the basic type. */

        int32_t i32 () const;           /**< Return the basic value as a signed 32 bit integer. */
        dbus_basic& i32 (int32_t val);  /**< Assign a INT32 value to the basic type. */

        uint32_t u32 () const;          /**< Return the basic value as an unsigned 32 bit integer. */
        dbus_basic& u32 (uint32_t val); /**< Assign a UINT32 value to the basic type. */

        bool boolean () const;          /**< Return the basic value as a boolean. */
        dbus_basic& boolean (bool val); /**< Assign a BOOLEAN value to the basic type. */

        int64_t i64 () const;           /**< Return the basic value as a signed 64 bit integer. */
        dbus_basic& i64 (int64_t val);  /**< Assign a INT64 value to the basic type. */

        uint64_t u64 () const;          /**< Return the basic value as an unsigned 64 bit integer. */
        dbus_basic& u64 (uint64_t val); /**< Assign a UINT64 value to the basic type. */

        double dbl () const;            /**< Return the basic value as a double. */
        dbus_basic& dbl (double val);   /**< Assign a DOUBLE value to the basic type. */

        int fd () const;                      /**< Return the basic value as a file descriptor. */
        dbus_basic& fd (const int file_desc); /**< Assign a UNIX_FD value to the basic type. */

        virtual const std::string str () const; /**< Return the basic value as a string. */
        /**
         * Set a string, object path or signature value. Default is a string value.
         * @param val The string value.
         * @param str_type The type of DBus string.
         *                 One of DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE.
         *                 Default is DBUS_TYPE_STRING.
         */
        dbus_basic& str (const std::string& val, int str_type=DBUS_TYPE_STRING);
        /**
         * Set a string, object path or signature value. Default is a string value.
         * @param val The string value.
         * @param str_type The type of DBus string.
         *                 One of DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE.
         *                 Default is DBUS_TYPE_STRING.
         */
        dbus_basic& str (std::string&& val, int str_type=DBUS_TYPE_STRING);

        dbus_basic& operator= (const uint8_t value) {return byt(value);}  /**< Assign a BYTE value to the basic type. */
        dbus_basic& operator= (const int16_t value) {return i16(value);}  /**< Assign a INT16 value to the basic type. */
        dbus_basic& operator= (const uint16_t value) {return u16(value);} /**< Assign a UINT16 value to the basic type. */
        dbus_basic& operator= (const int32_t value) {return i32(value);}  /**< Assign a INT32 value to the basic type. */
        dbus_basic& operator= (const uint32_t value) {return u32(value);} /**< Assign a UINT32 value to the basic type. */
        dbus_basic& operator= (const bool value) {return boolean(value);} /**< Assign a BOOLEAN value to the basic type. */
        dbus_basic& operator= (const int64_t value) {return i64(value);}  /**< Assign a INT64 value to the basic type. */
        dbus_basic& operator= (const uint64_t value) {return u64(value);} /**< Assign a UINT64 value to the basic type. */
        dbus_basic& operator= (const double value) {return dbl(value);}   /**< Assign a DOUBLE value to the basic type. */
        /**
         * Assign a string to the DBus type. If the basic DBus type is any other type than
         * DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE, the basic type
         * will be chaned to DBUS_TYPE_STRING.
         * @param value The string value.
         */
        dbus_basic& operator= (const std::string& value);
        /**
         * Move a string object to this DBus basic object. If the basic DBus type is any other type than
         * DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE, the basic type
         * will be chaned to DBUS_TYPE_STRING.
         * @param value The string value.
         */
        dbus_basic& operator= (std::string&& value);
        /**
         * Assign a string to the DBus type. If the basic DBus type is any other type than
         * DBUS_TYPE_STRING, DBUS_TYPE_OBJECT_PATH, or DBUS_TYPE_SIGNATURE, the DBus type
         * will be chaned to DBUS_TYPE_STRING.
         * @param value The string value.
         */
        dbus_basic& operator= (const char* value);
        dbus_basic& set_opath (const std::string& value); /**< Assign an DBUS_TYPE_OBJECT_PATH value to the basic type. */
        dbus_basic& set_sig   (const std::string& value); /**< Assign a SIGNATURE value to the basic type. */

        const DBusBasicValue& get_val () const { return val; } /**< Return the value to the wrapped DBus objec. */

    protected:
        virtual void copy (const dbus_type& obj);
        virtual void move (dbus_type&& obj);

    private:
        friend bool operator< (const dbus_basic& lval, const dbus_basic& rval);
        DBusBasicValue val;
        std::string str_val;
    };


    /**
     * An alias of a shared pointer to a dbus_basic class.
     * @see dbus_basic
     */
    using dbus_basic_ptr = std::shared_ptr<dbus_basic>;

    /**
     * Cast an shared dbus_type pointer to a dbus_basic pointer.
     * @return A shared pointer to an dbus_basic object or a nullptr if unsuccessful.
     */
    static inline dbus_basic_ptr dbus_type_to_dbus_basic (const dbus_type_ptr& ptr) {
        //return ptr!=nullptr ? std::dynamic_pointer_cast<dbus_basic>(ptr) : nullptr;
        return std::dynamic_pointer_cast<dbus_basic> (ptr);
    }

    /**
     * Less than operator.
     * Compare two dbus_basic objects and return true is lval is less than rval.
     * The basic DBus type used when comapring is determined by lval.
     * @return true if lval is less than rval.
     */
    bool operator< (const dbus_basic& lval, const dbus_basic& rval);



}

#endif
