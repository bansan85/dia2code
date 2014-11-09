/*
Copyright (C) 2000-2014 Javier O'Hara

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "parse_diagram.hpp"

#include <libxml/tree.h>

#define BAD_CAST2 reinterpret_cast <const xmlChar *>
#define BAD_TSAC2 reinterpret_cast <const char *>

#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

umlclasslist find(umlclasslist list, const char *id ) {
    if ( id != NULL ) {
        while ( list != NULL ) {
            if (list->key->id.compare (id) == 0) {
                return list;
            }
            list = list->next;
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
        result = 1;
    } else {
        result = 0;
    }
    free(val);
    return result;
}


void addparent(umlclass * key, umlclasslist derived) {
    umlclasslist tmp;
    tmp = new umlclassnode;
    tmp->key = key;
    tmp->parents = NULL;
    tmp->associations = NULL;
    tmp->dependencies = NULL;
    tmp->next = derived->parents;
    derived->parents = tmp;
}

void adddependency(umlclasslist dependent, umlclasslist dependee) {
    umlclasslist tmp;
    tmp = new umlclassnode;
    tmp->key = dependent->key;
    tmp->parents = NULL;
    tmp->associations = NULL;
    tmp->dependencies = NULL;
    tmp->next = dependee->dependencies;
    dependee->dependencies = tmp;
}

void addaggregate(const char *name, char composite, umlclasslist base,
                  umlclasslist associate, const char *multiplicity) {
    umlassoclist tmp;
    tmp = new umlassocnode;
    if (name != NULL && strlen (name) > 2)
        parse_dia_string(name, tmp->name);
    if (multiplicity != NULL)
        strncpy (tmp->multiplicity, multiplicity+1, strlen (multiplicity)-2);
    else
        sprintf(tmp->multiplicity, "1");
    tmp->key = base->key;
    tmp->composite = composite;
    tmp->next = associate->associations;
    associate->associations = tmp;
}

void inherit_realize ( umlclasslist classlist, const char * base, const char * derived ) {
    umlclasslist umlbase, umlderived;
    umlbase = find(classlist, base);
    umlderived = find(classlist, derived);
    if ( umlbase != NULL && umlderived != NULL ) {
        addparent(umlbase->key, umlderived);
    }
}

void associate ( umlclasslist classlist, const char * name, char composite,
                 const char * base, const char * aggregate, const char *multiplicity) {
    umlclasslist umlbase, umlaggregate;
    umlbase = find(classlist, base);
    umlaggregate = find(classlist, aggregate);
    if ( umlbase != NULL && umlaggregate != NULL) {
        addaggregate(name, composite, umlbase, umlaggregate, multiplicity);
    }
}

void make_depend ( umlclasslist classlist, const char * dependent, const char * dependee) {
    umlclasslist umldependent, umldependee;
    umldependent = find(classlist, dependent);
    umldependee = find(classlist, dependee);
    if ( umldependent != NULL && umldependee != NULL) {
        adddependency(umldependent, umldependee);
    }
}

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
umlattrlist insert_attribute(umlattrlist n, umlattrlist l) {
    if ( l != NULL ) {
        if ( l->key.visibility <= n->key.visibility ) {
            l->next = insert_attribute(n, l->next);
            return l;
        } else {
            n->next = l;
            return n;
        }
    } else {
        return n;
    }
}

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
umloplist insert_operation(umloplist n, umloplist l) {
    if ( l != NULL ) {
        if ( l->key.attr.visibility <= n->key.attr.visibility ) {
            l->next = insert_operation(n, l->next);
            return l;
        } else {
            n->next = l;
            return n;
        }
    } else {
        return n;
    }
}

umltemplatelist insert_template(umltemplatelist n, umltemplatelist l) {
    if ( l != NULL) {
        n->next = l;
        return n;
    } else {
        return n;
    }
}

void parse_attribute(xmlNodePtr node, umlattribute *tmp) {
    xmlChar *nodename;
    xmlChar *attrval;

    tmp->name.clear ();
    tmp->value.clear ();
    tmp->type.clear ();
    tmp->comment.clear ();
    tmp->visibility = '0';
    tmp->kind     = '0';
    while ( node != NULL ) {
        nodename = xmlGetProp(node, BAD_CAST2 ("name"));

        if ( !strcmp("name", BAD_TSAC2 (nodename)) ) {
            parse_dia_node(node->xmlChildrenNode, tmp->name);
        } else if ( !strcmp("value", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, tmp->value);
            }
        } else if ( !strcmp("type", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, tmp->type);
            } else {
                tmp->type.clear ();
            }
        } else if ( !strcmp("comment", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
               parse_dia_node(node->xmlChildrenNode, tmp->comment);
            } else {
               tmp->comment.clear ();
          }
        } else if ( !strcmp("kind", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(tmp->kind));
            free(attrval);
        } else if ( !strcmp("visibility", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(tmp->visibility));
            free(attrval);
        } else if ( !strcmp("abstract", BAD_TSAC2 (nodename))) {
            tmp->isabstract = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("class_scope", BAD_TSAC2 (nodename))) {
            tmp->isstatic = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("query", BAD_TSAC2 (nodename))) {
            tmp->isconstant = parse_boolean(node->xmlChildrenNode);
        }

        free(nodename);
        node = node->next;
    }
}

umlattrlist parse_attributes(xmlNodePtr node) {
    umlattrlist list = NULL, an;
    while ( node != NULL ) {
        an = new umlattrnode;
        an->next = NULL;
        parse_attribute(node->xmlChildrenNode, &(an->key));
        list = insert_attribute(an, list);
        node = node->next;
    }
    return list;
}

void parse_operation(xmlNodePtr node, umloperation *tmp) {
    xmlChar *nodename;
    parse_attribute(node, &(tmp->attr));
    while ( node != NULL ) {
        nodename = xmlGetProp(node, BAD_CAST2 ("name"));
        if ( !strcmp("parameters", BAD_TSAC2 (nodename)) ) {
            tmp->parameters = parse_attributes(node->xmlChildrenNode);
        }
        free(nodename);
        node = node->next;
    }
}

umloplist parse_operations(xmlNodePtr node) {
    umloplist list = NULL, on;
    while ( node != NULL ) {
        on = new umlopnode;
        on->next = NULL;
        on->key.implementation.clear ();
        parse_operation(node->xmlChildrenNode, &(on->key));
        list = insert_operation(on, list);
        node = node->next;
    }
    return list;
}

void parse_template(xmlNodePtr node, umltemplate *tmp) {
    parse_dia_string(BAD_TSAC2 (node->xmlChildrenNode->xmlChildrenNode->content), tmp->name);
    parse_dia_string(BAD_TSAC2 (node->next->xmlChildrenNode->xmlChildrenNode->content), tmp->type);
}

umltemplatelist parse_templates(xmlNodePtr node) {
    umltemplatelist list = NULL, tn;
    while ( node != NULL) {
        if ( node->xmlChildrenNode->xmlChildrenNode->xmlChildrenNode != NULL &&
                node->xmlChildrenNode->next->xmlChildrenNode->xmlChildrenNode != NULL ) {
            tn = new umltemplatenode;
            tn->next = NULL;
            parse_template(node->xmlChildrenNode, &(tn->key));
            list = insert_template(tn, list);
        }
        node = node->next;
    }
    return list;
}

/**
  * Adds get() (or is()) and set() methods for each attribute
  * myself MUST be != null
*/
void make_javabean_methods(umlclass *myself) {
    std::string tmpname;
    umloplist operation;
    umlattrlist attrlist, parameter;

    attrlist = myself->attributes;
    while (attrlist != NULL) {
        if ( ! attrlist->key.isabstract) {
            /* The SET method */
            operation = new umlopnode;
            parameter = new umlattrnode;

            parameter->key.name.assign ("value");
            parameter->key.type.assign (attrlist->key.type);
            parameter->key.value.clear ();
            parameter->key.isstatic = 0;
            parameter->key.isconstant = 0;
            parameter->key.isabstract = 0;
            parameter->key.visibility = '0';
            parameter->next = NULL;
            operation->key.parameters = parameter;

            operation->key.implementation.clear ();
            operation->key.implementation.append ("    ");
            operation->key.implementation.append (attrlist->key.name);
            operation->key.implementation.append (" = value;");

            tmpname = strtoupperfirst(attrlist->key.name);
            operation->key.attr.name.append ("set");
            operation->key.attr.name.append (tmpname);
            operation->key.attr.isabstract = 0;
            operation->key.attr.isstatic = 0;
            operation->key.attr.isconstant = 0;
            operation->key.attr.visibility = '0';
            operation->key.attr.value.clear ();
            operation->key.attr.type.assign ("void");
            operation->next = NULL;

            myself->operations = insert_operation(operation, myself->operations);

            /* The GET or IS method */
            operation = new umlopnode;
            operation->key.parameters = NULL;
            tmpname = strtoupperfirst(attrlist->key.name);
            if ( attrlist->key.type.compare ("boolean") == 0) {
                operation->key.attr.name.assign ("is");
            } else {
                operation->key.attr.name.assign ("get");
            }
            operation->key.attr.name.append (tmpname);

            operation->key.implementation.assign ("    return ");
            operation->key.implementation.append (attrlist->key.name);
            operation->key.implementation.append (";");

            operation->key.attr.isabstract = 0;
            operation->key.attr.isstatic = 0;
            operation->key.attr.isconstant = 0;
            operation->key.attr.visibility = '0';
            operation->key.attr.value.clear ();
            operation->key.attr.type.assign (attrlist->key.type);
            operation->next = NULL;

            myself->operations = insert_operation(operation, myself->operations);
        }
        attrlist = attrlist->next;
    }
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


/*
   returns 1 if the position point of the object with geom2 is inside the object with geom1
           0 otherwise
*/
int is_inside(const geometry * geom1, const geometry * geom2) {
    return geom1->pos_x < geom2->pos_x && geom2->pos_x < (geom1->pos_x+geom1->width) &&
           geom1->pos_y < geom2->pos_y && geom2->pos_y < (geom1->pos_y+geom1->height);

}

umlpackagelist parse_package(xmlNodePtr package) {
    xmlNodePtr attribute;
    umlpackagelist listmyself;
    umlpackage *myself;
    xmlChar *attrname;

    listmyself = new umlpackagenode;
    myself = new umlpackage;

    myself->parent = NULL;

    listmyself->next = NULL;
    listmyself->key = myself;

    attribute = package->xmlChildrenNode;
    while ( attribute != NULL ) {
        attrname = xmlGetProp(attribute, BAD_CAST2 ("name"));
        /* fix a segfault - dia files contains *also* some rare tags without any "name" attribute : <dia:parent  for ex.  */
        if( attrname != NULL ) {
            if ( !strcmp("name", BAD_TSAC2 (attrname)) ) {
                parse_dia_node(attribute->xmlChildrenNode, myself->name);
                //debug( 4, "name is %s \n", myself->name );
            } else if ( !strcmp ( "obj_pos", BAD_TSAC2 (attrname) ) ) {
                parse_geom_position(attribute->xmlChildrenNode, &myself->geom );
            } else if ( !strcmp ( "elem_width", BAD_TSAC2 (attrname) ) ) {
                parse_geom_width(attribute->xmlChildrenNode, &myself->geom );
            } else if ( !strcmp ( "elem_height", BAD_TSAC2 (attrname) ) ) {
                parse_geom_height(attribute->xmlChildrenNode, &myself->geom );
            }
            xmlFree (attrname);
        }
        attribute = attribute->next;
    }
    return listmyself;
}

umlclasslist parse_class(xmlNodePtr class_) {
    xmlNodePtr attribute;
    xmlChar *attrname;
    umlclasslist listmyself;
    umlclass *myself;

    listmyself = new umlclassnode;
    myself = new umlclass;
    myself->package = NULL;

    listmyself->key = myself;
    listmyself->parents = NULL;
    listmyself->associations = NULL;
    listmyself->dependencies = NULL;
    listmyself->next = NULL;

    attribute = class_->xmlChildrenNode;
    while ( attribute != NULL ) {
        attrname = xmlGetProp(attribute, BAD_CAST2 ("name"));
        /* fix a segfault - dia files contains *also* some rare tags without any "name" attribute : <dia:parent  for ex.  */
        if( attrname == NULL ) {
            attribute = attribute->next;
            continue;
        }
        if ( !strcmp("name", BAD_TSAC2 (attrname)) ) {
            parse_dia_node(attribute->xmlChildrenNode, myself->name);
        } else if ( !strcmp ( "obj_pos", BAD_TSAC2 (attrname) ) ) {
            parse_geom_position(attribute->xmlChildrenNode, &myself->geom );
        } else if ( !strcmp ( "elem_width", BAD_TSAC2 (attrname) ) ) {
            parse_geom_width(attribute->xmlChildrenNode, &myself->geom );
        } else if ( !strcmp ( "elem_height", BAD_TSAC2 (attrname )) ) {
            parse_geom_height(attribute->xmlChildrenNode, &myself->geom );
        } else if ( !strcmp("comment", BAD_TSAC2 (attrname)))  {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
               parse_dia_node(attribute->xmlChildrenNode, myself->comment);
            }  else {
               myself->comment.clear ();
            }
        } else if ( !strcmp("stereotype", BAD_TSAC2 (attrname)) ) {
            if ( attribute->xmlChildrenNode->xmlChildrenNode != NULL ) {
                parse_dia_node(attribute->xmlChildrenNode, myself->stereotype);
            } else {
                myself->stereotype.clear ();
            }
        } else if ( !strcmp("abstract", BAD_TSAC2 (attrname)) ) {
            myself->isabstract = parse_boolean(attribute->xmlChildrenNode);
        } else if ( !strcmp("attributes", BAD_TSAC2 (attrname)) ) {
            myself->attributes = parse_attributes(attribute->xmlChildrenNode);
        } else if ( !strcmp("operations", BAD_TSAC2 (attrname)) ) {
            myself->operations = parse_operations(attribute->xmlChildrenNode);
            if ( myself->stereotype.compare ("JavaBean") == 0) {
                /* Javabean: we should now add a get() and set() methods
                for each attribute */
                make_javabean_methods(myself);
            }
        } else if ( !strcmp("templates", BAD_TSAC2 (attrname)) ) {
            myself->templates = parse_templates(attribute->xmlChildrenNode);
        }
        free(attrname);
        attribute = attribute->next;
    }
    return listmyself;
}

