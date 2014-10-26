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

#ifndef PARSE_DIAGRAM_HPP
#define PARSE_DIAGRAM_HPP
#include "dia2code.hpp"

umlclasslist parse_diagram(char *diafile);
umlattrlist insert_attribute(umlattrlist n, umlattrlist l);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */