#
# Copyright (C) 2021,2023 Dan Arrhenius <dan@ultramarin.se>
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

if ENABLE_UTILS_SET

AM_CPPFLAGS = -I$(srcdir)/../src -I../src -DSYSCONFDIR='"${sysconfdir}"' -DLOCALSTATEDIR='"${localstatedir}"'
AM_CXXFLAGS = -Wall -pipe -O2 -g
AM_LDFLAGS =

LDADD = -L../src -lultrabus

AM_CXXFLAGS += $(dbus_CFLAGS) $(iomultiplex_CFLAGS) $(libxml2_CFLAGS)
AM_LDFLAGS += $(dbus_LIBS) $(iomultiplex_LIBS) $(libxml2_LIBS)
if HAVE_LIBXML2
AM_CXXFLAGS += $(libxml2_CFLAGS)
AM_LDFLAGS += $(libxml2_LIBS)
else
AM_CXXFLAGS += -DNO_LIBXML2
endif


bin_PROGRAMS =

bin_PROGRAMS += dbus-tool
dbus_tool_SOURCES  = dbus-tool.cpp
dbus_tool_SOURCES += dbus-tool-print-introspect.cpp
dbus_tool_SOURCES += dbus-tool-print-introspect.hpp
dbus_tool_SOURCES += dbus-tool-args.cpp
dbus_tool_SOURCES += dbus-tool-args.hpp
dbus_tool_SOURCES += dbus_arg_parser.cpp

dist_man1_MANS =
dist_man1_MANS += dbus-tool.man

endif