/**
  Simple, non-compromising, implementation declaration.
  This function creates a plain vanilla interface (an
  umlclasslist) and associates it to the implementator.
  The implementator's code should contain the interface
  name, but the interface itself will not be inserted
  into the classlist, so no code can be generated for it.
*/
void lolipop_implementation(umlclasslist classlist, xmlNodePtr object) {
    xmlNodePtr attribute;
    xmlChar *id = NULL;
    const char *name = NULL;
    xmlChar *attrname;
    umlclasslist implementator;

    attribute = object->xmlChildrenNode;
    while ( attribute != NULL ) {
        if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
            id = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
        } else {
            attrname = xmlGetProp(attribute, BAD_CAST2 ("name"));
            if ( attrname != NULL && !strcmp("text", BAD_TSAC2 (attrname)) && attribute->xmlChildrenNode != NULL &&
                    attribute->xmlChildrenNode->xmlChildrenNode != NULL ){
                name = BAD_TSAC2 (attribute->xmlChildrenNode->xmlChildrenNode->content);
            } else {
                name = "";
            }
            free(attrname);
        }
        attribute = attribute->next;
    }
    implementator = find(classlist, BAD_TSAC2 (id));
    free(id);
    if (implementator != NULL && name != NULL && strlen(name) > 2) {
        umlclass * key = new umlclass;
        key->id.assign ("00");
        parse_dia_string (name, key->name);
        key->stereotype.assign ("Interface");
        key->isabstract = 1;
        key->attributes = NULL;
        key->operations = NULL;
        addparent(key, implementator);
    }
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
   it is null it checks for from->parent->next.
   FIXME: the code is ugly */
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

