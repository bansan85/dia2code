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

#include <libxml/tree.h>

#include "parse_diagram.hpp"

#define BAD_CAST2 reinterpret_cast <const xmlChar *>
#define BAD_TSAC2 reinterpret_cast <const char *>

#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

umlclassnode * find(std::list <umlclassnode> & list, const char *id ) {
    if ( id != NULL ) {
        std::list <umlclassnode>::iterator it = list.begin ();
        while ( it != list.end () ) {
            if ((*it).key.id.compare (id) == 0) {
                return &(*it);
            }
            ++it;
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


void addparent(umlclass & key, umlclassnode & derived) {
    umlclassnode tmp;
    tmp.key = key;
    derived.parents.push_front (tmp);
}

void adddependency(umlclassnode & dependent, umlclassnode & dependee) {
    umlclassnode tmp;
    tmp.key = dependent.key;
    dependee.dependencies.push_front (tmp);
}

void addaggregate(const char *name, char composite, umlclassnode & base,
                  umlclassnode & associate, const char *multiplicity) {
    umlassoc tmp;
    if (name != NULL && strlen (name) > 2)
        parse_dia_string(name, tmp.name);
    if (multiplicity != NULL)
        strncpy (tmp.multiplicity, multiplicity+1, strlen (multiplicity)-2);
    else
        sprintf(tmp.multiplicity, "1");
    tmp.key = base.key;
    tmp.composite = composite;
    associate.associations.push_front (tmp);
}

void inherit_realize ( std::list <umlclassnode> & classlist, const char * base, const char * derived ) {
    umlclassnode *umlbase, *umlderived;
    umlbase = find(classlist, base);
    umlderived = find(classlist, derived);
    if ( umlbase != NULL && umlderived != NULL ) {
        addparent(umlbase->key, *umlderived);
    }
}

void associate ( std::list <umlclassnode> & classlist, const char * name, char composite,
                 const char * base, const char * aggregate, const char *multiplicity) {
    umlclassnode *umlbase, *umlaggregate;
    umlbase = find(classlist, base);
    umlaggregate = find(classlist, aggregate);
    if ( umlbase != NULL && umlaggregate != NULL) {
        addaggregate(name, composite, *umlbase, *umlaggregate, multiplicity);
    }
}

void make_depend ( std::list <umlclassnode> & classlist, const char * dependent, const char * dependee) {
    umlclassnode *umldependent, *umldependee;
    umldependent = find(classlist, dependent);
    umldependee = find(classlist, dependee);
    if ( umldependent != NULL && umldependee != NULL) {
        adddependency(*umldependent, *umldependee);
    }
}

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void insert_attribute(umlattribute &n, std::list <umlattribute> &l) {
    std::list <umlattribute>::iterator itl;
    
    itl = l.begin ();
    
    if (itl == l.end ()) {
        l.push_back (n);
    }
    else {
        while ((itl != l.end ()) && ((*itl).visibility >= n.visibility)) {
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

/**
  * Inserts "n" into the list "l", in orderly fashion
*/
void insert_operation(umloperation &n, std::list <umloperation> &l) {
    std::list <umloperation>::iterator itl;
    
    itl = l.begin ();
    
    if (itl == l.end ()) {
        l.push_back (n);
    }
    else {
        while ((itl != l.end ()) && ((*itl).attr.visibility >= n.attr.visibility)) {
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

void insert_template(umltemplate &n, std::list <umltemplate> &l) {
    l.push_back (n);
}

void parse_attribute(xmlNodePtr node, umlattribute &tmp) {
    xmlChar *nodename;
    xmlChar *attrval;

    tmp.name.clear ();
    tmp.value.clear ();
    tmp.type.clear ();
    tmp.comment.clear ();
    tmp.visibility = '0';
    tmp.kind     = '0';
    while ( node != NULL ) {
        nodename = xmlGetProp(node, BAD_CAST2 ("name"));

        if ( !strcmp("name", BAD_TSAC2 (nodename)) ) {
            parse_dia_node(node->xmlChildrenNode, tmp.name);
        } else if ( !strcmp("value", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, tmp.value);
            }
        } else if ( !strcmp("type", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
                parse_dia_node(node->xmlChildrenNode, tmp.type);
            } else {
                tmp.type.clear ();
            }
        } else if ( !strcmp("comment", BAD_TSAC2 (nodename))) {
            if (node->xmlChildrenNode->xmlChildrenNode != NULL) {
               parse_dia_node(node->xmlChildrenNode, tmp.comment);
            } else {
               tmp.comment.clear ();
          }
        } else if ( !strcmp("kind", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(tmp.kind));
            free(attrval);
        } else if ( !strcmp("visibility", BAD_TSAC2 (nodename))) {
            attrval = xmlGetProp(node->xmlChildrenNode, BAD_CAST2 ("val"));
            sscanf(BAD_TSAC2 (attrval), "%c", &(tmp.visibility));
            free(attrval);
        } else if ( !strcmp("abstract", BAD_TSAC2 (nodename))) {
            tmp.isabstract = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("class_scope", BAD_TSAC2 (nodename))) {
            tmp.isstatic = parse_boolean(node->xmlChildrenNode);
        } else if ( !strcmp("query", BAD_TSAC2 (nodename))) {
            tmp.isconstant = parse_boolean(node->xmlChildrenNode);
        }

        free(nodename);
        node = node->next;
    }
}

void parse_attributes(xmlNodePtr node, std::list <umlattribute> &retour) {
    while ( node != NULL ) {
        umlattribute an;
        parse_attribute(node->xmlChildrenNode, an);
        insert_attribute(an, retour);
        node = node->next;
    }
}

void parse_operation(xmlNodePtr node, umloperation &tmp) {
    xmlChar *nodename;
    parse_attribute(node, tmp.attr);
    while ( node != NULL ) {
        nodename = xmlGetProp(node, BAD_CAST2 ("name"));
        if ( !strcmp("parameters", BAD_TSAC2 (nodename)) ) {
            parse_attributes(node->xmlChildrenNode, tmp.parameters);
        }
        free(nodename);
        node = node->next;
    }
}

void parse_operations(xmlNodePtr node, std::list <umloperation> &res) {
    while ( node != NULL ) {
        umloperation on;
        parse_operation (node->xmlChildrenNode, on);
        insert_operation(on, res);
        node = node->next;
    }
    return;
}

void parse_template(xmlNodePtr node, umltemplate *tmp) {
    parse_dia_string(BAD_TSAC2 (node->xmlChildrenNode->xmlChildrenNode->content), tmp->name);
    parse_dia_string(BAD_TSAC2 (node->next->xmlChildrenNode->xmlChildrenNode->content), tmp->type);
}

void parse_templates(xmlNodePtr node, std::list <umltemplate> &res) {
    while ( node != NULL) {
        if ( node->xmlChildrenNode->xmlChildrenNode->xmlChildrenNode != NULL &&
                node->xmlChildrenNode->next->xmlChildrenNode->xmlChildrenNode != NULL ) {
            umltemplate tn;
            parse_template(node->xmlChildrenNode, &tn);
            insert_template(tn, res);
        }
        node = node->next;
    }
    return;
}

/**
  * Adds get() (or is()) and set() methods for each attribute
  * myself MUST be != null
*/
void make_getset_methods(umlclass &myself) {
    std::string tmpname;
    std::list <umlattribute>::iterator attrlist;

    attrlist = myself.attributes.begin ();
    while (attrlist != myself.attributes.end ()) {
        if ( ! (*attrlist).isabstract) {
            umlattribute parameter;
            /* The SET method */
            umloperation operation;

            parameter.name.assign ("value");
            parameter.type.assign ((*attrlist).type);
            parameter.value.clear ();
            parameter.isstatic = 0;
            parameter.isconstant = 0;
            parameter.isabstract = 0;
            parameter.visibility = '0';
            operation.parameters.push_back (parameter);

            operation.implementation.clear ();
            operation.implementation.append ("    ");
            operation.implementation.append ((*attrlist).name);
            operation.implementation.append (" = value;");

            tmpname = strtoupperfirst((*attrlist).name);
            operation.attr.name.append ("set");
            operation.attr.name.append (tmpname);
            operation.attr.isabstract = 0;
            operation.attr.isstatic = 0;
            operation.attr.isconstant = 0;
            operation.attr.visibility = '0';
            operation.attr.value.clear ();
            operation.attr.type.assign ("void");

            insert_operation(operation, myself.operations);

            /* The GET or IS method */
            tmpname = strtoupperfirst((*attrlist).name);
            if ( (*attrlist).type.compare ("boolean") == 0) {
                operation.attr.name.assign ("is");
            } else {
                operation.attr.name.assign ("get");
            }
            operation.attr.name.append (tmpname);

            operation.implementation.assign ("    return ");
            operation.implementation.append ((*attrlist).name);
            operation.implementation.append (";");

            operation.attr.isabstract = 0;
            operation.attr.isstatic = 0;
            operation.attr.isconstant = 1;
            operation.attr.visibility = '0';
            operation.attr.value.clear ();
            operation.attr.type.assign ((*attrlist).type);

            insert_operation(operation, myself.operations);
        }
        ++attrlist;
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

void parse_package(xmlNodePtr package, umlpackage &res) {
    xmlNodePtr attribute;
    xmlChar *attrname;

    res.parent = NULL;

    attribute = package->xmlChildrenNode;
    while ( attribute != NULL ) {
        attrname = xmlGetProp(attribute, BAD_CAST2 ("name"));
        if( attrname != NULL ) {
            if ( !strcmp("name", BAD_TSAC2 (attrname)) ) {
                parse_dia_node(attribute->xmlChildrenNode, res.name);
            } else if ( !strcmp ( "obj_pos", BAD_TSAC2 (attrname) ) ) {
                parse_geom_position(attribute->xmlChildrenNode, &res.geom );
            } else if ( !strcmp ( "elem_width", BAD_TSAC2 (attrname) ) ) {
                parse_geom_width(attribute->xmlChildrenNode, &res.geom );
            } else if ( !strcmp ( "elem_height", BAD_TSAC2 (attrname) ) ) {
                parse_geom_height(attribute->xmlChildrenNode, &res.geom );
            }
            xmlFree (attrname);
        }
        attribute = attribute->next;
    }
    return;
}

void parse_class(xmlNodePtr class_, umlclassnode & res) {
    xmlNodePtr attribute;
    xmlChar *attrname;

    res.key.package = NULL;

    attribute = class_->xmlChildrenNode;
    while ( attribute != NULL ) {
        attrname = xmlGetProp(attribute, BAD_CAST2 ("name"));
        /* fix a segfault - dia files contains *also* some rare tags without any "name" attribute : <dia:parent  for ex.  */
        if( attrname == NULL ) {
            attribute = attribute->next;
            continue;
        }
        if ( !strcmp("name", BAD_TSAC2 (attrname)) ) {
            parse_dia_node(attribute->xmlChildrenNode, res.key.name);
        } else if ( !strcmp ( "obj_pos", BAD_TSAC2 (attrname) ) ) {
            parse_geom_position(attribute->xmlChildrenNode, &res.key.geom );
        } else if ( !strcmp ( "elem_width", BAD_TSAC2 (attrname) ) ) {
            parse_geom_width(attribute->xmlChildrenNode, &res.key.geom );
        } else if ( !strcmp ( "elem_height", BAD_TSAC2 (attrname )) ) {
            parse_geom_height(attribute->xmlChildrenNode, &res.key.geom );
        } else if ( !strcmp("comment", BAD_TSAC2 (attrname)))  {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
               parse_dia_node(attribute->xmlChildrenNode, res.key.comment);
            }  else {
               res.key.comment.clear ();
            }
        } else if ( !strcmp("stereotype", BAD_TSAC2 (attrname)) ) {
            if ( attribute->xmlChildrenNode->xmlChildrenNode != NULL ) {
                parse_dia_node(attribute->xmlChildrenNode, res.key.stereotype);
            } else {
                res.key.stereotype.clear ();
            }
        } else if ( !strcmp("abstract", BAD_TSAC2 (attrname)) ) {
            res.key.isabstract = parse_boolean(attribute->xmlChildrenNode);
        } else if ( !strcmp("attributes", BAD_TSAC2 (attrname)) ) {
            parse_attributes(attribute->xmlChildrenNode, res.key.attributes);
        } else if ( !strcmp("operations", BAD_TSAC2 (attrname)) ) {
            parse_operations(attribute->xmlChildrenNode, res.key.operations);
            if ( res.key.stereotype.compare ("getset") == 0) {
                make_getset_methods(res.key);
            }
        } else if ( !strcmp("templates", BAD_TSAC2 (attrname)) ) {
            parse_templates(attribute->xmlChildrenNode, res.key.templates);
        }
        free(attrname);
        attribute = attribute->next;
    }
    return;
}

/**
  Simple, non-compromising, implementation declaration.
  This function creates a plain vanilla interface (an
  umlclasslist) and associates it to the implementator.
  The implementator's code should contain the interface
  name, but the interface itself will not be inserted
  into the classlist, so no code can be generated for it.
*/
void lolipop_implementation(std::list <umlclassnode> & classlist, xmlNodePtr object) {
    xmlNodePtr attribute;
    xmlChar *id = NULL;
    const char *name = NULL;
    xmlChar *attrname;
    umlclassnode * implementator;

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
        umlclass key;
        key.package = NULL;
        key.id.assign ("00");
        parse_dia_string (name, key.name);
        key.stereotype.assign ("Interface");
        key.isabstract = 1;
        addparent(key, *implementator);
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

void parse_diagram(char *diafile, std::list <umlclassnode> & res) {
    xmlDocPtr ptr;
    xmlChar *end1 = NULL;
    xmlChar *end2 = NULL;

    xmlNodePtr object = NULL;
    std::list <umlpackage>::iterator dummypcklst;
    std::list <umlpackage> packagelst;

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
            umlclassnode tmplist;
            parse_class(object, tmplist);
            // We get the ID of the object here
            xmlChar *objid = xmlGetProp(object, BAD_CAST2 ("id"));
            tmplist.key.id.assign (BAD_TSAC2 (objid));
            free(objid);

            // We insert it here
            res.push_back (tmplist);
        } else if ( !strcmp("UML - LargePackage", BAD_TSAC2 (objtype)) || !strcmp("UML - SmallPackage", BAD_TSAC2 (objtype)) ) {
            umlpackage tmppck;
            parse_package(object, tmppck);

            xmlChar *objid = xmlGetProp(object, BAD_CAST2 ("id"));
            tmppck.id.assign (BAD_TSAC2 (objid));
            free(objid);

            // We insert it here
            packagelst.push_back (tmppck);
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
                    associate(res, thisname, composite, BAD_TSAC2 (end1), BAD_TSAC2 (end2), multiplicity_a);
                } else {
                    if (thisname == NULL || !*thisname || !strcmp("##", thisname))
                        thisname = name_b;
                    associate(res, thisname, composite, BAD_TSAC2 (end2), BAD_TSAC2(end1), multiplicity_b);
                }
                free(end1);
                end1 = NULL;
                free(end2);
                end2 = NULL;
            }

        } else if ( !strcmp("UML - Dependency", BAD_TSAC2 (objtype)) ) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while ( attribute != NULL ) {
                if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                    make_depend(res, BAD_TSAC2 (end1), BAD_TSAC2 (end2));
                    free(end1);
                    end1 = NULL;
                    free(end2);
                    end2 = NULL;
                }
                attribute = attribute->next;
            }
        } else if ( !strcmp("UML - Realizes", BAD_TSAC2 (objtype)) ) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while ( attribute != NULL ) {
                if ( !strcmp("connections", BAD_TSAC2 (attribute->name)) ) {
                    end1 = xmlGetProp(attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp(attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                    inherit_realize(res, BAD_TSAC2 (end1), BAD_TSAC2(end2));
                    free(end2);
                    end2 = NULL;
                    free(end1);
                    end1 = NULL;
                }
                attribute = attribute->next;
            }
        } else if ( !strcmp("UML - Implements", BAD_TSAC2 (objtype)) ) {
            lolipop_implementation(res, object);
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
                    inherit_realize(res, BAD_TSAC2 (end1), BAD_TSAC2(end2));
                    free(end2);
                    free(end1);
                }
                attribute = attribute->next;
            }
        }
        free(objtype);
        object = getNextObject(object);
    }

    /* Packages: we should scan the packagelist and then the res.
       Scanning the packagelist we'll build all relationships between
       packages.  Scanning the res we'll associate its own package
       to each class.

       FIXME: maybe we can do both in the same pass */

    /* Build the relationships between packages */
    dummypcklst = packagelst.begin ();
    while ( dummypcklst != packagelst.end () ) {
        std::list <umlpackage>::iterator tmppcklst = packagelst.begin ();
        while ( tmppcklst != packagelst.end () ) {
            if ( is_inside(&(*dummypcklst).geom, &(*tmppcklst).geom) ) {
                if ( ((*tmppcklst).parent == NULL) ||
                     (! is_inside ( &(*dummypcklst).geom, &(*tmppcklst).parent->geom ))) {
                    (*tmppcklst).parent = new umlpackage;
                    (*tmppcklst).parent->id = (*dummypcklst).id;
                    (*tmppcklst).parent->name = (*dummypcklst).name;
                    (*tmppcklst).parent->geom = (*dummypcklst).geom;
                    (*tmppcklst).parent->parent = (*dummypcklst).parent;
                    (*tmppcklst).parent->directory = (*dummypcklst).directory;
                }
            }
            ++tmppcklst;
        }
        ++dummypcklst;
    }

    /* Associate packages to classes */
    dummypcklst = packagelst.begin ();
    while ( dummypcklst != packagelst.end () ) {
        std::list <umlclassnode>::iterator it = res.begin ();
        while ( it != res.end () ) {
            if ( is_inside(&(*dummypcklst).geom,&(*it).key.geom) ) {
                if ( ((*it).key.package == NULL) ||
                     (! is_inside ( &(*dummypcklst).geom, &(*it).key.package->geom ))) {
                    free ((*it).key.package);
                    (*it).key.package = new umlpackage;
                    (*it).key.package->id = (*dummypcklst).id;
                    (*it).key.package->name = (*dummypcklst).name;
                    (*it).key.package->geom = (*dummypcklst).geom;
                    (*it).key.package->parent = (*dummypcklst).parent;
                    (*it).key.package->directory = (*dummypcklst).directory;
                }
            }
            ++it;
        }
        ++dummypcklst;
    }
    
    xmlFreeDoc (ptr);
    
    return;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
