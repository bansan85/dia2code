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

#ifndef CODE_GENERATORS
#define CODE_GENERATORS

#include "dia2code.hpp"

#define NO_GENERATORS 13

void generate_code_c(batch *b);
void generate_code_cpp(batch *b);
void generate_code_java(batch *b);
void generate_code_sql(batch *b);
void generate_code_ada(batch *b);
void generate_code_python(batch *b);
void generate_code_php(batch *b);
void generate_code_shp(batch *b);
void generate_code_idl(batch *b);
void generate_code_csharp(batch *b);
void generate_code_php_five(batch *b);
void generate_code_ruby(batch *b);
void generate_code_as3(batch *b);

void inherit_attributes(umlclasslist, umlattrlist);

#endif