umlclasslist parse_diagram(char *diafile) {
    xmlDocPtr ptr;
    xmlChar *end1 = NULL;
    xmlChar *end2 = NULL;

    xmlNodePtr object = NULL;
    umlclasslist classlist = NULL, endlist = NULL;
    umlpackagelist packagelist = NULL, dummypcklist, endpcklist = NULL;

    ptr = xmlParseFile(diafile);

    if (ptr == NULL) {
        fprintf(stderr, "That file does not exist or is not a Dia diagram\n");
        exit(2);
    }

    // we search for the first "object" node
    recursive_search( ptr->xmlRootNode->xmlChildrenNode->next, &object );

    while (object != NULL) {
        xmlChar *objtype = xmlGetProp(object, BAD_CAST2 ("type"));
        // Here we have a Dia object 
        if (strcmp("UML - Class", BAD_TSAC2 (objtype)) == 0) {
            // Here we have a class definition
            umlclasslist tmplist = parse_class(object);
            // We get the ID of the object here
            xmlChar *objid = xmlGetProp(object, BAD_CAST2 ("id"));
            tmplist->key->id.assign (BAD_TSAC2 (objid));
            free(objid);

            // We insert it here
            if ( classlist == NULL ) {
                classlist = endlist = tmplist;
            } else {
                endlist->next = tmplist;
                endlist = tmplist;
            }
        } else if ( !strcmp("UML - LargePackage", BAD_TSAC2 (objtype)) || !strcmp("UML - SmallPackage", BAD_TSAC2 (objtype)) ) {
            umlpackagelist tmppcklist = parse_package(object);
            if ( tmppcklist != NULL ) {
                // We get the ID of the object here
                xmlChar *objid = xmlGetProp(object, BAD_CAST2 ("id"));
                tmppcklist->key->id.assign (BAD_TSAC2 (objid));
                free(objid);
            }
            // We insert it here
            if ( packagelist == NULL ) {
                packagelist = endpcklist = tmppcklist;
            } else {
                endpcklist->next = tmppcklist;
                endpcklist = tmppcklist;
            }

        }
        free(objtype);
        object = getNextObject(object);
    }

    /* Second pass - Implementations and associations */

    /* The association is done as a queue, so we must first put in
         realizations (interfaces) and then generalizations (inheritance)
         so we will have the latter first and the former after (!)
         THIS STILL SUCKS !!! How soon is now? */

    /* we search for the first "object" node */
    recursive_search( ptr->xmlRootNode->xmlChildrenNode->next, &object );

    while ( object != NULL ) {
        xmlChar *objtype = xmlGetProp(object, BAD_CAST2 ("type"));
        if ( !strcmp("UML - Association", BAD_TSAC2 (objtype)) ) {
            const char *name = NULL;
            const char *name_a = NULL;
            const char *name_b = NULL;
            const char *multiplicity_a = NULL;
            const char *multiplicity_b = NULL;
            char direction = 0;
            char composite = 0;
            xmlNodePtr attribute = object->xmlChildrenNode;

            while (attribute != NULL) {
                char *attrtype = reinterpret_cast <char *> (xmlGetProp(attribute, BAD_CAST2  ("name")));

                if (attrtype != NULL) {
                    xmlNodePtr child = attribute->xmlChildrenNode;
                    if ( !strcmp("direction", attrtype) ) {
                        xmlChar *tmptype = xmlGetProp(child, BAD_CAST2 ("val"));
                        if ( !strcmp(BAD_TSAC2 (tmptype), "0") ) {
                            direction = 1;
                        }
                        else {
                            direction = 0;
                        }
                        free(tmptype);
                    }
                    else if ( !strcmp("assoc_type", attrtype) ) {
                        xmlChar *tmptype = xmlGetProp(child, BAD_CAST2 ("val"));
                        if ( !strcmp(BAD_TSAC2 (tmptype), "1") ) {
                            composite = 0;
                        }
                        else {
                            composite = 1;
                        }
                        free(tmptype);
                    }
                    else if ( child->xmlChildrenNode ) {
                        xmlNodePtr grandchild = child->xmlChildrenNode;
                        if ( !strcmp(attrtype, "name") ) {
                            name = BAD_TSAC2 (grandchild->content);
                        }
                        else if ( !strcmp(attrtype, "role_a") ) {
                            name_a = BAD_TSAC2 (grandchild->content);
                        }
                        else if ( !strcmp(attrtype, "role_b") ) {
                            name_b = BAD_TSAC2 (grandchild->content);
                        }
                        else if ( !strcmp(attrtype, "multipicity_a") ) {
                            multiplicity_a = BAD_TSAC2 (grandchild->content);
                        }
                        else if ( !strcmp(attrtype, "multipicity_b") ) {
                            multiplicity_b = BAD_TSAC2 (grandchild->content);
                        } 
                        else if ( !strcmp(attrtype, "ends") ) {
                            if ( !strcmp(BAD_TSAC2 (child->name), "composite") ) {
                                while (grandchild) {
                                    xmlNodePtr ggchild = grandchild->xmlChildrenNode;
                                    if (ggchild->xmlChildrenNode) {
                                        attrtype = reinterpret_cast <char *> (xmlGetProp(grandchild, BAD_CAST2 ("name")));
                                        if ( !strcmp(attrtype, "role") ) {
                                            name_a = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if ( !strcmp(attrtype, "multiplicity") ) {
                                            multiplicity_a = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if ( !strcmp(attrtype, "aggregate") ) {
                                            /* todo */
                                        }
                                    }
                                    grandchild = grandchild->next;
                                }
                            }
                            child = child->next;
                            if ( child != NULL && !strcmp(BAD_TSAC2 (child->name), "composite") ) {
                                grandchild = child->xmlChildrenNode;
                                while (grandchild) {
                                    xmlNodePtr ggchild = grandchild->xmlChildrenNode;
                                    if (ggchild->xmlChildrenNode) {
                                        attrtype = reinterpret_cast <char *> (xmlGetProp(grandchild, BAD_CAST2 ("name")));
                                        if ( !strcmp(attrtype, "role") ) {
                                            name_b = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if ( !strcmp(attrtype, "multiplicity") ) {
                                            multiplicity_b = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if ( !strcmp(attrtype, "aggregate") ) {
                                            /* todo */
                                        }
                                    }
                                    grandchild = grandchild->next;
                                }
                            }
                        }
                    }
                    free(attrtype);
                }
                else if ( !strcmp(BAD_TSAC2 (attribute->name), "connections") ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                }

                attribute = attribute->next;
            }

            if (end1 != NULL && end2 != NULL) {
                const char *thisname = name;
                if (direction == 1) {
                    if (thisname == NULL || !*thisname || !strcmp("##", thisname))
                        thisname = name_a;
                    associate(classlist, thisname, composite, BAD_TSAC2 (end1), BAD_TSAC2 (end2), multiplicity_a);
                } else {
                    if (thisname == NULL || !*thisname || !strcmp("##", thisname))
                        thisname = name_b;
                    associate(classlist, thisname, composite, BAD_TSAC2 (end2), BAD_TSAC2(end1), multiplicity_b);
                }
                free(end1);
                free(end2);
            }

        } else if ( !strcmp("UML - Dependency", BAD_TSAC2 (objtype)) ) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while ( attribute != NULL ) {
                if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                    make_depend(classlist, BAD_TSAC2 (end1), BAD_TSAC2 (end2));
                    free(end1);
                    free(end2);
                }
                attribute = attribute->next;
            }
        } else if ( !strcmp("UML - Realizes", BAD_TSAC2 (objtype)) ) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while ( attribute != NULL ) {
                if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                    inherit_realize(classlist, BAD_TSAC2 (end1), BAD_TSAC2(end2));
                    free(end2);
                    free(end1);
                }
                attribute = attribute->next;
            }
        } else if ( !strcmp("UML - Implements", BAD_TSAC2 (objtype)) ) {
            lolipop_implementation(classlist, object);
        }
        free(objtype);
        object = getNextObject(object);
    }


    /* Generalizations: we must put this AFTER all the interface
       implementations. generate_code_java relies on this. */
    recursive_search( ptr->xmlRootNode->xmlChildrenNode->next, &object );
    while ( object != NULL ) {
        xmlChar *objtype = xmlGetProp(object, BAD_CAST2 ("type"));
        if ( !strcmp("UML - Generalization", BAD_TSAC2 (objtype)) ) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while ( attribute != NULL ) {
                if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                    inherit_realize(classlist, BAD_TSAC2 (end1), BAD_TSAC2(end2));
                    free(end2);
                    free(end1);
                }
                attribute = attribute->next;
            }
        }
        free(objtype);
        object = getNextObject(object);
    }

    /* Packages: we should scan the packagelist and then the classlist.
       Scanning the packagelist we'll build all relationships between
       packages.  Scanning the classlist we'll associate its own package
       to each class.

       FIXME: maybe we can do both in the same pass */

    /* Build the relationships between packages */
    dummypcklist = packagelist;
    while ( dummypcklist != NULL ) {
        umlpackagelist tmppcklist = packagelist;
        while ( tmppcklist != NULL ) {
            if ( is_inside(&dummypcklist->key->geom, &tmppcklist->key->geom) ) {
                if ( tmppcklist->key->parent == NULL ) {
                    tmppcklist->key->parent = dummypcklist->key;
                } else {
                    if ( ! is_inside ( &dummypcklist->key->geom, &tmppcklist->key->parent->geom ) ) {
                        tmppcklist->key->parent = dummypcklist->key;
                    }
                }
            }
            tmppcklist = tmppcklist->next;
        }
        dummypcklist = dummypcklist->next;
    }

    /* Associate packages to classes */
    dummypcklist = packagelist;
    while ( dummypcklist != NULL ) {
        umlclasslist tmplist = classlist;
        while ( tmplist != NULL ) {
            if ( is_inside(&dummypcklist->key->geom,&tmplist->key->geom) ) {
                if ( tmplist->key->package == NULL ) {
                    tmplist->key->package = dummypcklist->key;
                } else {
                    if ( ! is_inside ( &dummypcklist->key->geom, &tmplist->key->package->geom ) ) {
                        tmplist->key->package = dummypcklist->key;
                    }
                }
            }
            tmplist = tmplist->next;
        }
        dummypcklist = dummypcklist->next;
    }
    
    while (packagelist != NULL)
    {
        endpcklist = packagelist->next;
        delete packagelist;
        packagelist = endpcklist;
    }


    xmlFreeDoc (ptr);
    
    return classlist;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
