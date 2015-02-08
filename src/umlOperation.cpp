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

#include "umlOperation.hpp"
#include "parse_diagram.hpp"
#include "string2.hpp"

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void
umlOperation::insert_operation (umlOperation &n, std::list <umlOperation> &l) {
    l.push_back (n);
/*    std::list <umlOperation>::iterator itl;

    itl = l.begin ();

    if (itl == l.end ()) {
        l.push_back (n);
    }
    else {
        while ((itl != l.end ()) &&
               ((*itl).getVisibility () >= n.getVisibility ())) {
            ++itl;
        }
        if (itl == l.end ()) {
            l.push_back (n);
        }
        else {
            l.insert (std::next (itl), n);
        }
    }*/
}

void
umlOperation::parse_operations (xmlNodePtr node,
                                std::list <umlOperation> &res) {
    while (node != NULL) {
        umlOperation on (node->xmlChildrenNode);
        insert_operation (on, res);
        node = node->next;
    }
    return;
}

umlOperation::umlOperation (xmlNodePtr node) :
    umlAttribute (),
    stereotype (),
    parameters ()
{
    parse (node);
    while (node != NULL) {
        xmlChar *nodename;
        nodename = xmlGetProp (node, BAD_CAST2 ("name"));
        if (!strcmp ("parameters", BAD_TSAC2 (nodename))) {
            parseAttributes (node->xmlChildrenNode, parameters);
        }
        else if (!strcmp ("stereotype", BAD_TSAC2 (nodename))) {
            parseDiaNode (node->xmlChildrenNode, stereotype);
        }
        free (nodename);
        node = node->next;
    }
}

umlOperation::umlOperation (std::string name_,
                            std::string value_,
                            std::string type_,
                            std::string comment_,
                            Visibility visibility_,
                            Inheritance inheritance_,
                            unsigned char isstatic_,
                            unsigned char isconstant_,
                            Kind kind_,
                            std::string stereotype_) :
    umlAttribute (name_,
                  value_,
                  type_,
                  comment_,
                  visibility_,
                  inheritance_,
                  isstatic_,
                  isconstant_,
                  kind_),
    stereotype (stereotype_),
    parameters ()
{
}

void
umlOperation::addParameter (umlAttribute & attr) {
    parameters.push_back (attr);
}

const std::list <umlAttribute> &
umlOperation::getParameters () const {
    return parameters;
}

const std::string &
umlOperation::getStereotype () const {
    return stereotype;
}

umlOperation::~umlOperation ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
