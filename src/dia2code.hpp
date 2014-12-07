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


#ifndef DIA2CODE_H
#define DIA2CODE_H

#include "config.h"

#include <list>
#include <string>
#include <cstring>
#include <stdlib.h>
/* for mkdir and mode_t */
#include <sys/types.h>
#include <sys/stat.h>

#include "umlAttribute.hpp"
#include "umlOperation.hpp"

#define kind_str(A)   ((A)=='1'?"in":((A)=='2'?"in/out":((A)=='3'?"out":"???")))
#define BAD_CAST2 reinterpret_cast <const xmlChar *>
#define BAD_TSAC2 reinterpret_cast <const char *>

struct umltemplate {
    std::string name;
    std::string type;
};


struct geometry {
    float pos_x;
    float pos_y;
    float width;
    float height;
};

struct umlpackage {
    std::string id;
    std::string name;
    geometry geom;
    struct umlpackage *parent;
    std::string directory;
};

struct umlclass {
    std::string id;
    std::string name;
    std::string stereotype;
    std::string comment;
    int isabstract;
    std::list <umlAttribute> attributes;
    std::list <umlOperation> operations;
    std::list <umltemplate> templates;
    umlpackage *package;
    geometry geom;
};

struct umlassoc {
    umlclass key;
    std::string name;
    char composite;
    // Unused
    char multiplicity[10];
};

struct umlclassnode {
    umlclass key;
    std::list <umlclassnode> parents;
    std::list <umlassoc> associations;
    std::list <umlclassnode> dependencies;
};

std::string strtoupper(std::string s);
std::string strtoupperfirst(std::string s);
std::list <std::string> parse_class_names (char *s);
int is_present(std::list <std::string> list, const char *name);
umlclassnode * find_by_name(std::list <umlclassnode> & list, const char * name);

int is_enum_stereo (const char * stereo);
int is_struct_stereo (const char * stereo);
int is_typedef_stereo (const char * stereo);
int is_const_stereo (const char * stereo);

void make_package_list(umlpackage *package, std::list <umlpackage> &res);

extern int generate_backup;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
