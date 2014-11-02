/*
Copyright (C) 2001-2014 Ruben Lopez

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "dia2code.hpp"

umlclasslist find_by_name(umlclasslist list, const char * name ) {
    if ( name != NULL && strlen(name) > 0 ) {
        while ( list != NULL ) {
            if ( list->key->name.compare (name) == 0) {
                return list;
            }
            list = list->next;
        }
    }
    return NULL;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
