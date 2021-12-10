#
# Copyright (C) 2021 Dan Arrhenius <dan@ultramarin.se>
#
# This file is part of libultrabus.
#
# libultrabus is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
lib_LTLIBRARIES = libultrabus.la

AM_CPPFLAGS = -I$(srcdir) -DSYSCONFDIR='"${sysconfdir}"' -DLOCALSTATEDIR='"${localstatedir}"'
AM_LDFLAGS =

AM_CXXFLAGS = -Wall -pipe -O2 -g
#AM_YFLAGS = -d
#AM_LFLAGS =

AM_CXXFLAGS += $(dbus_CFLAGS) $(iomultiplex_CFLAGS)
AM_LDFLAGS += $(dbus_LIBS) $(iomultiplex_LIBS)


# CURRENT : REVISION : AGE
AM_LDFLAGS += -version-info @LIBRARY_VERSION@


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ultrabus.pc

# Generated source and header files
BUILT_SOURCES =

#CLEANFILES = $(BUILT_SOURCES)


# Source files
libultrabus_la_SOURCES  =
libultrabus_la_SOURCES += ultrabus/types.cpp
libultrabus_la_SOURCES += ultrabus/dbus_type_base.cpp
libultrabus_la_SOURCES += ultrabus/dbus_type.cpp
libultrabus_la_SOURCES += ultrabus/dbus_basic.cpp
libultrabus_la_SOURCES += ultrabus/dbus_array.cpp
libultrabus_la_SOURCES += ultrabus/dbus_dict_entry.cpp
libultrabus_la_SOURCES += ultrabus/dbus_struct.cpp
libultrabus_la_SOURCES += ultrabus/dbus_variant.cpp
libultrabus_la_SOURCES += ultrabus/Properties.cpp
libultrabus_la_SOURCES += ultrabus/MessageParamIterator.cpp
libultrabus_la_SOURCES += ultrabus/Message.cpp
libultrabus_la_SOURCES += ultrabus/Connection.cpp
libultrabus_la_SOURCES += ultrabus/MessageHandler.cpp
libultrabus_la_SOURCES += ultrabus/CallbackMessageHandler.cpp
libultrabus_la_SOURCES += ultrabus/ObjectHandler.cpp
libultrabus_la_SOURCES += ultrabus/CallbackObjectHandler.cpp
libultrabus_la_SOURCES += ultrabus/ObjectProxy.cpp
libultrabus_la_SOURCES += ultrabus/utils.cpp
libultrabus_la_SOURCES += ultrabus/org_freedesktop_DBus.cpp
libultrabus_la_SOURCES += ultrabus/org_freedesktop_DBus_Peer.cpp
libultrabus_la_SOURCES += ultrabus/org_freedesktop_DBus_Introspectable.cpp
libultrabus_la_SOURCES += ultrabus/org_freedesktop_DBus_ObjectManager.cpp
libultrabus_la_SOURCES += ultrabus/org_freedesktop_DBus_Properties.cpp
#libultrabus_la_SOURCES += ultrabus/

# Header files
libultrabusdir = $(includedir)
nobase_libultrabus_HEADERS =
nobase_libultrabus_HEADERS += ultrabus.hpp
nobase_libultrabus_HEADERS += ultrabus/types.hpp
nobase_libultrabus_HEADERS += ultrabus/retvalue.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_type_base.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_type.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_basic.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_array.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_dict_entry.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_struct.hpp
nobase_libultrabus_HEADERS += ultrabus/dbus_variant.hpp
nobase_libultrabus_HEADERS += ultrabus/Properties.hpp
nobase_libultrabus_HEADERS += ultrabus/MessageParamIterator.hpp
nobase_libultrabus_HEADERS += ultrabus/Message.hpp
nobase_libultrabus_HEADERS += ultrabus/Connection.hpp
nobase_libultrabus_HEADERS += ultrabus/MessageHandler.hpp
nobase_libultrabus_HEADERS += ultrabus/CallbackMessageHandler.hpp
nobase_libultrabus_HEADERS += ultrabus/ObjectHandler.hpp
nobase_libultrabus_HEADERS += ultrabus/CallbackObjectHandler.hpp
nobase_libultrabus_HEADERS += ultrabus/ObjectProxy.hpp
nobase_libultrabus_HEADERS += ultrabus/utils.hpp
nobase_libultrabus_HEADERS += ultrabus/org_freedesktop_DBus.hpp
nobase_libultrabus_HEADERS += ultrabus/org_freedesktop_DBus_Peer.hpp
nobase_libultrabus_HEADERS += ultrabus/org_freedesktop_DBus_Introspectable.hpp
nobase_libultrabus_HEADERS += ultrabus/org_freedesktop_DBus_ObjectManager.hpp
nobase_libultrabus_HEADERS += ultrabus/org_freedesktop_DBus_Properties.hpp
#nobase_libultrabus_HEADERS += ultrabus/

# Header files that is not to be installed
#noinst_HEADERS = not_included_in_installation.hpp
