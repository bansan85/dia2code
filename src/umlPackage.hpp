/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2014-2014 Vincent Le Garrec

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

#ifndef UMLPACKAGE_HPP
#define UMLPACKAGE_HPP

#include "config.h"

#include <string>
#include <libxml/tree.h>

#include "umlClass.hpp"

class umlPackage {
    private :
        std::string id;
        std::string name;
        geometry geom;
        umlPackage *parent;
        std::string directory;
    public :
        umlPackage ();
        umlPackage (xmlNodePtr package, std::string id_);
        umlPackage (const umlPackage & pack);

        const std::string getName () const;
        const geometry & getGeometry () const;
        const umlPackage * getParent () const;
        void setParent (umlPackage * pack);

        static void make_package_list (const umlPackage * package,
                                       std::list <umlPackage> & res);
        static void make_package_list_name (const umlPackage * package,
                                            std::list <std::string> & res);
        
        ~umlPackage ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
