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

#include "parse_diagram.hpp"
#include "string2.hpp"

void
parseDiaString (const char * stringnode, std::string &buffer) {
    buffer.assign (stringnode, 1, strlen (stringnode) - 2);
}

void
parseDiaNode (xmlNodePtr stringnode, std::string &buffer) {
    xmlChar *content;

    content = xmlNodeGetContent (stringnode);
    buffer.assign (BAD_TSAC2 (content), 1, strlen (BAD_TSAC2 (content)) - 2);
    xmlFree (content);
}

bool
parseBoolean (xmlNodePtr booleannode) {
    xmlChar *val;
    bool result;

    val = xmlGetProp (booleannode, BAD_CAST2 ("val"));
    result = val != NULL && !strcmp (BAD_TSAC2 (val), "true");
    free (val);
    return result;
}

void
parseAttributes (xmlNodePtr node, std::list <umlAttribute> &retour) {
    while (node != NULL) {
        umlAttribute an;
        an.parse (node->xmlChildrenNode);
        an.insert (retour);
        node = node->next;
    }
}

void
parseTemplate (xmlNodePtr node, std::pair <std::string, std::string> &tmp) {
    parseDiaString (BAD_TSAC2 (
                              node->xmlChildrenNode->xmlChildrenNode->content),
                                 tmp.first);
    parseDiaString (BAD_TSAC2 (
                        node->next->xmlChildrenNode->xmlChildrenNode->content),
                                 tmp.second);
}

void
parseTemplates (xmlNodePtr node,
                std::list <std::pair <std::string, std::string>> &res) {
    while (node != NULL) {
        if (node->xmlChildrenNode->xmlChildrenNode->xmlChildrenNode != NULL &&
            node->xmlChildrenNode->next->xmlChildrenNode->xmlChildrenNode !=
                                                                       NULL ) {
            std::pair <std::string, std::string> tn;
            parseTemplate (node->xmlChildrenNode, tn);
            res.push_back (tn);
        }
        node = node->next;
    }
    return;
}

void
recursiveSearch (xmlNodePtr node, xmlNodePtr * object) {
    xmlNodePtr child;
    if (*object != NULL ) {
        return;
    }
    if (node != NULL ) {
        if (!strcmp (BAD_TSAC2 (node->name), "object") ) {
            *object = node;
            return;
        }
        child = node->xmlChildrenNode;
        while (child != NULL) {
            recursiveSearch (child, object);
            child = child->next;
        }
    }
}

/* Gets the next "object" node. Basically, gets from->next.  When
   it is null it checks for from->parent->next. */
xmlNodePtr
getNextObject (xmlNodePtr from) {
    xmlNodePtr next = NULL;
    if (from->next != NULL) {
        next = from->next;
        if (!strcmp (BAD_TSAC2 (next->name), "group")) {
            next = next->xmlChildrenNode;
            while (!strcmp (BAD_TSAC2 (next->name), "group")) {
                next = next->xmlChildrenNode;
            }
        }
        return next;
    }
    next = from->parent->next;
    if (next != NULL) {
        if (!strcmp (BAD_TSAC2 (next->name), "group")) {
            next = next->xmlChildrenNode;
            while (!strcmp (BAD_TSAC2 (next->name), "group")) {
                next = next->xmlChildrenNode;
            }
            return next;
        }
        if (!strcmp (BAD_TSAC2 (next->name), "layer")) {
            return next->xmlChildrenNode;
        }
        return next;
    }
    return NULL;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
