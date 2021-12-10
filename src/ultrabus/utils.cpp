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
#include <ultrabus/utils.hpp>
#include <ultrabus/dbus_basic.hpp>
#include <ultrabus/dbus_struct.hpp>
#include <ultrabus/dbus_array.hpp>
#include <ultrabus/dbus_dict_entry.hpp>
#include <ultrabus/dbus_variant.hpp>
#include <dbus/dbus.h>
#include <string>
#include <map>


namespace ultrabus {



    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    std::string dbus_type_code_to_name (int dbus_type_code)
    {
        static std::map<int, std::string> type_names {{
                {DBUS_TYPE_BYTE, "byte"},
                {DBUS_TYPE_BOOLEAN, "boolean"},
                {DBUS_TYPE_INT16, "int16"},
                {DBUS_TYPE_UINT16, "uint16"},
                {DBUS_TYPE_INT32, "int32"},
                {DBUS_TYPE_UINT32, "uint32"},
                {DBUS_TYPE_INT64, "int64"},
                {DBUS_TYPE_UINT64, "uint64"},
                {DBUS_TYPE_DOUBLE, "double"},
                {DBUS_TYPE_STRING, "string"},
                {DBUS_TYPE_OBJECT_PATH, "object path"},
                {DBUS_TYPE_SIGNATURE, "signature"},
                {DBUS_TYPE_UNIX_FD, "unix fd"},
                {DBUS_TYPE_STRUCT, "struct"},
                {DBUS_TYPE_ARRAY, "array"},
                {DBUS_TYPE_DICT_ENTRY, "dict entry"},
                {DBUS_TYPE_VARIANT, "variant"}
            }};
        auto i = type_names.find (dbus_type_code);
        return i!=type_names.end() ? i->second : "invalid";
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type_ptr clone_dbus_type (const dbus_type& mt)
    {
        if (typeid(mt) == typeid(dbus_basic)) {
            return std::make_shared<dbus_basic> (dynamic_cast<const dbus_basic&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_struct)) {
            return std::make_shared<dbus_struct> (dynamic_cast<const dbus_struct&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_array)) {
            return std::make_shared<dbus_array> (dynamic_cast<const dbus_array&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_dict_entry)) {
            return std::make_shared<dbus_dict_entry> (dynamic_cast<const dbus_dict_entry&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_variant)) {
            return std::make_shared<dbus_variant> (dynamic_cast<const dbus_variant&>(mt));
        }
        return nullptr;
    }


    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    dbus_type_ptr clone_dbus_type (dbus_type&& mt)
    {
        if (typeid(mt) == typeid(dbus_basic)) {
            //return std::make_shared<dbus_basic> (std::forward<dbus_basic>(dynamic_cast<dbus_basic&&>(mt)));
            return std::make_shared<dbus_basic> (std::forward<dbus_basic>(mt));
        }
        else if (typeid(mt) == typeid(dbus_struct)) {
            return std::make_shared<dbus_struct> (dynamic_cast<dbus_struct&&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_array)) {
            return std::make_shared<dbus_array> (dynamic_cast<dbus_array&&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_dict_entry)) {
            return std::make_shared<dbus_dict_entry> (dynamic_cast<dbus_dict_entry&&>(mt));
        }
        else if (typeid(mt) == typeid(dbus_variant)) {
            return std::make_shared<dbus_variant> (dynamic_cast<dbus_variant&&>(mt));
        }
        return nullptr;
    }



}
