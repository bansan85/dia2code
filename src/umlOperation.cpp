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

#include "umlOperation.hpp"
#include "parse_diagram.hpp"
#include "string2.hpp"

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void
umlOperation::insertOperation (umlOperation &n, std::list <umlOperation> &l) {
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
umlOperation::parseOperations (xmlNodePtr node,
                                std::list <umlOperation> &res) {
    while (node != NULL) {
        umlOperation on (node->xmlChildrenNode);
        insertOperation (on, res);
        node = node->next;
    }
    return;
}

umlOperation::umlOperation (xmlNodePtr node) :
    umlAttribute (),
    stereotypeDelete (false),
    stereotypeGetSet (false),
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
            std::string stereo;
            parseDiaNode (node->xmlChildrenNode, stereo);
            if (isInside (stereo, "delete")) {
                stereotypeDelete = true;
            }
            else if (isInside (stereo, "GetSet")) {
                stereotypeGetSet = true;
            }
            else if (!stereo.empty ()) {
                std::cerr << "Unknown stereotype: " << stereo << ".\n"
                          << "Allow stereotypes are: "
                          << "\"delete\", \"GetSet\".\n";
            }
        }
        free (nodename);
        node = node->next;
    }
}

umlOperation::umlOperation (std::string name_,
                            std::string type_,
                            std::string comment_,
                            Visibility visibility_,
                            Inheritance inheritance_,
                            bool isstatic_,
                            bool isconstant_,
                            bool stereotypeDelete_,
                            bool stereotypeGetSet_) :
    umlAttribute (name_,
                  "",
                  type_,
                  comment_,
                  visibility_,
                  inheritance_,
                  isstatic_,
                  isconstant_,
                  Kind::UNKNOWN),
    stereotypeDelete (stereotypeDelete_),
    stereotypeGetSet (stereotypeGetSet_),
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

bool
umlOperation::isStereotypeDelete () const {
    return stereotypeDelete;
}

bool
umlOperation::isStereotypeGetSet () const {
    return stereotypeGetSet;
}

umlOperation::~umlOperation ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
