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

#ifndef UMLCLASS_HPP
#define UMLCLASS_HPP

#include "config.h"

#include <list>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <libxml/tree.h>

#include "umlAttribute.hpp"
#include "umlOperation.hpp"
// No include to avoid circular dependencies.
class umlClassNode;
class umlPackage;

struct geometry {
    float pos_x;
    float pos_y;
    float width;
    float height;
};

void parse_geom_position (xmlNodePtr attribute, geometry * geom);
void parse_geom_width (xmlNodePtr attribute, geometry * geom);
void parse_geom_height (xmlNodePtr attribute, geometry * geom);

class umlClass {
    private :
        std::string id;
        std::string name;
        std::string stereotype;
        std::string comment;
        bool isabstract;
        std::list <umlAttribute> attributes;
        std::list <umlOperation> operations;
/*
 * struct umltemplate {
    std::string name;
    std::string type;
};*/
        std::list <std::pair <std::string, std::string> > templates;
        umlPackage *package;
        geometry geom;
    public :
        umlClass ();
        
        const std::string & getId () const;
        const std::string & getName () const;
        const std::string & getStereotype () const;
        const std::string & getComment () const;
        const std::list <umlAttribute> & getAttributes () const;
        const std::list <umlOperation> & getOperations () const;
        const umlPackage * getPackage () const;
        const std::list <std::pair <std::string, std::string> > &
                                                         getTemplates () const;
        
        void makeGetSetMethods ();
        static void lolipop_implementation (std::list <umlClassNode> & classlist,
                                            xmlNodePtr object);
        static void parse_diagram (char *diafile,
                                   std::list <umlClassNode> & res);
        void parse_class (xmlNodePtr class_);

        ~umlClass ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
