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

#include "config.h"

#include <iostream>

#include "umlClassNode.hpp"
#include "umlAttribute.hpp"
#include "string2.hpp"
#include "parse_diagram.hpp"

umlAttribute::umlAttribute () :
    name (),
    value (),
    type (),
    comment (),
    visibility (Visibility::PUBLIC),
    inheritance (Inheritance::FINAL),
    isstatic (false),
    isconstant (false),
    kind (Kind::UNKNOWN)
{
}

umlAttribute::umlAttribute (std::string name_,
                            std::string value_,
                            std::string type_,
                            std::string comment_,
                            Visibility visibility_,
                            Inheritance inheritance_,
                            bool isstatic_,
                            bool isconstant_,
                            Kind kind_) :
    name (name_),
    value (value_),
    type (type_),
    comment (comment_),
    visibility (visibility_),
    inheritance (inheritance_),
    isstatic (isstatic_),
    isconstant (isconstant_),
    kind (kind_)
{
}

const std::string &
umlAttribute::getName () const
{
    return name;
}

const std::string &
umlAttribute::getValue () const
{
    return value;
}

const std::string &
umlAttribute::getType () const
{
    return type;
}

const std::string &
umlAttribute::getComment () const
{
    return comment;
}

const Visibility &
umlAttribute::getVisibility () const
{
    return visibility;
}

void
umlAttribute::setVisibility (Visibility visible) {
    visibility = visible;
}

const Inheritance &
umlAttribute::getInheritance () const
{
    return inheritance;
}

bool
umlAttribute::isStatic () const
{
    return isstatic;
}

bool
umlAttribute::isConstant () const
{
    return isconstant;
}

Kind
umlAttribute::getKind () const
{
    return kind;
}

void
umlAttribute::assign (std::string name_,
                      std::string value_,
                      std::string type_,
                      std::string comment_,
                      Visibility visibility_,
                      Inheritance inheritance_,
                      bool isstatic_,
                      bool isconstant_,
                      Kind kind_)
{
    name = name_;
    value = value_;
    type = type_;
    comment = comment_;
    visibility = visibility_;
    inheritance = inheritance_;
    isstatic = isstatic_ & 1;
    isconstant = isconstant_ & 1;
    kind = kind_;
}

void
umlAttribute::check (const umlClassNode & node) const {
    if (node.isStereotypeTypedef ()) {
        if (!name.empty ()) {
            std::cerr << "Typedef \"" << node.getName () << "\", attribute \""
                      << name
                      << "\": ignoring name field in typedef class.\n";
        }
    }
    else {
        if (name.empty ()) {
            std::cerr << "Class \"" << node.getName () << "\": an unamed attribute is found.\n";
        }
    }
    if ((!node.isStereotypeEnum ()) && (type.empty ())) {
        std::cerr << "Class \"" << node.getName () << "\", attribute \""
                  << name << "\": no type defined.\n";
    }
    if ((node.isStereotypeEnum ()) && (visibility != Visibility::PUBLIC)) {
        std::cerr << "Class \"" << node.getName () << "\", attribute \""
                  << name << "\": visibility forced to public.\n";
    }
}

