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

#include "dia2code.hpp"

#include "parse_diagram.hpp"
#include "umlAttribute.hpp"

umlAttribute::umlAttribute () :
    name (),
    value (),
    type (),
    comment (),
    visibility ('0'),
    isabstract (false),
    isstatic (false),
    isconstant (false),
    kind ('1')
{
}

umlAttribute::umlAttribute (std::string name_,
                            std::string value_,
                            std::string type_,
                            std::string comment_,
                            char visibility_,
                            unsigned char isabstract_,
                            unsigned char isstatic_,
                            unsigned char isconstant_,
                            char kind_) :
    name (name_),
    value (value_),
    type (type_),
    comment (comment_),
    visibility (visibility_),
    isabstract (isabstract_),
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

char
umlAttribute::getVisibility () const
{
    return visibility;
}

unsigned char
umlAttribute::isAbstract () const
{
    return isabstract;
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

char
umlAttribute::getKind () const
{
    return kind;
}

void
umlAttribute::assign (std::string name_,
                      std::string value_,
                      std::string type_,
                      std::string comment_,
                      char visibility_,
                      unsigned char isabstract_,
                      unsigned char isstatic_,
                      unsigned char isconstant_,
                      char kind_)
{
    name = name_;
    value = value_;
    type = type_;
    comment = comment_;
    visibility = visibility_;
    isabstract = isabstract_;
    isstatic = isstatic_;
    isconstant = isconstant_;
    kind = kind_;
}

void
umlAttribute::check (const char *typename_)
{
    /* Check settings that don't make sense for C++ generation.  */
    if (visibility == '1')
        fprintf (stderr, "%s/%s: ignoring non-visibility\n", typename_, name.c_str ());
    if (isstatic)
        fprintf (stderr, "%s/%s: ignoring staticness\n", typename_, name.c_str ());
}

void
umlAttribute::parse(xmlNodePtr node) {
    xmlChar *nodename;
    xmlChar *attrval;

    name.clear ();
    value.clear ();
    type.clear ();
    comment.clear ();
    visibility = '0';
    kind     = '0';
    while ( node != NULL ) {
        nodename = xmlGetProp(node, BAD_CAST2 ("name"));

        if ( !strcmp("name", BAD_TSAC2 (nodename)) ) {
            parse_dia_node(node->xmlChildrenNode, name);
        } else if ( !strcmp("value", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, value);
            }
        } else if ( !strcmp("type", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, type);
            } else {
                type.clear ();
            }
        } else if ( !strcmp("comment", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
               parse_dia_node(node->xmlChildrenNode, comment);
            } else {
               comment.clear ();
          }
        } else if ( !strcmp("kind", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(kind));
            free(attrval);
        } else if ( !strcmp("visibility", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(visibility));
            free(attrval);
        } else if ( !strcmp("abstract", BAD_TSAC2 (nodename))) {
            isabstract = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("class_scope", BAD_TSAC2 (nodename))) {
            isstatic = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("query", BAD_TSAC2 (nodename))) {
            isconstant = parse_boolean(node->xmlChildrenNode);
        }

        free(nodename);
        node = node->next;
    }
}

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void
umlAttribute::insert(std::list <umlAttribute> &l) {
    std::list <umlAttribute>::iterator itl;
    
    itl = l.begin ();
    
    if (itl == l.end ()) {
        l.push_back (*this);
    }
    else {
        while ((itl != l.end ()) && ((*itl).visibility >= this->visibility)) {
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
