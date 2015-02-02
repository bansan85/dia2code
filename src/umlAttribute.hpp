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

#ifndef UMLATTRIBUTE_HPP
#define UMLATTRIBUTE_HPP

#include "config.h"

#include <list>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <libxml/tree.h>

enum class Visibility {
    PUBLIC,
    PRIVATE,
    PROTECTED,
    IMPLEMENTATION
};

enum class Inheritance {
    ABSTRACT,
    VIRTUAL,
    FINAL
};

enum class Kind {
    UNKNOWN,
    IN,
    OUT,
    IN_OUT
};

class umlAttribute {
    private :
        std::string name;
        std::string value;
        std::string type;
        std::string comment;
        Visibility visibility;
        Inheritance inheritance;
        unsigned char isstatic : 1;
        unsigned char isconstant : 1;
        Kind kind;
    public :
        umlAttribute ();
        umlAttribute (std::string name_,
                      std::string value_,
                      std::string type_,
                      std::string comment_,
                      Visibility visibility_,
                      Inheritance inheritance_,
                      unsigned char isstatic_,
                      unsigned char isconstant_,
                      Kind kind_);
        
        const std::string & getName () const;
        const std::string & getValue () const;
        const std::string & getType () const;
        const std::string & getComment () const;
        const Visibility & getVisibility () const;
        Inheritance getInheritance () const;
        unsigned char isStatic () const;
        unsigned char isConstant () const;
        Kind getKind () const;

        void assign (std::string name_,
                     std::string value_,
                     std::string type_,
                     std::string comment_,
                     Visibility visibility_,
                     Inheritance inheritance_,
                     unsigned char isstatic_,
                     unsigned char isconstant_,
                     Kind kind_);

        void check (const char *typename_) const;
        void parse (xmlNodePtr node);
        void insert (std::list <umlAttribute> &l);

        ~umlAttribute ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
