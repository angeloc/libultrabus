/*
 * Copyright (C) 2021,2023 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/CallbackMessageHandler.hpp>

//#define TRACE_DEBUG

#ifdef TRACE_DEBUG
#  include <cstdio>
#  define TRACE(format, ...) fprintf(stderr, "(%u) %s:%s:%d: " format "\n", (unsigned)gettid(), __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#  define TRACE(format, ...)
#endif


namespace ultrabus {



    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    CallbackMessageHandler::CallbackMessageHandler (Connection& connection,
                                                    msg_cb_t method_call_cb,
                                                    msg_cb_t signal_cb)
        : MessageHandler (connection),
          on_method_call_cb (method_call_cb),
          on_signal_cb (signal_cb),
          on_message_cb (nullptr)
    {
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void CallbackMessageHandler::set_method_call_cb (msg_cb_t callback)
    {
        on_method_call_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void CallbackMessageHandler::set_signal_cb (msg_cb_t callback)
    {
        on_signal_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    void CallbackMessageHandler::set_message_cb (msg_cb_t callback)
    {
        on_message_cb = callback;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool CallbackMessageHandler::on_method_call (Message& msg)
    {
        if (on_method_call_cb)
            return on_method_call_cb (msg);
        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool CallbackMessageHandler::on_signal (Message& msg)
    {
        if (on_signal_cb)
            return on_signal_cb (msg);
        return false;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    bool CallbackMessageHandler::on_message (Message& msg)
    {
        if (on_message_cb)
            return on_message_cb (msg);
        else
            return dispatch_msg (msg);
    }


}
