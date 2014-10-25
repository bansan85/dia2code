/***************************************************************************
                          parse_diagram.h  -  The parser of the Dia file
                             -------------------
    begin                : Sat Dec 16 2000
    copyright            : (C) 2000-2001 by Javier O'Hara
    email                : joh314@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSE_DIAGRAM_HPP
#define PARSE_DIAGRAM_HPP
#include "dia2code.hpp"

umlclasslist parse_diagram(char *diafile);
umlattrlist insert_attribute(umlattrlist n, umlattrlist l);

#endif
