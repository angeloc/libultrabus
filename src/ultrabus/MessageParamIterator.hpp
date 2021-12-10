/*
 * Copyright (C) 2015-2017,2021 Dan Arrhenius <dan@ultramarin.se>
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
#ifndef ULTRABUS_MESSAGEPARAMITERATOR_HPP
#define ULTRABUS_MESSAGEPARAMITERATOR_HPP

#include <ultrabus/Message.hpp>
#include <string>
#include <memory>
#include <dbus/dbus.h>


namespace ultrabus {


    /**
     * DBus message parameter iterator.
     */
    class MessageParamIterator {
    public:
        /**
         * Default constructor.
         */
        MessageParamIterator () = default;

        /**
         * Copy constructor.
         */
        MessageParamIterator (const MessageParamIterator& iter) = default;

        /**
         * Move constructor.
         */
        MessageParamIterator (MessageParamIterator&& iter) = default;

        /**
         * Constructor.
         */
        explicit MessageParamIterator (const Message& message);

        /**
         * Destructor.
         */
        ~MessageParamIterator () = default;

        /**
         * Assignment operator.
         */
        MessageParamIterator& operator= (const MessageParamIterator& iter) = default;

        /**
         * Move operator.
         */
        MessageParamIterator& operator= (MessageParamIterator&& iter) = default;

        /**
         * Return false if no more arguments.
         */
        operator bool () const;

        /**
         * Operator ++a.
         */
        MessageParamIterator& operator++ ();

        /**
         * Return the argument type of the current argument.
         * Or DBUS_TYPE_INVALID of there are no arguments left.
         */
        int arg_type () const;

        /**
         * Return the element type of the array that the message iterator points to.
         * Note that you need to check that the iterator points to an array prior to using this method.
         */
        int element_type () const;

        /**
         * Create a recursive iterator for a value.
         * Note that this recurses into a value, not a type, so you can only recurse if the value exists.
         */
        MessageParamIterator iterator ();

        /**
         * Return the signature.
         */
        std::string signature ();

        /**
         * Read a basic-typed value.
         */
        void basic_value (void* value);


    private:
        std::shared_ptr<DBusMessageIter> msg_iter;
    };


}

#endif
