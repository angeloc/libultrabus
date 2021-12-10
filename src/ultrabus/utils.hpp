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
#ifndef ULTRABUS_UTILS_HPP
#define ULTRABUS_UTILS_HPP

#include <memory>
#include <chrono>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>

#include <ultrabus/types.hpp>
#include <ultrabus/dbus_type.hpp>


namespace ultrabus {

    /**
     * Make a clone of a dbus_type object and return a shared pointer to it.
     */
    dbus_type_ptr clone_dbus_type (const dbus_type& t);

    /**
     * Make a clone of a dbus_type object and return a shared pointer to it.
     */
    dbus_type_ptr clone_dbus_type (dbus_type&& t);

    /**
     * Return a string describing a DBus type code.
     */
    std::string dbus_type_code_to_name (int dbus_type_code);
}


#endif
