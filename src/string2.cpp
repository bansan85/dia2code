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

#include "config.h"

#include "string2.hpp"

/**
 * This function returns the upper case char* of the one taken on input
 * The char * received may be freed by the caller
*/
std::string
strtoupper (const std::string & s) {
    std::string tmp (s);
    size_t i, n;
    n = tmp.length ();
    for (i = 0; i < n; i++) {
        tmp[i] = static_cast <char> (toupper (tmp[i]));
        if ((tmp[i] == SEPARATOR) || (tmp[i] == '-')) {
            tmp[i] = '_';
        }
    }
    return tmp;
}

/**
  * This function returns the a char* that has the first
  * character in upper case and the rest unchanged.
  * The char * received may be freed by the caller
*/
std::string
strtoupperfirst (const std::string & s) {
    std::string tmp (s);
    if (s.empty ()) {
        return s;
    }
    tmp[0] = static_cast <char> (toupper (tmp[0]));
    return tmp;
}


std::list <std::string>
parseClassNames (char *s) {
    char *token;
    const char *delim = ",";
    std::list <std::string> list;
#if defined(_WIN32) || defined(_WIN64)
    char *context = NULL;

    token = strtok_s (s, delim, &context);
#else
    token = strtok (s, delim);
#endif
    while ( token != NULL ) {
        list.push_back (token);
#if defined(_WIN32) || defined(_WIN64)
        token = strtok_s (nullptr, delim, &context);
#else
        token = strtok (nullptr, delim);
#endif
    }
    return list;
}


bool
isPresent (std::list <std::string> list, const char *name) {
    for (std::string str : list) {
        const char *namei = str.c_str ();
        size_t len;
        const char* mask;
        if ((namei == NULL) || (strcmp (namei, name) == 0)) {
            return true;
        }
        len = strlen (namei);
        if (len >= 2 && len <= strlen (name)
                && (mask = strchr (namei, '*')) != NULL
                && mask == strrchr (namei, '*')) {
            len--;
            if (mask == namei && !strcmp (namei + 1,
                                          name + strlen (name) - len) ) {
                return true;
            }
            if (mask == namei + len && ! strncmp (namei, name, len)) {
                return true;
            }
        }
    }
    return false;
}

const char *
kindStr (Kind k) {
    switch (k) {
        case Kind::UNKNOWN : {
            return "???";
        }
        case Kind::IN : {
            return "in";
        }
        case Kind::OUT :  {
            return "out";
        }
        case Kind::IN_OUT : {
            return "in/out";
        }
        default : {
            throw std::string ("Unknown kind.\n");
        }
    }
}

bool
isInside (const std::string & meule, const char * aiguille) {
    char * token = NULL;
    char * tmp;
    char * meule2 = new char [meule.length () + 1];

    strcpy (meule2, meule.c_str ());

    token = strtok_r (meule2, ",", &tmp);
    while (token != nullptr) {
        if (strcmp (token, aiguille) == 0) {
            delete [] meule2;
            return true;
        }
        token = strtok_r (nullptr, ",", &tmp);
    }
    delete [] meule2;
    return false;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
