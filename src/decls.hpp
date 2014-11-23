/*
Copyright (C) 2000-2014

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

#ifndef DECLS_H
#define DECLS_H

#include "dia2code.hpp"

/* Package processing auxiliary structure:
   "Declarations" are UML packages (which map to IDL module or C++
   namespace or Ada package or ...) or UML classes (which map to various
   other language constructs.)
   For each top level declaration, a separate file is generated.
   Nested packages are generated into the same file as nested modules.
   `struct declaration' is a buffer that gathers together classes and
   packages ordered by their dependencies, going from least dependencies
   to most.  We cannot handle circular dependencies yet.  */

struct declaration;

extern declaration *decls;

struct module {  /* UML package = IDL module
                    What we call `module' equally applies to C++ (namespace)
                    and Ada (package.) I don't know about other languages.  */
    umlpackage pkg;
    struct declaration *contents;
};

typedef enum { dk_module, dk_class } decl_kind_t;

struct declaration {
    decl_kind_t decl_kind;
    union /* switch(decl_kind) */ {
    /* case dk_module: */
        module *this_module;
    /* case dk_class:  */
        umlclassnode *this_class;
        /* In `this_class', `next' is not used since we use our own
           sequencing (see `prev' and `next' below.)  */
    } u;
    struct declaration *prev, *next;  /* other declarations in this scope */
};

/* Utilities for building the global `decls' from umlclassnodes and
   their parents.  (`decls' contains everything in ascending order of
   interdependence.)  */

module *
find_or_add_module (declaration **dptr, std::list <umlpackage> &pkglist);

declaration * find_class (umlclassnode &node);

declaration * append_decl (declaration *d);

#endif  /* DECLS_H */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
