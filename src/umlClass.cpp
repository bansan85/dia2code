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

#include "umlClass.hpp"
#include "string2.hpp"
#include "parse_diagram.hpp"
#include "umlClassNode.hpp"

umlClass::umlClass () :
    id ("00"),
    name (),
    stereotype (),
    comment (),
    isabstract (false),
    attributes (),
    operations (),
    templates (),
    package (NULL),
    geom {0., 0., 0., 0.}
{
}

const std::string &
umlClass::getId () const
{
    return id;
}

const std::string &
umlClass::getName () const
{
    return name;
}

const std::string &
umlClass::getStereotype () const
{
    return stereotype;
}

const std::string &
umlClass::getComment () const
{
    return comment;
}

const std::list <umlAttribute> &
umlClass::getAttributes () const
{
    return attributes;
}

const std::list <umlOperation> &
umlClass::getOperations () const
{
    return operations;
}

const umlPackage *
umlClass::getPackage () const
{
    return package;
}

const std::list <std::pair <std::string, std::string> > &
umlClass::getTemplates () const
{
    return templates;
}

/**
  * Adds get() (or is()) and set() methods for each attribute
*/
void
umlClass::make_getset_methods () {
    for (umlAttribute & attrlist : attributes) {
        if (!attrlist.isAbstract ()) {
            std::string tmpname, impl;

            /* The SET method */
            umlAttribute parameter ("value",
                                    "",
                                    attrlist.getType (),
                                    "",
                                    '0',
                                    false,
                                    false,
                                    false,
                                    '1');
            impl.assign ("    ");
            impl.append (attrlist.getName ());
            impl.append (" = value;");

            tmpname.assign ("set");
            tmpname.append (strtoupperfirst(attrlist.getName ()));

            umlOperation operation (tmpname,
                                    "",
                                    "void",
                                    "",
                                    '0',
                                    false,
                                    false,
                                    false,
                                    '1',
                                    impl);
            operation.addParameter (parameter);
            umlOperation::insert_operation(operation, operations);

            /* The GET or IS method */
            impl.assign ("    return ");
            impl.append (attrlist.getName ());
            impl.append (";");
            if (attrlist.getType ().compare ("boolean") == 0) {
                tmpname.assign ("is");
            } else {
                tmpname.assign ("get");
            }
            tmpname.append (strtoupperfirst (attrlist.getName ()));

            umlOperation operation2 (tmpname, "", attrlist.getType (),
                                     "", '0', false, false, true, '1', impl);
            umlOperation::insert_operation (operation2, operations);
        }
    }
 }

/**
  Simple, non-compromising, implementation declaration.
  This function creates a plain vanilla interface (an
  umlclasslist) and associates it to the implementator.
  The implementator's code should contain the interface
  name, but the interface itself will not be inserted
  into the classlist, so no code can be generated for it.
*/
void
umlClass::lolipop_implementation (std::list <umlClassNode> & classlist,
                                  xmlNodePtr object) {
    xmlNodePtr attribute;
    xmlChar *id = NULL;
    const char *name = NULL;
    xmlChar *attrname;
    umlClassNode * implementator;

    attribute = object->xmlChildrenNode;
    while (attribute != NULL) {
        if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
            id = xmlGetProp (attribute->xmlChildrenNode, BAD_CAST2 ("to"));
        } else {
            attrname = xmlGetProp (attribute, BAD_CAST2 ("name"));
            if (attrname != NULL &&
                !strcmp ("text", BAD_TSAC2 (attrname)) &&
                attribute->xmlChildrenNode != NULL &&
                attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
                name = BAD_TSAC2 (attribute->xmlChildrenNode->xmlChildrenNode->content);
            } else {
                name = "";
            }
            free (attrname);
        }
        attribute = attribute->next;
    }
    implementator = umlClassNode::find (classlist, BAD_TSAC2 (id));
    free (id);
    if (implementator != NULL && name != NULL && strlen (name) > 2) {
        umlClass * key = new umlClass ();
        key->package = NULL;
        key->id.assign ("00");
        parseDiaString (name, key->name);
        key->stereotype.assign ("Interface");
        key->isabstract = 1;
        implementator->addparent (key);
    }
}

void
associate (std::list <umlClassNode> & classlist,
           const char * name,
           char composite,
           const char * base,
           const char * aggregate,
           const char *multiplicity) {
    umlClassNode *umlbase, *umlaggregate;
    umlbase = umlClassNode::find (classlist, base);
    umlaggregate = umlClassNode::find (classlist, aggregate);
    if (umlbase != NULL && umlaggregate != NULL) {
        umlaggregate->addaggregate (name, composite, *umlbase, multiplicity);
    }
}

