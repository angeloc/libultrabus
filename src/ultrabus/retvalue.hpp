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
#ifndef ULTRABUS_RETVALUE_HPP
#define ULTRABUS_RETVALUE_HPP


#include <string>


namespace ultrabus {


    /**
     * A return value that also includes a message transmission error code
     * and optional error description.
     * When sending a message on the message bus a lot of things can go wrong,
     * the message can be malformed, the object the message is sent to might
     * no longer exist, the interface may be wrong, etc.<br/>
     * To distinguish between the result of the message transmission and the
     * result of the method call, a <code>retvalue</code> object is returned
     * when sending messages on the message bus. A <code>retvalue</code>
     * object contains the return value of the method call and also an error
     * code/description for the actual message transmission.<br/>
     * By convention, error code 0 is considered to be a success.
     */
    template <typename T>
    class retvalue {
    public:

        /**
         * Default constructor.
         * Create a return value with error code 0(success) an no error description.
         * The value of the return value is not defined.
         */
        retvalue () : err_num {0}
        {
        }

        /**
         * Constructor.
         * Create a retvalue with a given return value.
         * The error code is set to 0(success) an the error description is an empty string.
         */
        retvalue (const T& val) : err_num {0}
        {
            value = val;
        }

        /**
         * Constructor.
         * Create a retvalue with a given return value.
         * The error code is set to 0(success) an the error description is an empty string.
         */
        retvalue (T&& val) : err_num {0}
        {
            value = std::forward<T> (val);
        }

        /**
         * Constructor.
         * Construct a retvalue with a given error and error description.
         * The value of the return value is not defined.
         */
        retvalue (int err, const std::string& err_desc)
            : err_num{err}, err_str{err_desc}
        {
        }

        /**
         * Copy constructor.
         */
        retvalue (const retvalue<T>& r)
        {
            value   = r.value;
            err_num = r.err_num;
            err_str = r.err_str;
        }

        /**
         * Move constructor.
         */
        retvalue (retvalue<T>&& r)
        {
            value   = std::move (r.value);
            err_num = r.err_num;
            err_str = std::move (r.err_str);
        }

        /**
         * Assignment operator.
         */
        retvalue<T>& operator= (const retvalue<T>& rhs) {
            if (&rhs != this) {
                value   = rhs.value;
                err_num = rhs.err_num;
                err_str = rhs.err_str;
            }
            return *this;
        }

        /**
         * Move operator.
         */
        retvalue<T>& operator= (retvalue<T>&& rhs) {
            if (&rhs != this) {
                value   = std::move (rhs.value);
                err_num = rhs.err_num;
                err_str = std::move (rhs.err_str);
            }
            return *this;
        }

        /**
         * Assignment operator.
         * Assign a return value.
         */
        T& operator= (const T& rhs) {
            value = rhs;
            return value;
        }

        /**
         * Move operator.
         * Assign a return value.
         */
        T& operator= (T&& rhs) {
            value = std::move (rhs);
            return value;
        }

        /**
         * Implicit conversion operator.
         * Convert this object to the return value.
         */
        operator T () const {
            return get ();
        }

        /**
         * Implicit conversion operator.
         * Convert this object to a reference to the return value.
         */
        operator T& () {
            return get ();
        }

        /**
         * Return a reference to the actual return value.
         */
        T& get () {
            return value;
        }

        /**
         * Set the return value.
         */
        retvalue<T>& set (const T& return_value) {
            value = return_value;
            return *this;
        }

        /**
         * Set the return value.
         */
        retvalue<T>& set (T&& return_value) {
            value = std::move (return_value);
            return *this;
        }

        /**
         * Return the error code.
         */
        int err () const {
            return err_num;
        }

        /**
         * Set the error code.
         * The error description will not be modified by this call.
         */
        retvalue<T>& err (int e) {
            err_num = e;
            return *this;
        }

        /**
         * Set the error code and an error description.
         */
        retvalue<T>& err (int e, const std::string& description) {
            err_num  = e;
            err_str = description;
            return *this;
        }

        /**
         * Set the error description string.
         * The error code is not modified by this call.
         */
        retvalue<T>& err (const std::string& description) {
            err_str = description;
            return *this;
        }

        /**
         * Return an error string describing the error.
         * The error description can be an empty string.
         */
        const std::string& what () const {
            return err_str;
        }


    private:
        T value;
        int err_num;
        std::string err_str;
    };


}

#endif
