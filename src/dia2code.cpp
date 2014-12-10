/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014 Javier O'Hara

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>

#include "dia2code.hpp"

int generate_backup;

/**
 * This function returns the upper case char* of the one taken on input
 * The char * received may be freed by the caller
*/
std::string strtoupper(std::string s) {
    std::string tmp (s);
    size_t i, n;
    n = tmp.length ();
    for (i = 0; i < n; i++) {
        tmp[i] = static_cast <char> (toupper(tmp[i]));
    }
    return tmp;
}

/**
  * This function returns the a char* that has the first
  * character in upper case and the rest unchanged.
  * The char * received may be freed by the caller
*/
std::string strtoupperfirst(std::string s) {
    std::string tmp (s);
    if (s.empty ())
        return s;
    tmp[0] = static_cast <char> (toupper(tmp[0]));
    return tmp;
}


std::list <std::string> parse_class_names (char *s) {
    char *token;
    const char *delim = ",";
    std::list <std::string> list;

    token = strtok (s, delim);
    while ( token != NULL ) {
        list.push_back (token);
        token = strtok (NULL, delim);
    }
    return list;
}


int is_present(std::list <std::string> list, const char *name) {
    for (std::string str : list) {
        const char *namei = str.c_str ();
        size_t len;
        const char* mask;
        if ( ! strcmp(namei, name) ) {
            return 1;
        }
        len = strlen(namei);
        if (len >= 2 && len <= strlen(name)
                && (mask = strchr(namei, '*')) != NULL
                && mask == strrchr(namei, '*') ) {
            len--;
            if ( mask == namei && ! strcmp(namei+1, name+strlen(name)-len) ) {
                return 1;
            }
            if ( mask == namei+len && ! strncmp(namei, name, len) ) {
                return 1;
            }
        }
    }
    return 0;
}

int
is_enum_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "enum") ||
            !strcasecmp (stereo, "enumeration") ||
            !strcmp (stereo, "CORBAEnum"));
}

int
is_struct_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "struct") ||
            !strcasecmp (stereo, "structure") ||
            !strcmp (stereo, "CORBAStruct"));
}

int
is_typedef_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "typedef") ||
            !strcmp (stereo, "CORBATypedef"));
}

int
is_const_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "const") ||
            !strcasecmp (stereo, "constant") ||
            !strcmp (stereo, "CORBAConstant"));
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
