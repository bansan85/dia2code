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
    float posX;
    float posY;
    float width;
    float height;
};

void parseGeomPosition (xmlNodePtr attribute, geometry * geom);
void parseGeomWidth (xmlNodePtr attribute, geometry * geom);
void parseGeomHeight (xmlNodePtr attribute, geometry * geom);

class umlClass {
    private :
        std::string id;
        std::string name;
        std::string comment;
        bool abstract : 1;
        bool stereotypeTypedef : 1;
        bool stereotypeEnum : 1;
        bool stereotypeConst : 1;
        bool stereotypeStruct : 1;
        bool stereotypeGetSet : 1;
        bool stereotypeExtern : 1;
        bool stereotypeInterface : 1;
        bool stereotypeDllExport : 1;
#ifdef ENABLE_CORBA
        bool stereotypeCorba : 1;
#endif
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
        static bool isTypedefStereo (std::string & stereo);
        static bool isEnumStereo (std::string & stereo);
        static bool isConstStereo (std::string & stereo);
        static bool isStructStereo (std::string & stereo);
        static bool isGetSetStereo (std::string & stereo);
        static bool isInterfaceStereo (std::string & stereo);
        static bool isDllExportStereo (std::string & stereo);
        static bool isCorbaStereo (std::string & stereo);

        umlClass ();
        
        const std::string & getId () const;
        const std::string & getName () const;
        const std::string & getComment () const;
        bool isAbstract () const;
        bool isStereotypeTypedef () const;
        bool isStereotypeEnum () const;
        bool isStereotypeConst () const;
        bool isStereotypeStruct () const;
        void setStereotypeStruct (bool val);
        bool isStereotypeGetSet () const;
        bool isStereotypeCorba () const;
        bool isStereotypeExtern () const;
        bool isStereotypeInterface () const;
        bool isStereotypeDllExport () const;
        const std::list <umlAttribute> & getAttributes () const;
        const std::list <umlOperation> & getOperations () const;
        umlPackage * getPackage () const;
        const std::list <std::pair <std::string, std::string> > &
                                                         getTemplates () const;
        
        void makeGetSetMethods ();
        static void lolipopImplementation (std::list <umlClassNode> & classlist,
                                           xmlNodePtr object);
        static bool parseDiagram (char *diafile,
                                  std::list <umlClassNode> & res);
        void parseClass (xmlNodePtr class_);

        ~umlClass ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
