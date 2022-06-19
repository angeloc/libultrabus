/*
 * Copyright (C) 2015-2017,2021,2022 Dan Arrhenius <dan@ultramarin.se>
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
#include <ultrabus/Message.hpp>
#include <ultrabus/MessageParamIterator.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_struct.hpp>
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/dbus_array.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace ultrabus {


    static std::string type_to_string (int type);
    static std::string value_to_string (const int type, const dbus_basic& value);


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static bool msg_validate_dest (const std::string& path,
                                   const std::string& iface,
                                   const std::string& method,
                                   DBusError& err)
    {
        dbus_error_init (&err);
        if (!dbus_validate_path(path.c_str(), &err))
            return false;
        if (!dbus_validate_interface(iface.c_str(), &err))
            return false;
        if (!dbus_validate_member(method.c_str(), &err))
            return false;
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static bool msg_validate_msg_dest (const std::string& destination,
                                       const std::string& path,
                                       const std::string& iface,
                                       const std::string& method,
                                       DBusError& err)
    {
        dbus_error_init (&err);
        if (!dbus_validate_bus_name(destination.c_str(), &err))
            return false;
        if (!dbus_validate_path(path.c_str(), &err))
            return false;
        if (!iface.empty() && !dbus_validate_interface(iface.c_str(), &err))
            return false;
        if (!dbus_validate_member(method.c_str(), &err))
            return false;
        return true;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message::Message ()
    {
        msg_handle = dbus_message_new (DBUS_MESSAGE_TYPE_METHOD_CALL);
        if (msg_handle == nullptr)
            throw std::bad_alloc ();
    }


    //-----------------------------------------------------------------------
    // Create a method call message
    //-----------------------------------------------------------------------
    Message::Message (const std::string& destination,
                      const std::string& path,
                      const std::string& iface,
                      const std::string& method)
    {
        DBusError err;
        if (!msg_validate_msg_dest(destination, path, iface, method, err)) {
            std::string msg =
                std::string(err.name) +
                std::string(": ") +
                std::string(err.message);
            dbus_error_free (&err);
            throw std::invalid_argument (msg);
        }
        msg_handle = dbus_message_new_method_call (destination.c_str(),
                                                   path.c_str(),
                                                   iface.empty() ? nullptr : iface.c_str(),
                                                   method.c_str());
        if (msg_handle == nullptr)
            throw std::bad_alloc ();
    }


    //-----------------------------------------------------------------------
    // Create a Message object from a DBusMessage pointer.
    //-----------------------------------------------------------------------
    Message::Message (DBusMessage* message)
    {
        msg_handle = message;
        if (msg_handle != nullptr)
            dbus_message_ref (msg_handle);
    }


    //-----------------------------------------------------------------------
    // Create a signal message
    //-----------------------------------------------------------------------
    Message::Message (const std::string& path,
                      const std::string& iface,
                      const std::string& name)
    {
        DBusError err;
        if (!msg_validate_dest(path, iface, name, err)) {
            std::string msg =
                std::string(err.name) +
                std::string(": ") +
                std::string(err.message);
            dbus_error_free (&err);
            throw std::invalid_argument (msg);
        }

        msg_handle = dbus_message_new_signal (path.c_str(), iface.c_str(), name.c_str());
        if (msg_handle == nullptr)
            throw std::bad_alloc ();
    }


    //-----------------------------------------------------------------------
    // Create an error message
    //-----------------------------------------------------------------------
    Message::Message (DBusMessage* message,
                      const bool is_error,
                      const std::string& error_name,
                      const std::string& error_message)
    {
        if (is_error) {
            DBusError err;
            dbus_error_init (&err);
            if (!dbus_validate_error_name(error_name.c_str(), &err)) {
                std::string msg =
                    std::string(err.name) +
                    std::string(": ") +
                    std::string(err.message);
                dbus_error_free (&err);
                throw std::invalid_argument (msg);
            }
            msg_handle = dbus_message_new_error (message, error_name.c_str(), error_message.c_str());
        }else{
            msg_handle = dbus_message_new_method_return (message);
        }
        if (msg_handle == nullptr)
            throw std::bad_alloc ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message::Message (const Message& message)
    {
        if (message.msg_handle) {
            msg_handle = dbus_message_copy (message.msg_handle);
            if (msg_handle == nullptr)
                throw std::bad_alloc ();
        }else{
            msg_handle = nullptr;
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message::Message (Message&& message)
    {
        msg_handle = message.msg_handle;
        message.msg_handle = nullptr;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message::~Message ()
    {
        if (msg_handle) {
            dbus_message_unref (msg_handle);
            msg_handle = nullptr;
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message& Message::operator= (const Message& message)
    {
        if (&message != this) {
            DBusMessage* old_msg_handle = msg_handle;
            if (message.msg_handle) {
                msg_handle = dbus_message_copy (message.msg_handle);
                if (msg_handle == nullptr) {
                    msg_handle = old_msg_handle;
                    throw std::bad_alloc ();
                }
            }else{
                msg_handle = nullptr;
            }
            if (old_msg_handle)
                dbus_message_unref (old_msg_handle);
        }
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message& Message::operator= (Message&& message)
    {
        if (&message != this) {
            msg_handle = message.msg_handle;
            message.msg_handle = nullptr;
        }
        return *this;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    DBusMessage* Message::handle ()
    {
        return msg_handle;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static void append_dbus_type_base_impl (DBusMessageIter& iter, const dbus_type_base& arg)
    {
        if (arg.is_basic()) {
            const dbus_basic& basic {dynamic_cast<const dbus_basic&>(arg)};
            auto val = basic.get_val ();
            dbus_message_iter_append_basic (&iter, basic.type_code(), &val);
        }
        else if (arg.is_struct()) {
            const dbus_struct& arg_struct {dynamic_cast<const dbus_struct&>(arg)};
            if (arg_struct.size() > 0) {
                DBusMessageIter sub_iter;
                dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, nullptr, &sub_iter);
                for (std::size_t i=0; i<arg_struct.size(); ++i) {
                    auto& sub_arg = const_cast<dbus_struct&>(arg_struct)[i];
                    append_dbus_type_base_impl (sub_iter, sub_arg);
                }
                dbus_message_iter_close_container (&iter, &sub_iter);
            }
        }
        else if (arg.is_array() || arg.is_properties()) {
            dbus_array const* arg_array = nullptr;
            if (arg.is_properties()) {
                const dbus_array& da = dynamic_cast<Properties&>(const_cast<dbus_type_base&>(arg)).data ();
                arg_array = dynamic_cast<dbus_array const*> (&da);
            }else{
                arg_array = dynamic_cast<dbus_array const*> (&arg);
            }
            if (!arg_array->element_signature().empty()) {
                DBusMessageIter sub_iter;
                dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                                                  arg_array->element_signature().c_str(), &sub_iter);
                for (std::size_t i=0; i<arg_array->size(); ++i) {
                    auto& sub_arg = const_cast<dbus_array&>(*arg_array)[i];
                    append_dbus_type_base_impl (sub_iter, sub_arg);
                }
                dbus_message_iter_close_container (&iter, &sub_iter);
            }
        }
        else if (arg.is_dict_entry()) {
            const dbus_dict_entry& arg_dict_entry {dynamic_cast<const dbus_dict_entry&>(arg)};
            auto& key   = const_cast<dbus_dict_entry&>(arg_dict_entry).key ();
            auto& value = const_cast<dbus_dict_entry&>(arg_dict_entry).value ();
            DBusMessageIter sub_iter;
            dbus_message_iter_open_container (&iter, DBUS_TYPE_DICT_ENTRY, nullptr, &sub_iter);
            append_dbus_type_base_impl (sub_iter, key);
            append_dbus_type_base_impl (sub_iter, value);
            dbus_message_iter_close_container (&iter, &sub_iter);
        }
        else if (arg.is_variant()) {
            const dbus_variant& arg_variant {dynamic_cast<const dbus_variant&>(arg)};

            auto& sub_arg = const_cast<dbus_variant&>(arg_variant).value ();
            DBusMessageIter sub_iter;
            dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT,
                                              sub_arg.signature().c_str(), &sub_iter);
            append_dbus_type_base_impl (sub_iter, sub_arg);
            dbus_message_iter_close_container (&iter, &sub_iter);
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::append_dbus_type_base (const dbus_type_base& arg)
    {
        DBusMessageIter iter;
        dbus_message_iter_init_append (msg_handle, &iter);
        append_dbus_type_base_impl (iter, arg);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static dbus_type_ptr arguments_get_arg_impl (ultrabus::MessageParamIterator& iter)
    {
        DBusBasicValue basic_value;
        dbus_basic*    arg_basic;
        dbus_struct*   arg_struct;
        dbus_variant*  arg_variant;

        switch (iter.arg_type()) {
        case DBUS_TYPE_BYTE:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.byt));

        case DBUS_TYPE_BOOLEAN:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(static_cast<bool>(basic_value.bool_val)));
            break;

        case DBUS_TYPE_INT16:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.i16));
            break;

        case DBUS_TYPE_UINT16:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.u16));
            break;

        case DBUS_TYPE_INT32:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.i32));
            break;

        case DBUS_TYPE_UINT32:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.u32));
            break;

        case DBUS_TYPE_INT64:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(static_cast<const int64_t>(basic_value.i64)));
            break;

        case DBUS_TYPE_UINT64:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(static_cast<const uint64_t>(basic_value.u64)));
            break;

        case DBUS_TYPE_DOUBLE:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.dbl));
            break;

        case DBUS_TYPE_STRING:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.str));
            break;

        case DBUS_TYPE_OBJECT_PATH:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.str, DBUS_TYPE_OBJECT_PATH));
            break;

        case DBUS_TYPE_SIGNATURE:
            iter.basic_value (&basic_value);
            return dbus_type_ptr (new dbus_basic(basic_value.str, DBUS_TYPE_SIGNATURE));
            break;

        case DBUS_TYPE_UNIX_FD:
            iter.basic_value (&basic_value);
            arg_basic = new dbus_basic;
            arg_basic->fd (basic_value.fd);
            return dbus_type_ptr (arg_basic);
            break;

        case DBUS_TYPE_STRUCT:
            arg_struct = new dbus_struct;
            for (auto sub_iter = iter.iterator(); sub_iter==true; ++sub_iter)
                arg_struct->add (*arguments_get_arg_impl(sub_iter));
            return dbus_type_ptr (arg_struct);

        case DBUS_TYPE_ARRAY:
        {
            auto sub_iter = iter.iterator ();
            dbus_array* arg_array = new dbus_array (sub_iter.signature());
            for (; sub_iter==true; ++sub_iter)
                arg_array->add (*arguments_get_arg_impl(sub_iter));
            return dbus_type_ptr (arg_array);
        }

        case DBUS_TYPE_DICT_ENTRY:
            {
                dbus_dict_entry* arg_dict_entry = new dbus_dict_entry;
                auto sub_iter = iter.iterator ();
                if (sub_iter == true) {
                    arg_dict_entry->key (*arguments_get_arg_impl(sub_iter));
                    ++sub_iter;
                    if (sub_iter == true)
                        arg_dict_entry->value (*arguments_get_arg_impl(sub_iter));
                }
                return dbus_type_ptr (arg_dict_entry);
            }
            break;

        case DBUS_TYPE_VARIANT:
            arg_variant = new dbus_variant;
            for (auto sub_iter = iter.iterator(); sub_iter==true; ++sub_iter)
                arg_variant->value (*arguments_get_arg_impl(sub_iter));
            return dbus_type_ptr (arg_variant);
        }

        return nullptr;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::vector<dbus_type_ptr> Message::arguments ()
    {
        std::vector<dbus_type_ptr> args;

        ultrabus::MessageParamIterator arg_iter (*this);

        for (; arg_iter==true; ++arg_iter) {
            auto arg_ptr = arguments_get_arg_impl (arg_iter);
            if (arg_ptr != nullptr) {
                args.push_back (arg_ptr);
            }
        }

        return args;//std::move (args);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool Message::get_args (dbus_type* arg, ...)
    {
        auto params = arguments ();
        std::size_t i {0};
        bool retval {true};
        va_list ap;
        va_start (ap, arg);

        for (auto& param : params) {
            if (arg == nullptr)
                break;

            if (param == nullptr) {
                return false;
            }

            if (typeid(*arg) == typeid(Properties)) {
                Properties* props = dynamic_cast<Properties*> (arg);
                if (param->signature() != "a{sv}") {
                    retval = false;
                    break;
                }
                props->reset (*param);
            }else{
                if ( !(arg->is_basic()  && param->is_basic())  &&
                      (arg->type_code() != param->type_code()))
                {
                    retval = false;
                    break;
                }
                *arg = *param;
            }

            // Get next argument
            arg = va_arg (ap, dbus_type*);
            ++i;
        }
        va_end (ap);
        return retval;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Message::type () const
    {
        if (msg_handle == nullptr)
            return DBUS_MESSAGE_TYPE_INVALID;
        return dbus_message_get_type (msg_handle);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::signature () const
    {
        if (msg_handle == nullptr)
            return std::string {""};
        auto signature = dbus_message_get_signature (msg_handle);
        return std::string (signature!=nullptr ? signature : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::destination () const
    {
        if (msg_handle == nullptr)
            return std::string {""};
        auto dest = dbus_message_get_destination (msg_handle);
        return std::string (dest!=nullptr ? dest : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::destination (const std::string& bus_name)
    {
        if (msg_handle) {
            if (bus_name.empty())
                dbus_message_set_destination (msg_handle, nullptr);
            else
                dbus_message_set_destination (msg_handle, bus_name.c_str());
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::path () const
    {
        if (!msg_handle)
            return std::string {""};
        auto opath = dbus_message_get_path (msg_handle);
        return std::string (opath!=nullptr ? opath : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::path (const std::string& object_path)
    {
        if (!msg_handle)
            return;
        if (object_path.empty())
            dbus_message_set_path (msg_handle, nullptr);
        else
            dbus_message_set_path (msg_handle, object_path.c_str());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::interface () const
    {
        if (!msg_handle)
            return std::string {""};
        auto iface = dbus_message_get_interface (msg_handle);
        return std::string (iface!=nullptr ? iface : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::interface (const std::string& iface)
    {
        if (!msg_handle)
            return;
        if (iface.empty())
            dbus_message_set_interface (msg_handle, nullptr);
        else
            dbus_message_set_interface (msg_handle, iface.c_str());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::name () const
    {
        if (!msg_handle)
            return std::string {""};
        auto method = dbus_message_get_member (msg_handle);
        return std::string (method!=nullptr ? method : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::name (const std::string& msg_name)
    {
        if (!msg_handle)
            return;
        if (msg_name.empty())
            dbus_message_set_member (msg_handle, nullptr);
        else
            dbus_message_set_member (msg_handle, msg_name.c_str());
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::error_name () const
    {
        if (!msg_handle)
            return std::string {""};
        auto error = dbus_message_get_error_name (msg_handle);
        return std::string (error!=nullptr ? error : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    int Message::error_name (const std::string& error)
    {
        if (!error.empty() && !dbus_validate_error_name(error.c_str(), nullptr))
            return -1;

        if (msg_handle) {
            if (error.empty())
                dbus_message_set_error_name (msg_handle, nullptr);
            else
                dbus_message_set_error_name (msg_handle, error.c_str());
        }
        return 0;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::error_msg () const
    {
        std::string error {""};
        if (msg_handle != nullptr) {
            ultrabus::MessageParamIterator param (*this);
            if (param.signature() == "s") {
                DBusBasicValue basic_value;
                param.basic_value (&basic_value);
                error = std::string (basic_value.str==nullptr ? "" : basic_value.str);
            }
        }
        return error;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::sender () const
    {
        if (msg_handle == nullptr)
            return std::string {""};
        auto sender = dbus_message_get_sender (msg_handle);
        return std::string (sender!=nullptr ? sender : "");
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint32_t Message::serial () const
    {
        if (msg_handle == nullptr)
            return 0;
        return dbus_message_get_serial (msg_handle);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    uint32_t Message::reply_serial () const
    {
        if (msg_handle == nullptr)
            return 0;
        return dbus_message_get_reply_serial (msg_handle);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::get_string_ret () const
    {
        DBusBasicValue value;
        ultrabus::MessageParamIterator iter (*this);
        if (iter==false)
            return std::string {""};

        switch (iter.arg_type()) {
        case DBUS_TYPE_STRING:
        case DBUS_TYPE_OBJECT_PATH:
        case DBUS_TYPE_SIGNATURE:
            iter.basic_value (&value);
            return std::string (value.str==nullptr ? "" : value.str);
        default:
            return std::string {""};
        }
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //static std::string describe_param (std::stringstream& ss, const std::string& prefix, dbus_type_ptr param)
    static std::string describe_param (std::stringstream& ss, const std::string& prefix, dbus_type& p)
    {
        ss << prefix << "Signature: " << p.signature();

        if (p.is_basic()) {
            ss << " \tType: " << type_to_string(static_cast<int>(p.signature()[0])) << " \tValue:";
            ss << " " << value_to_string(static_cast<int>(p.signature()[0]), dynamic_cast<dbus_basic&>(p));
            ss << std::endl;
        }
        else if (p.is_array()) {
            ss << " \tType: DBUS_TYPE_ARRAY:";
            dbus_array& pa {dynamic_cast<dbus_array&>(p)};
            ss << std::endl;
            for(size_t i=0; i<pa.size(); ++i)
                describe_param(ss, prefix+std::string("    "), pa[i]);
        }
        else if (p.is_struct()) {
            ss << " \tType: DBUS_TYPE_STRUCT:";
            dbus_struct& ps {dynamic_cast<dbus_struct&>(p)};
            ss << std::endl;
            for(size_t i=0; i<ps.size(); ++i)
                describe_param(ss, prefix+std::string("    "), ps[i]);
        }
        else if (p.is_dict_entry()) {
            ss << " \tType: DBUS_TYPE_DICT_ENTRY:";
            dbus_dict_entry& pd {dynamic_cast<dbus_dict_entry&>(p)};
            ss << std::endl;
            describe_param(ss, prefix+std::string("    "), pd.key());
            describe_param(ss, prefix+std::string("    "), pd.value());
        }
        else if (p.is_variant()) {
            ss << " \tType: DBUS_TYPE_VARIANT:";
            dbus_variant& pv {dynamic_cast<dbus_variant&>(p)};
            ss << std::endl;
            describe_param(ss, prefix+std::string("    "), pv.value());
        }

        return ss.str ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::inc_ref ()
    {
        if (msg_handle != nullptr)
            dbus_message_ref (msg_handle);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Message::dec_ref ()
    {
        if (msg_handle != nullptr)
            dbus_message_unref (msg_handle);
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string Message::describe () const
    {
        std::stringstream ss;

        ss << "Message: " << std::endl;

        ss << "    Type:         ";
        switch (type()) {
        case DBUS_MESSAGE_TYPE_METHOD_CALL:
            ss << "Method call";
            break;
        case DBUS_MESSAGE_TYPE_METHOD_RETURN:
            ss << "Method return";
            break;
        case DBUS_MESSAGE_TYPE_SIGNAL:
            ss << "Signal";
            break;
        case DBUS_MESSAGE_TYPE_ERROR:
            ss << "Error";
            break;
        case DBUS_MESSAGE_TYPE_INVALID:
            ss << "Invalid message";
            break;
        default:
            ss << "Unknown";
            break;
        }
        ss << std::endl;
        ss << "    Sender:       " << sender() << std::endl;
        ss << "    Destination:  " << destination() << std::endl;
        ss << "    Path:         " << path() << std::endl;
        ss << "    Interface:    " << interface() << std::endl;
        ss << "    Name:         " << name() << std::endl;
        ss << "    Signature:    " << signature() << std::endl;
        ss << "    Serial:       " << serial() << std::endl;
        ss << "    Reply serial: " << reply_serial() << std::endl;
        ss << "    Parameters:" << std::endl;

        auto args = (const_cast<Message*>(this))->arguments ();
        for (auto arg : args)
            describe_param(ss, "        ", *arg);

        return ss.str ();
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static std::string type_to_string (int type)
    {
        switch (type) {
        case DBUS_TYPE_INVALID:
            return "DBUS_TYPE_INVALID";
        case DBUS_TYPE_BYTE:
            return "DBUS_TYPE_BYTE";
        case DBUS_TYPE_BOOLEAN:
            return "DBUS_TYPE_BOOLEAN";
        case DBUS_TYPE_INT16:
            return "DBUS_TYPE_INT16";
        case DBUS_TYPE_UINT16:
            return "DBUS_TYPE_UINT16";
        case DBUS_TYPE_INT32:
            return "DBUS_TYPE_INT32";
        case DBUS_TYPE_UINT32:
            return "DBUS_TYPE_UINT32";
        case DBUS_TYPE_INT64:
            return "DBUS_TYPE_INT64";
        case DBUS_TYPE_UINT64:
            return "DBUS_TYPE_UINT64";
        case DBUS_TYPE_DOUBLE:
            return "DBUS_TYPE_DOUBLE";
        case DBUS_TYPE_STRING:
            return "DBUS_TYPE_STRING";
        case DBUS_TYPE_OBJECT_PATH:
            return "DBUS_TYPE_OBJECT_PATH";
        case DBUS_TYPE_SIGNATURE:
            return "DBUS_TYPE_SIGNATURE";
        case DBUS_TYPE_UNIX_FD:
            return "DBUS_TYPE_UNIX_FD";
        case DBUS_TYPE_ARRAY:
            return "DBUS_TYPE_ARRAY";
        case DBUS_TYPE_VARIANT:
            return "DBUS_TYPE_VARIANT";
        case DBUS_TYPE_STRUCT:
            return "DBUS_TYPE_STRUCT";
        case DBUS_TYPE_DICT_ENTRY:
            return "DBUS_TYPE_DICT_ENTRY";
        }
        return "DBUS_TYPE_INVALID";
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static std::string value_to_string (const int type, const dbus_basic& value)
    {
        std::stringstream ss;

        switch (type) {
        case DBUS_TYPE_INVALID:
            ss << "<undefined>";
            break;
        case DBUS_TYPE_BYTE:
            ss << "0x" << std::setbase(16) << (unsigned int)value.byt() << std::setbase(10);
            if (isprint(value.byt()))
                ss << " '" << (char)value.byt() << "'";
            break;
        case DBUS_TYPE_BOOLEAN:
            ss << (static_cast<bool>(value.boolean()) ? "true" : "false");
            break;
        case DBUS_TYPE_INT16:
            ss << value.i16();
            break;
        case DBUS_TYPE_UINT16:
            ss << value.u16();
            break;
        case DBUS_TYPE_INT32:
            ss << value.i32();
            break;
        case DBUS_TYPE_UINT32:
            ss << value.u32();
            break;
        case DBUS_TYPE_INT64:
            ss << value.i64();
            break;
        case DBUS_TYPE_UINT64:
            ss << value.u64();
            break;
        case DBUS_TYPE_DOUBLE:
            ss << value.dbl();
            break;
        case DBUS_TYPE_STRING:
            ss << value.str();
            break;
        case DBUS_TYPE_OBJECT_PATH:
            ss << value.str();
            break;
        case DBUS_TYPE_SIGNATURE:
            ss << value.str();
            break;
        case DBUS_TYPE_UNIX_FD:
            ss << value.fd();
            break;
        case DBUS_TYPE_ARRAY:
            ss << "<array>";
            break;
        case DBUS_TYPE_VARIANT:
            ss << "<variant>";
            break;
        case DBUS_TYPE_STRUCT:
            ss << "<struct>";
            break;
        case DBUS_TYPE_DICT_ENTRY:
            ss << "<dict_entry>";
            break;
        default:
            ss << "<undefined>";
        }
        return ss.str();
    }

}
