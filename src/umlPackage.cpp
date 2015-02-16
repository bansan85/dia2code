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

#include "parse_diagram.hpp"
#include "umlPackage.hpp"
#include "string2.hpp"

umlPackage::umlPackage () :
    id (),
    name (),
    geom ({0., 0., 0., 0.}),
    parent (nullptr),
    directory ()
{
}

umlPackage::umlPackage (xmlNodePtr package, std::string id_) :
    id (id_),
    name (),
    geom ({0., 0., 0., 0.}),
    parent (nullptr),
    directory ()
{
    xmlNodePtr attribute;

    attribute = package->xmlChildrenNode;
    while (attribute != NULL) {
        xmlChar *attrname;
        attrname = xmlGetProp (attribute, BAD_CAST2 ("name"));
        if (attrname != NULL) {
            if (!strcmp ("name", BAD_TSAC2 (attrname))) {
                parseDiaNode (attribute->xmlChildrenNode, name);
            } else if (!strcmp ("obj_pos", BAD_TSAC2 (attrname))) {
                parseGeomPosition (attribute->xmlChildrenNode, &geom);
            } else if (!strcmp ("elem_width", BAD_TSAC2 (attrname))) {
                parseGeomWidth (attribute->xmlChildrenNode, &geom);
            } else if (!strcmp ("elem_height", BAD_TSAC2 (attrname))) {
                parseGeomHeight (attribute->xmlChildrenNode, &geom);
            } else if (!strcmp ("stereotype", BAD_TSAC2 (attrname))) {
                std::string stereo;
                parseDiaNode (attribute->xmlChildrenNode, stereo);
                if (!stereo.empty ()) {
                    std::cerr << "Unknown stereotype: " << stereo << ".\n"
                              << "No stereotype is allowed for package.\n";
                }
            }
            xmlFree (attrname);
        }
        attribute = attribute->next;
    }
    return;
}

umlPackage::umlPackage (const umlPackage & pack) :
    id (pack.id),
    name (pack.name),
    geom (pack.geom),
    parent (pack.parent),
    directory (pack.directory) {
}

/*
    Builds a package list from the hierarchy of parents of package.
    The topmost package will be the first on the list and the initial
    package will be the last.
*/
void
umlPackage::makePackageList (umlPackage *package,
                             std::list <umlPackage *> &res) {
    umlPackage *par;
    
    if (package == NULL) {
        return;
    }
    res.push_back (package);
    par = package->parent;
    while (par != NULL) {
        res.push_front (par);
        par = par->parent;
    }
    return;
}

const std::string &
umlPackage::getId () const {
    return id;
}

const std::string &
umlPackage::getName () const {
    return name;
}

const geometry &
umlPackage::getGeometry () const {
    return geom;
}

umlPackage *
umlPackage::getParent () const {
    return parent;
}

void
umlPackage::setParent (umlPackage * pack) {
    parent = pack;
}

umlPackage *
umlPackage::find (std::list <umlPackage *> & packagelist, const char *id) {
    if (id != NULL) {
        for (umlPackage * it : packagelist) {
            if (it->getId ().compare (id) == 0) {
                return it;
            }
        }
    }
    return NULL;
}

umlPackage::~umlPackage () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
