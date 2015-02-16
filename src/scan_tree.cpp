/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2001-2014 Ruben Lopez

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


#include "config.h"

#include <iostream>

#include "scan_tree.hpp"

umlClassNode * findByName (std::list <umlClassNode> & list,
                           const std::string & name ) {
    umlClassNode * ret = NULL;
    if (!name.empty ()) {
        for (umlClassNode & it : list) {
            if ( it.getName ().compare (name) == 0) {
                if (ret != NULL) {
                    std::cerr << "More than one class as the same name "
                              << name << ". \n";
                }
                ret = &it;
            }
        }
    }
    return ret;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
