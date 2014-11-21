/*
Copyright (C) 2000-2014 Javier O'Hara

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

#define kind_str(A)   ((A)=='1'?"in":((A)=='2'?"in/out":((A)=='3'?"out":"???")))

struct umlattribute {
    std::string name;
    std::string value;
    std::string type;
    std::string comment;
    char visibility;
    bool isabstract : 1;
    bool isstatic : 1;
    bool isconstant : 1;
    char kind;
    char diaoid[10];
};

struct umloperation {
    umlattribute attr;
    std::list <umlattribute> parameters;
    std::string implementation;
};

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
    struct umlpackage * parent;
    std::string directory;
};

struct umlpackagenode {
    umlpackage *key;
    struct umlpackagenode * next;
};

typedef umlpackagenode * umlpackagelist;

struct umlclass {
    std::string id;
    std::string name;
    std::string stereotype;
    std::string comment;
    int isabstract;
    std::list <umlattribute> attributes;
    std::list <umloperation> operations;
    std::list <umltemplate> templates;
    umlpackage * package;
    geometry geom;
};

struct umlassocnode {
    umlclass * key;
    std::string name;
    char composite;
    struct umlassocnode * next;
    char multiplicity[10]; /* association can declare multiplicity */
};

typedef umlassocnode * umlassoclist;

struct umlclassnode {
    umlclass * key;
    struct umlclassnode * parents;
    struct umlassocnode * associations;
    struct umlclassnode * dependencies;
    struct umlclassnode * next;
};

typedef umlclassnode * umlclasslist;

std::string strtoupper(std::string s);
std::string strtoupperfirst(std::string s);
std::list <std::string> parse_class_names (char *s);
int is_present(std::list <std::string> list, const char *name);
umlclasslist find_by_name(umlclasslist list, const char * name);

int is_enum_stereo (const char * stereo);
int is_struct_stereo (const char * stereo);
int is_typedef_stereo (const char * stereo);
int is_const_stereo (const char * stereo);

umlpackagelist make_package_list( umlpackage * package);

char *find_diaoid( const char *buf, char **newpos  );
void d2c_log( int level, char * msg );

struct param_list
{
  std::string name;
  std::string value;
  struct param_list *next;
};

extern int indent_count;
extern int indent_open_brace_on_newline;
extern int generate_backup;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