void
umlAttribute::parse (xmlNodePtr node) {
    xmlChar *attrval;

    name.clear ();
    value.clear ();
    type.clear ();
    comment.clear ();
    visibility = Visibility::PUBLIC;
    kind = Kind::UNKNOWN;
    while (node != NULL) {
        xmlChar *nodename;
        nodename = xmlGetProp (node, BAD_CAST2 ("name"));

        if (nodename == NULL) {
            std::cerr << "Failed to get name of a node (" <<
                node->name << ")." << std::endl <<
                "Parent is \"" << node->parent->name <<
                "\"" << std::endl;
        }
        else if (!strcmp ("name", BAD_TSAC2 (nodename))) {
            parseDiaNode (node->xmlChildrenNode, name);
        } else if (!strcmp ("value", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parseDiaNode (node->xmlChildrenNode, value);
            }
        } else if (!strcmp ("type", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parseDiaNode (node->xmlChildrenNode, type);
            } else {
                type.clear ();
            }
        } else if (!strcmp("comment", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
               parseDiaNode (node->xmlChildrenNode, comment);
            } else {
               comment.clear ();
          }
        } else if (!strcmp ("kind", BAD_TSAC2 (nodename))) {
            char tmp;
            attrval = xmlGetProp (node->xmlChildrenNode, BAD_CAST2 ("val"));
            if (attrval != NULL) {
                tmp = attrval[0];
                switch (tmp) {
                    case '0' : {
                        kind = Kind::UNKNOWN;
                        break;
                    }
                    case '1' : {
                        kind = Kind::IN;
                        break;
                    }
                    case '2' : {
                        kind = Kind::OUT;
                        break;
                    }
                    case '3' : {
                        kind = Kind::IN_OUT;
                        break;
                    }
                    default : {
                        std::cerr << "Unknown kind: "  <<
                            std::string (1, tmp) << std::endl;
                    }
                }
                free (attrval);
            }
            else {
                std::cerr << "Failed to get val of a node (" <<
                    node->xmlChildrenNode->name << ")." << std::endl <<
                    "Parent is \"" << node->xmlChildrenNode->parent->name <<
                    "\"" << std::endl;
            }
        } else if (!strcmp ("visibility", BAD_TSAC2 (nodename))) {
            char tmp;
            attrval = xmlGetProp (node->xmlChildrenNode, BAD_CAST2 ("val"));
            if (attrval != NULL) {
                tmp = attrval[0];
                switch (tmp) {
                    case '0' : {
                        visibility = Visibility::PUBLIC;
                        break;
                    }
                    case '1' : {
                        visibility = Visibility::PRIVATE;
                        break;
                    }
                    case '2' : {
                        visibility = Visibility::PROTECTED;
                        break;
                    }
                    case '3' : {
                        visibility = Visibility::IMPLEMENTATION;
                        break;
                    }
                    default : {
                        std::cerr << "Unknown visibility: " <<
                            std::string (1, tmp) << std::endl;
                    }
                }
                free (attrval);
            }
            else {
                std::cerr << "Failed to get val of a node (" <<
                    node->xmlChildrenNode->name << ")." << std::endl <<
                    "Parent is \"" << node->xmlChildrenNode->parent->name <<
                    "\"" << std::endl;
            }
        } else if (!strcmp ("inheritance_type", BAD_TSAC2 (nodename))) {
            char inheritance_tmp;
            attrval = xmlGetProp (node->xmlChildrenNode, BAD_CAST2 ("val"));
            if (attrval != NULL) {
                inheritance_tmp = attrval[0];
                switch (inheritance_tmp) {
                    case '0' : {
                        inheritance = Inheritance::ABSTRACT;
                        break;
                    }
                    case '1' : {
                        inheritance = Inheritance::VIRTUAL;
                        break;
                    }
                    case '2' : {
                        inheritance = Inheritance::FINAL;
                        break;
                    }
                    default : {
                        std::cerr << "Unknown inheritance : " <<
                            std::string (1, inheritance_tmp) << std::endl;
                    }
                }
                free (attrval);
            }
            else {
                std::cerr << "Failed to get val of a node (" <<
                    node->xmlChildrenNode->name << ")." << std::endl <<
                    "Parent is \"" << node->xmlChildrenNode->parent->name <<
                    "\"" << std::endl;
            }
        } else if (!strcmp ("class_scope", BAD_TSAC2 (nodename))) {
            isstatic = parseBoolean (node->xmlChildrenNode);
        } else if (!strcmp ("query", BAD_TSAC2 (nodename))) {
            isconstant = parseBoolean (node->xmlChildrenNode);
        }

        free (nodename);
        node = node->next;
    }
}

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void
umlAttribute::insert (std::list <umlAttribute> &l) {
    l.push_back (*this);
/*    std::list <umlAttribute>::iterator itl;
    
    itl = l.begin ();
    
    if (itl == l.end ()) {
        l.push_back (*this);
    }
    else {
        while ((itl != l.end ()) && ((*itl).visibility >= visibility)) {
            ++itl;
        }
        if (itl == l.end ()) {
            l.push_back (*this);
        }
        else {
            l.insert (std::next (itl), *this);
        }
    }*/
}

umlAttribute::~umlAttribute ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
