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
#include <ultrabus/MessageParamIterator.hpp>


namespace ultrabus {


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MessageParamIterator::MessageParamIterator (const Message& message)
    {
        auto* msg_handle = const_cast<Message&>(message).handle ();
        if (msg_handle != nullptr) {
            msg_iter = std::make_shared<DBusMessageIter> ();
            if (!dbus_message_iter_init(msg_handle, msg_iter.get()))
                msg_iter.reset (); // Reset shared pointer
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MessageParamIterator::operator bool () const
    {
        return arg_type() != DBUS_TYPE_INVALID;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MessageParamIterator& MessageParamIterator::operator++ ()
    {
        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr)
            dbus_message_iter_next (msg_iter.get());

        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int MessageParamIterator::arg_type () const
    {
        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr)
            return dbus_message_iter_get_arg_type (msg_iter.get());
        else
            return DBUS_TYPE_INVALID;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int MessageParamIterator::element_type () const
    {
        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr)
            return dbus_message_iter_get_element_type (msg_iter.get());
        else
            return DBUS_TYPE_INVALID;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MessageParamIterator MessageParamIterator::iterator ()
    {
        MessageParamIterator recursive_iter;
        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr) {
            recursive_iter.msg_iter = std::make_shared<DBusMessageIter> ();
            dbus_message_iter_recurse (msg_iter.get(), recursive_iter.msg_iter.get());
        }
        return recursive_iter;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string MessageParamIterator::signature ()
    {
        std::string s {""};

        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr) {
            char* tmp_signature = dbus_message_iter_get_signature (msg_iter.get());
            if (tmp_signature) {
                s = std::string (tmp_signature);
                dbus_free (tmp_signature);
            }
        }
        return s;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void MessageParamIterator::basic_value (void* value)
    {
        if (msg_iter.use_count()!=0 && msg_iter.get()!=nullptr && value!=nullptr)
            dbus_message_iter_get_basic (msg_iter.get(), value);
    }


}
