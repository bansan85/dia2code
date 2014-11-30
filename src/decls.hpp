/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014

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

struct module {  /* UML package = IDL module
                    What we call `module' equally applies to C++ (namespace)
                    and Ada (package.) I don't know about other languages.  */
    umlpackage pkg;
    std::list<declaration> contents;
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
};

/* Utilities for building the global `decls' from umlclassnodes and
   their parents.  (`decls' contains everything in ascending order of
   interdependence.)  */

module *
find_or_add_module (std::list <declaration> &dptr, std::list <umlpackage> &pkglist);

declaration * find_class (umlclassnode &node, std::list <declaration> &decl);

#endif  /* DECLS_H */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
