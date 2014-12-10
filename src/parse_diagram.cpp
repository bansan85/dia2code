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

#include <libxml/tree.h>

#include "parse_diagram.hpp"
#include "umlOperation.hpp"
#include "umlClassNode.hpp"

#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

umlClassNode * find(std::list <umlClassNode> & list, const char *id ) {
    if ( id != NULL ) {
        for (umlClassNode & it : list) {
            if (it.getId ().compare (id) == 0) {
                return &it;
            }
        }
    }
    return NULL;
}

void parse_dia_string(const char * stringnode, std::string &buffer) {
    buffer.assign (stringnode, 1, strlen(stringnode) - 2);
}

void parse_dia_node(xmlNodePtr stringnode, std::string &buffer) {
    xmlChar *content;

    content = xmlNodeGetContent(stringnode);
    buffer.assign (BAD_TSAC2 (content), 1, strlen(BAD_TSAC2 (content)) - 2);
    xmlFree (content);
}

bool parse_boolean(xmlNodePtr booleannode) {
    xmlChar *val;
    int result;

    val = xmlGetProp(booleannode, BAD_CAST2 ("val"));
    if ( val != NULL && !strcmp(BAD_TSAC2 (val), "true")) {
        result = true;
    } else {
        result = false;
    }
    free(val);
    return result;
}


/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void insert_operation(umlOperation &n, std::list <umlOperation> &l) {
    std::list <umlOperation>::iterator itl;
    
    itl = l.begin ();
    
    if (itl == l.end ()) {
        l.push_back (n);
    }
    else {
        while ((itl != l.end ()) && ((*itl).getVisibility () >= n.getVisibility ())) {
            ++itl;
        }
        if (itl == l.end ()) {
            l.push_back (n);
        }
        else {
            l.insert (std::next (itl), n);
        }
    }
}

void insert_template(std::pair <std::string, std::string> &n, std::list <std::pair <std::string, std::string> > &l) {
    l.push_back (n);
}


void parse_attributes(xmlNodePtr node, std::list <umlAttribute> &retour) {
    while ( node != NULL ) {
        umlAttribute an;
        an.parse(node->xmlChildrenNode);
        an.insert(retour);
        node = node->next;
    }
}

void parse_operations(xmlNodePtr node, std::list <umlOperation> &res) {
    while ( node != NULL ) {
        umlOperation on (node->xmlChildrenNode);
        insert_operation(on, res);
        node = node->next;
    }
    return;
}

void parse_template(xmlNodePtr node, std::pair <std::string, std::string> &tmp) {
    parse_dia_string(BAD_TSAC2 (node->xmlChildrenNode->xmlChildrenNode->content), tmp.first);
    parse_dia_string(BAD_TSAC2 (node->next->xmlChildrenNode->xmlChildrenNode->content), tmp.second);
}

void parse_templates(xmlNodePtr node, std::list <std::pair <std::string, std::string>> &res) {
    while ( node != NULL) {
        if ( node->xmlChildrenNode->xmlChildrenNode->xmlChildrenNode != NULL &&
                node->xmlChildrenNode->next->xmlChildrenNode->xmlChildrenNode != NULL ) {
            std::pair <std::string, std::string> tn;
            parse_template(node->xmlChildrenNode, tn);
            insert_template(tn, res);
        }
        node = node->next;
    }
    return;
}

void parse_geom_position(xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    char * token;
    val = xmlGetProp(attribute, BAD_CAST2 ("val"));
    token = strtok(reinterpret_cast <char *> (val),",");
    sscanf ( token, "%f", &(geom->pos_x) );
    token = strtok(NULL,",");
    sscanf ( token, "%f", &(geom->pos_y) );
    xmlFree (val);
}

void parse_geom_width(xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp(attribute, BAD_CAST2 ("val"));
    sscanf ( BAD_TSAC2 (val), "%f", &(geom->width) );
    xmlFree (val);
}

void parse_geom_height(xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp(attribute, BAD_CAST2 ("val"));
    sscanf ( BAD_TSAC2 (val), "%f", &(geom->height) );
    xmlFree (val);
}



void recursive_search(xmlNodePtr node, xmlNodePtr * object) {
    xmlNodePtr child;
    if ( *object != NULL ) {
        return;
    }
    if ( node != NULL ) {
        if ( !strcmp(BAD_TSAC2 (node->name),"object") ){
            *object = node;
            return;
        }
        child = node->xmlChildrenNode;
        while ( child != NULL ) {
            recursive_search(child,object);
            child=child->next;
        }
    }
}

/* Gets the next "object" node. Basically, gets from->next.  When
   it is null it checks for from->parent->next. */
xmlNodePtr getNextObject(xmlNodePtr from) {
    xmlNodePtr next = NULL;
    if ( from->next != NULL ){
        next = from->next;
        if ( !strcmp(BAD_TSAC2 (next->name),"group") ){
            next = next->xmlChildrenNode;
            while ( !strcmp(BAD_TSAC2 (next->name),"group") ){
                next=next->xmlChildrenNode;
            }
        }
        return next;
    }
    next = from->parent->next;
    if ( next != NULL ){
        if ( !strcmp(BAD_TSAC2 (next->name),"group") ){
            next = next->xmlChildrenNode;
            while ( !strcmp(BAD_TSAC2 (next->name),"group") ){
                next=next->xmlChildrenNode;
            }
            return next;
        }
        if ( !strcmp(BAD_TSAC2 (next->name), "layer") ){
            return next->xmlChildrenNode;
        }
        return next;
    }
    return NULL;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
