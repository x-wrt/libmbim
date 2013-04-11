#!/usr/bin/env python
# -*- Mode: python; tab-width: 4; indent-tabs-mode: nil -*-
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Copyright (C) 2013 Aleksander Morgado <aleksander@gnu.org>
#

from Value import Value

"""
The ValueStringArray class takes care of string array variables
"""
class ValueStringArray(Value):

    """
    Constructor
    """
    def __init__(self, dictionary):

        # Call the parent constructor
        Value.__init__(self, dictionary)

        """ The type of the variable """
        self.type = 'string_array'

        """ The public format of the value """
        self.public_format = 'gchar **'

        """ Type of the value when used as input parameter """
        self.in_format = 'const gchar *const *'
        self.in_description = 'The \'' + self.name + '\' field, given as an array of strings.'

        """ Type of the value when used as output parameter """
        self.out_format = 'gchar ***'
        self.out_description = 'Return location for a newly allocated array of strings, or %NULL if the \'' + self.name + '\' field is not needed. Free the returned value with g_strfreev().'

        """ The name of the method used to read the value """
        self.reader_method_name = '_mbim_message_read_string_array'

        """ Whether this value is an array """
        self.is_array = True

        """ The size of a member of the array """
        self.array_member_size = 8

        """ The field giving the size of the array """
        self.array_size_field = dictionary['array-size-field']