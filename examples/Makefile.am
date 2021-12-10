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

if ENABLE_EXAMPLES_SET

AM_CPPFLAGS = -I$(srcdir)/../src -I../src -DSYSCONFDIR='"${sysconfdir}"' -DLOCALSTATEDIR='"${localstatedir}"'
AM_CXXFLAGS = -Wall -pipe -O2 -g
AM_LDFLAGS =

LDADD = -L../src -lultrabus

AM_CXXFLAGS += $(dbus_CFLAGS) $(iomultiplex_CFLAGS)
AM_LDFLAGS += $(dbus_LIBS) $(iomultiplex_LIBS)


noinst_bindir =
noinst_bin_PROGRAMS =

noinst_bin_PROGRAMS += example-service
example_service_SOURCES = example-service.cpp

endif
