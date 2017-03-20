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


#ifndef STRING2_HPP
#define STRING2_HPP

#include <list>
#include <string>
#include <cstring>
#include <stdlib.h>
/* for mkdir and mode_t */
#include <sys/types.h>
#include <sys/stat.h>

#include "umlAttribute.hpp"

#define BAD_CAST2 reinterpret_cast <const xmlChar *>
#define BAD_TSAC2 reinterpret_cast <const char *>

#if defined(_WIN32) || defined(_WIN64)
#define SEPARATOR '\\'
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#else
#define SEPARATOR '/'
#endif

std::string strtoupper (const std::string & s);
std::string strtoupperfirst (const std::string & s);
std::string strtolower (const std::string & s);
std::list <std::string> parseClassNames (char *s);
bool isPresent (std::list <std::string> list, const char *name);
const char * kindStr (Kind k);

/**
 * Find `aiguille' in the segments of `meule' where `meule' is a CSV (Comma Separated Value) list.
 * @param meule  The haystack to search.
 * @param aiguille The needle to find.
 * @return True if found, false if not found.
 */
bool isInside (const std::string & meule, const char * aiguille);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