void
make_depend (std::list <umlClassNode> & classlist,
             const char * dependent,
             const char * dependee) {
    umlClassNode *umldependent, *umldependee;
    umldependent = umlClassNode::find (classlist, dependent);
    umldependee = umlClassNode::find (classlist, dependee);
    if (umldependent != NULL && umldependee != NULL) {
        umldependee->adddependency (*umldependent);
    }
    else {
        fprintf (stderr,
                 "Impossible to find dependance between id=%s and id=%s. "
                 "Maybe id=%s is not a class (package for example).\n",
                 dependent,
                 dependee,
                 umldependent == NULL ? dependent : dependee);
    }
}

void
inherit_realize (std::list <umlClassNode> & classlist,
                 const char * base,
                 const char * derived) {
    umlClassNode *umlbase, *umlderived;
    umlbase = umlClassNode::find (classlist, base);
    umlderived = umlClassNode::find (classlist, derived);
    if (umlbase != NULL && umlderived != NULL) {
        umlderived->addparent (umlbase);
    }
}

/*
   returns 1 if the position point of the object with geom2 is inside the object with geom1
           0 otherwise
*/
int
is_inside (const geometry & geom1,
           const geometry & geom2) {
    return geom1.pos_x < geom2.pos_x &&
           geom2.pos_x < geom1.pos_x + geom1.width &&
           geom1.pos_y < geom2.pos_y &&
           geom2.pos_y < geom1.pos_y + geom1.height;

}

void
parse_geom_position (xmlNodePtr attribute, geometry * geom) {
    xmlChar *val;
    char * token;
    val = xmlGetProp (attribute, BAD_CAST2 ("val"));
    token = strtok (reinterpret_cast <char *> (val), ",");
    sscanf (token, "%f", &(geom->pos_x) );
    token = strtok (NULL, ",");
    sscanf (token, "%f", &(geom->pos_y) );
    xmlFree (val);
}

void
parse_geom_width (xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp (attribute, BAD_CAST2 ("val"));
    sscanf (BAD_TSAC2 (val), "%f", &(geom->width) );
    xmlFree (val);
}

void
parse_geom_height (xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp (attribute, BAD_CAST2 ("val"));
    sscanf (BAD_TSAC2 (val), "%f", &(geom->height) );
    xmlFree (val);
}

