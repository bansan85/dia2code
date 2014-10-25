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

#ifndef INCLUDES_H
#define INCLUDES_H

/* Includefile computation related stuff (equally applicable to
   Ada with clause computation and Java package use computation)  */

#include "dia2code.hpp"
#include "decls.hpp"

extern namelist includes;

extern int have_include (char *name);
extern void add_include (char *name);
extern void push_include (umlclassnode *node);
extern void determine_includes (declaration *d, batch *b);

#endif  /* INCLUDES_H */

