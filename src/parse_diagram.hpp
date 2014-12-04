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


#ifndef PARSE_DIAGRAM_HPP
#define PARSE_DIAGRAM_HPP

#include "dia2code.hpp"

#include <list>


void parse_diagram(char *diafile, std::list <umlclassnode> & res);
void parse_dia_node(xmlNodePtr stringnode, std::string &buffer);
bool parse_boolean(xmlNodePtr booleannode);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