void
umlClass::parse_diagram (char *diafile, std::list <umlClassNode> & res) {
    xmlDocPtr ptr;
    xmlChar *end1 = NULL;
    xmlChar *end2 = NULL;

    xmlNodePtr object = NULL;
    std::list <umlPackage> packagelst;

    ptr = xmlParseFile (diafile);

    if (ptr == NULL) {
        throw std::string ("File " + std::string (diafile) + " does not exist or is not a Dia diagram.\n");
    }

    // we search for the first "object" node
    recursiveSearch (ptr->xmlRootNode->xmlChildrenNode->next, &object);

    while (object != NULL) {
        xmlChar *objtype = xmlGetProp (object, BAD_CAST2 ("type"));
        // Here we have a Dia object
        if (strcmp ("UML - Class", BAD_TSAC2 (objtype)) == 0) {
            // Here we have a class definition
            umlClassNode tmplist;
            tmplist.parse_class (object);
            // We get the ID of the object here
            xmlChar *objid = xmlGetProp (object, BAD_CAST2 ("id"));
            tmplist.id.assign (BAD_TSAC2 (objid));
            free (objid);

            // We insert it here
            res.push_back (tmplist);
        } else if (!strcmp ("UML - LargePackage", BAD_TSAC2 (objtype)) ||
                   !strcmp ("UML - SmallPackage", BAD_TSAC2 (objtype))) {
            xmlChar *objid = xmlGetProp (object, BAD_CAST2 ("id"));
            umlPackage tmppck (object, BAD_TSAC2 (objid));
            free (objid);

            // We insert it here
            packagelst.push_back (tmppck);
        }
        free (objtype);
        object = getNextObject (object);
    }

    // Second pass - Implementations and associations

    // we search for the first "object" node
    recursiveSearch (ptr->xmlRootNode->xmlChildrenNode->next, &object);

    while (object != NULL) {
        xmlChar *objtype = xmlGetProp (object, BAD_CAST2 ("type"));
        if (!strcmp ("UML - Association", BAD_TSAC2 (objtype))) {
            const char *name = NULL;
            const char *name_a = NULL;
            const char *name_b = NULL;
            const char *multiplicity_a = NULL;
            const char *multiplicity_b = NULL;
            char direction = 0;
            char composite = 0;
            xmlNodePtr attribute = object->xmlChildrenNode;

            while (attribute != NULL) {
                char *attrtype = reinterpret_cast <char *> (xmlGetProp (
                                              attribute, BAD_CAST2  ("name")));

                if (attrtype != NULL) {
                    xmlNodePtr child = attribute->xmlChildrenNode;
                    if (!strcmp ("direction", attrtype)) {
                        xmlChar *tmptype = xmlGetProp (child, BAD_CAST2 ("val"));
                        if (!strcmp (BAD_TSAC2 (tmptype), "0")) {
                            direction = 1;
                        }
                        else {
                            direction = 0;
                        }
                        free (tmptype);
                    }
                    else if (!strcmp ("assoc_type", attrtype)) {
                        xmlChar *tmptype = xmlGetProp (child,
                                                       BAD_CAST2 ("val"));
                        if (!strcmp (BAD_TSAC2 (tmptype), "1")) {
                            composite = 0;
                        }
                        else {
                            composite = 1;
                        }
                        free (tmptype);
                    }
                    else if (child->xmlChildrenNode) {
                        xmlNodePtr grandchild = child->xmlChildrenNode;
                        if (!strcmp (attrtype, "name")) {
                            name = BAD_TSAC2 (grandchild->content);
                        }
                        else if (!strcmp (attrtype, "role_a")) {
                            name_a = BAD_TSAC2 (grandchild->content);
                        }
                        else if (!strcmp (attrtype, "role_b")) {
                            name_b = BAD_TSAC2 (grandchild->content);
                        }
                        else if (!strcmp (attrtype, "multipicity_a")) {
                            multiplicity_a = BAD_TSAC2 (grandchild->content);
                        }
                        else if (!strcmp (attrtype, "multipicity_b")) {
                            multiplicity_b = BAD_TSAC2 (grandchild->content);
                        }
                        else if (!strcmp (attrtype, "ends")) {
                            if (!strcmp (BAD_TSAC2 (child->name), "composite")) {
                                while (grandchild) {
                                    xmlNodePtr ggchild = grandchild->xmlChildrenNode;
                                    if (ggchild->xmlChildrenNode) {
                                        attrtype = reinterpret_cast <char *> (xmlGetProp (grandchild, BAD_CAST2 ("name")));
                                        if (!strcmp (attrtype, "role")) {
                                            name_a = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if (!strcmp (attrtype, "multiplicity")) {
                                            multiplicity_a = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if (!strcmp (attrtype, "aggregate")) {
                                            /* TODO */
                                        }
                                    }
                                    grandchild = grandchild->next;
                                }
                            }
                            child = child->next;
                            if (child != NULL && !strcmp (BAD_TSAC2 (child->name), "composite")) {
                                grandchild = child->xmlChildrenNode;
                                while (grandchild) {
                                    xmlNodePtr ggchild = grandchild->xmlChildrenNode;
                                    if (ggchild->xmlChildrenNode) {
                                        attrtype = reinterpret_cast <char *> (xmlGetProp (grandchild, BAD_CAST2 ("name")));
                                        if (!strcmp (attrtype, "role")) {
                                            name_b = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if (!strcmp (attrtype, "multiplicity")) {
                                            multiplicity_b = BAD_TSAC2 (ggchild->xmlChildrenNode->content);
                                        }
                                        else if (!strcmp (attrtype, "aggregate")) {
                                            /* TODO */
                                        }
                                    }
                                    grandchild = grandchild->next;
                                }
                            }
                        }
                    }
                    free (attrtype);
                }
                else if (!strcmp (BAD_TSAC2 (attribute->name), "connections")) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode, BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode->next, BAD_CAST2 ("to"));
                }

                attribute = attribute->next;
            }

            if (end1 != NULL && end2 != NULL) {
                const char *thisname = name;
                if (direction == 1) {
                    if (thisname == NULL || !*thisname || !strcmp ("##", thisname)) {
                        thisname = name_a;
                    }
                    associate (res, thisname, composite, BAD_TSAC2 (end1), BAD_TSAC2 (end2), multiplicity_a);
                } else {
                    if (thisname == NULL || !*thisname || !strcmp ("##", thisname)) {
                        thisname = name_b;
                    }
                    associate (res, thisname, composite, BAD_TSAC2 (end2), BAD_TSAC2 (end1), multiplicity_b);
                }
                free (end1);
                end1 = NULL;
                free (end2);
                end2 = NULL;
            }

        } else if (!strcmp ("UML - Dependency", BAD_TSAC2 (objtype))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode->next,
                                       BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                    make_depend (res, BAD_TSAC2 (end1), BAD_TSAC2 (end2));
                    free (end1);
                    end1 = NULL;
                    free (end2);
                    end2 = NULL;
                }
                attribute = attribute->next;
            }
        } else if (!strcmp ("UML - Realizes", BAD_TSAC2 (objtype))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode->next,
                                       BAD_CAST2 ("to"));
                    inherit_realize (res, BAD_TSAC2 (end1), BAD_TSAC2 (end2));
                    free (end2);
                    end2 = NULL;
                    free (end1);
                    end1 = NULL;
                }
                attribute = attribute->next;
            }
        } else if (!strcmp ("UML - Implements", BAD_TSAC2 (objtype))) {
            umlClass::lolipop_implementation (res, object);
        }
        free (objtype);
        object = getNextObject (object);
    }


    /* Generalizations: we must put this AFTER all the interface
       implementations. generate_code_java relies on this. */
    recursiveSearch (ptr->xmlRootNode->xmlChildrenNode->next, &object);
    while (object != NULL) {
        xmlChar *objtype = xmlGetProp (object, BAD_CAST2 ("type"));
        if (!strcmp ("UML - Generalization", BAD_TSAC2 (objtype))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode->next,
                                       BAD_CAST2 ("to"));
                    inherit_realize (res, BAD_TSAC2 (end1), BAD_TSAC2(end2));
                    free (end2);
                    free (end1);
                }
                attribute = attribute->next;
            }
        }
        free (objtype);
        object = getNextObject (object);
    }

    /* Packages: we should scan the packagelist and then the res.
       Scanning the packagelist we'll build all relationships between
       packages.  Scanning the res we'll associate its own package
       to each class. */

    // Build the relationships between packages
    for (umlPackage & dummypcklst : packagelst) {
        for (umlPackage & tmppcklst : packagelst) {
            if (is_inside (dummypcklst.getGeometry (),
                           tmppcklst.getGeometry ())) {
                if ((tmppcklst.getParent () == NULL) ||
                     (! is_inside (dummypcklst.getGeometry (),
                                   tmppcklst.getParent ()->getGeometry ()))) {
                    tmppcklst.setParent (new umlPackage (dummypcklst));
                }
            }
        }
    }

    // Associate packages to classes
    for (umlPackage & dummypcklst : packagelst) {
        for (umlClassNode & it : res) {
            if (is_inside (dummypcklst.getGeometry (), it.geom)) {
                if ((it.package == NULL) ||
                     (! is_inside (dummypcklst.getGeometry (),
                                   it.package->getGeometry ()))) {
                    delete it.package;
                    it.package = new umlPackage (dummypcklst);
                }
            }
        }
    }

    xmlFreeDoc (ptr);

    return;
}

