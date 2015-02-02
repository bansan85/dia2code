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
                            unsigned char isstatic_,
                            unsigned char isconstant_,
                            Kind kind_) :
    name (name_),
    value (value_),
    type (type_),
    comment (comment_),
    visibility (visibility_),
    inheritance (inheritance_),
    isstatic (isstatic_ & 1),
    isconstant (isconstant_ & 1),
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

Inheritance
umlAttribute::getInheritance () const
{
    return inheritance;
}

unsigned char
umlAttribute::isStatic () const
{
    return isstatic;
}

unsigned char
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
                      unsigned char isstatic_,
                      unsigned char isconstant_,
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
umlAttribute::check (const char *typename_) const
{
    /* Check settings that don't make sense for C++ generation.  */
    if (visibility == Visibility::PRIVATE) {
        fprintf (stderr,
                 "%s/%s: ignoring non-visibility\n",
                 typename_,
                 name.c_str ());
    }
    if (isstatic) {
        fprintf (stderr,
                 "%s/%s: ignoring staticness\n",
                 typename_,
                 name.c_str ());
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

        if (!strcmp ("name", BAD_TSAC2 (nodename))) {
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
                    free (attrval);
                    throw std::string (std::string ("Unknown kind: ") +
                                       std::string (1, tmp));
                }
            }
            free (attrval);
        } else if (!strcmp ("visibility", BAD_TSAC2 (nodename))) {
            char tmp;
            attrval = xmlGetProp (node->xmlChildrenNode, BAD_CAST2 ("val"));
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
                    free (attrval);
                    throw std::string (std::string ("Unknown visibility: ") +
                                       std::string (1, tmp));
                }
            }
            free (attrval);
        } else if (!strcmp ("inheritance_type", BAD_TSAC2 (nodename))) {
            char inheritance_tmp;
            attrval = xmlGetProp (node->xmlChildrenNode, BAD_CAST2 ("val"));
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
                    free (attrval);
                    throw std::string (std::string ("Unknown inheritance : ") +
                                       std::string (1, inheritance_tmp));
                }
            }
            free (attrval);
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
    std::list <umlAttribute>::iterator itl;
    
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
    }
}

umlAttribute::~umlAttribute ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