void
umlClass::parse_class (xmlNodePtr class_) {
    xmlNodePtr attribute;

    package = NULL;

    attribute = class_->xmlChildrenNode;
    while (attribute != NULL) {
        xmlChar *attrname;
        attrname = xmlGetProp (attribute, BAD_CAST2 ("name"));
        if (attrname == NULL) {
            attribute = attribute->next;
            continue;
        }
        if (!strcmp ("name", BAD_TSAC2 (attrname))) {
            parseDiaNode (attribute->xmlChildrenNode, name);
        } else if (!strcmp ("obj_pos", BAD_TSAC2 (attrname))) {
            parse_geom_position (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("elem_width", BAD_TSAC2 (attrname))) {
            parse_geom_width (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("elem_height", BAD_TSAC2 (attrname))) {
            parse_geom_height (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("comment", BAD_TSAC2 (attrname)))  {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
               parseDiaNode (attribute->xmlChildrenNode, comment);
            }  else {
               comment.clear ();
            }
        } else if (!strcmp ("stereotype", BAD_TSAC2 (attrname))) {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
                parseDiaNode (attribute->xmlChildrenNode, stereotype);
            } else {
                stereotype.clear ();
            }
        } else if (!strcmp ("abstract", BAD_TSAC2 (attrname))) {
            isabstract = parseBoolean (attribute->xmlChildrenNode);
        } else if (!strcmp ("attributes", BAD_TSAC2 (attrname))) {
            parseAttributes (attribute->xmlChildrenNode, attributes);
        } else if (!strcmp ("operations", BAD_TSAC2 (attrname))) {
            umlOperation::parse_operations (attribute->xmlChildrenNode,
                                            operations);
            if (stereotype.compare ("getset") == 0) {
                this->make_getset_methods ();
            }
        } else if (!strcmp ("templates", BAD_TSAC2 (attrname))) {
            parseTemplates (attribute->xmlChildrenNode, templates);
        }
        free (attrname);
        attribute = attribute->next;
    }
    return;
}

umlClass::~umlClass ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
