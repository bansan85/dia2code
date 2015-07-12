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
#include <algorithm>

#include "umlClass.hpp"
#include "string2.hpp"
#include "parse_diagram.hpp"
#include "scan_tree.hpp"
#include "umlClassNode.hpp"

umlClass::umlClass () :
    id ("00"),
    name (),
    comment (),
    abstract (false),
    pushed (false),
    stereotypeTypedef (false),
    stereotypeEnum (false),
    stereotypeConst (false),
    stereotypeStruct (false),
    stereotypeGetSet (false),
    stereotypeExtern (false),
    stereotypeInterface (false),
    stereotypeDllExport (false),
#ifdef ENABLE_CORBA
    stereotypeCorba (false),
#endif
    attributes (),
    operations (),
    circularLoop (),
    templates (),
    package (nullptr),
    geom ({0., 0., 0., 0.}) {
}

const std::string &
umlClass::getId () const {
    return id;
}

const std::string &
umlClass::getName () const {
    return name;
}

const std::string &
umlClass::getComment () const {
    return comment;
}

bool
umlClass::isAbstract () const {
    return abstract;
}

bool
umlClass::isPushed () const {
    return pushed;
}

void
umlClass::setPushed () {
    pushed = true;
}

bool
umlClass::isStereotypeTypedef () const {
    return stereotypeTypedef;
}

bool
umlClass::isStereotypeEnum () const {
    return stereotypeEnum;
}

bool
umlClass::isStereotypeConst () const {
    return stereotypeConst;
}

bool
umlClass::isStereotypeStruct () const {
    return stereotypeStruct;
}

void
umlClass::setStereotypeStruct (bool val) {
    stereotypeStruct = val;
}

bool
umlClass::isStereotypeGetSet () const {
    return stereotypeGetSet;
}

bool
umlClass::isStereotypeExtern () const {
    return stereotypeExtern;
}

bool
umlClass::isStereotypeInterface () const {
    return stereotypeInterface;
}

bool
umlClass::isStereotypeDllExport () const {
    return stereotypeDllExport;
}

#ifdef ENABLE_CORBA
bool
umlClass::isStereotypeCorba () const {
    return stereotypeCorba;
}
#endif

const std::list <umlAttribute> &
umlClass::getAttributes () const {
    return attributes;
}

const std::list <umlOperation> &
umlClass::getOperations () const {
    return operations;
}

void
umlClass::addCircularLoop (const umlClass * classe) {
    if (find (circularLoop.begin (),
              circularLoop.end (),
              classe) == circularLoop.end ()) {
        circularLoop.push_back (classe);
    }
}

const std::list <const umlClass *> &
umlClass::getCircularLoop () const {
    return circularLoop;
}

umlPackage *
umlClass::getPackage () const {
    return package;
}

const std::list <std::pair <std::string, std::string> > &
umlClass::getTemplates () const {
    return templates;
}

bool
umlClass::isEnumStereo (std::string & stereo) {
    return (isInside (stereo, "enum") ||
            isInside (stereo, "enumeration") ||
            isInside (stereo, "Enum") ||
            isInside (stereo, "Enumeration")
#ifdef ENABLE_CORBA
            || isInside (stereo, "CORBAEnum")
#endif
            );
}

bool
umlClass::isStructStereo (std::string & stereo) {
    return (isInside (stereo, "struct") ||
            isInside (stereo, "structure") ||
            isInside (stereo, "Struct") ||
            isInside (stereo, "Structure")
#ifdef ENABLE_CORBA
            || isInside (stereo, "CORBAStruct")
#endif
            );
}

bool
umlClass::isTypedefStereo (std::string & stereo) {
    return (isInside (stereo, "typedef") ||
            isInside (stereo, "TypeDef")
#ifdef ENABLE_CORBA
            || !stereo.compare ("CORBATypedef")
#endif
            );
}

#ifdef ENABLE_CORBA
bool
umlClass::isCorbaStereo (std::string & stereo) {
    return (!stereo.compare (0, 5, "CORBA");
}
#endif

bool
umlClass::isConstStereo (std::string & stereo) {
    return (isInside (stereo, "const") ||
            isInside (stereo, "constant") ||
            isInside (stereo, "Const") ||
            isInside (stereo, "Constant")
#ifdef ENABLE_CORBA
            || isInside (stereo, "CORBAConstant")
#endif
            );
}

bool
umlClass::isGetSetStereo (std::string & stereo) {
    return (isInside (stereo, "getset") ||
            isInside (stereo, "GetSet")
#ifdef ENABLE_CORBA
            || isInside (stereo, "CORBAGetSet")
#endif
            );
}

bool
umlClass::isInterfaceStereo (std::string & stereo) {
    return (isInside (stereo, "interface") ||
            isInside (stereo, "Interface")
#ifdef ENABLE_CORBA
            || isInside (stereo, "CORBAInterface")
#endif
            );
}

bool
umlClass::isDllExportStereo (std::string & stereo) {
    return (isInside (stereo, "dllexport") ||
            isInside (stereo, "DllExport"));
}

/**
  * Adds get () (or is ()) and set () methods for each attribute
*/
void
umlClass::makeGetSetMethods () {
    for (umlAttribute & attrlist : attributes) {
        umlOperation operation2 (attrlist.getName (),
                                 attrlist.getType (),
                                 "",
                                 Visibility::PUBLIC,
                                 Inheritance::FINAL,
                                 false,
                                 false,
                                 false,
                                 true,
                                 false);
        umlOperation::insertOperation (operation2, operations);
    }
}

void
associate (std::list <umlClassNode *> & classlist,
           const char * name,
           char composite,
           const char * base,
           const char * aggregate,
           const char *multiplicity,
           Visibility visibility) {
    umlClassNode *umlbase, *umlaggregate;
    umlbase = umlClassNode::find (classlist, base);
    umlaggregate = umlClassNode::find (classlist, aggregate);
    if (umlbase != NULL && umlaggregate != NULL) {
        umlaggregate->addAggregate (name,
                                    composite,
                                    umlbase,
                                    multiplicity,
                                    visibility);
    }
}

void
makeDepend (std::list <umlClassNode *> & classlist,
            std::list <umlPackage *> & packagelist,
            const char * dependent,
            const char * dependee,
            uint8_t      flag) {
    umlClassNode *umldependent, *umldependee;
    umldependent = umlClassNode::find (classlist, dependent);
    umldependee = umlClassNode::find (classlist, dependee);
    if (umldependent != NULL && umldependee != NULL) {
        umldependee->addDependency (umldependent, flag);
    }
    // The dependence is a package
    else if (umldependent == NULL && umldependee != NULL) {
        umlPackage *umldependent2 = umlPackage::find (packagelist, dependent);
        if (umldependent2 != NULL) {
            umldependee->addDependency (umldependent2);
        }
        else {
            std::cerr << "Failed to find dependency for " << dependent
                      << ".\n";
        }
    }
    // A package has a dependence.
    else {
        std::cerr << "A package can not have a dependence, only class.\n";
    }
}

void
inheritRealize (std::list <umlClassNode *> & classlist,
                const char * base,
                const char * derived,
                Visibility visible) {
    umlClassNode *umlbase, *umlderived;
    umlbase = umlClassNode::find (classlist, base);
    umlderived = umlClassNode::find (classlist, derived);
    if (umlbase != NULL && umlderived != NULL) {
        umlderived->addParent (umlbase, visible);
    }
}

/*
   returns 1 if the position point of the object with geom2 is inside the object with geom1
           0 otherwise
*/
int
isInside (const geometry & geom1,
          const geometry & geom2) {
    return geom1.posX < geom2.posX &&
           geom2.posX < geom1.posX + geom1.width &&
           geom1.posY < geom2.posY &&
           geom2.posY < geom1.posY + geom1.height;

}

void
parseGeomPosition (xmlNodePtr attribute, geometry * geom) {
    xmlChar *val;
    char * token;

    val = xmlGetProp (attribute, BAD_CAST2 ("val"));

#if defined(_WIN32) || defined(_WIN64)
    char *context = NULL;

    token = strtok_s (reinterpret_cast <char *> (val), ",", &context);
    sscanf_s (token, "%f", &(geom->posX) );
    token = strtok_s(nullptr, ",", &context);
    sscanf_s (token, "%f", &(geom->posY));
#else
    token = strtok(reinterpret_cast <char *> (val), ",");
    sscanf (token, "%f", &(geom->posX));
    token = strtok (nullptr, ",");
    sscanf (token, "%f", &(geom->posY));
#endif

    xmlFree (val);
}

void
parseGeomWidth (xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp (attribute, BAD_CAST2 ("val"));
#if defined(_WIN32) || defined(_WIN64)
    sscanf_s (BAD_TSAC2 (val), "%f", &(geom->width) );
#else
    sscanf (BAD_TSAC2 (val), "%f", &(geom->width) );
#endif
    xmlFree (val);
}

void
parseGeomHeight (xmlNodePtr attribute, geometry * geom ) {
    xmlChar *val;
    val = xmlGetProp (attribute, BAD_CAST2 ("val"));
#if defined(_WIN32) || defined(_WIN64)
    sscanf_s (BAD_TSAC2 (val), "%f", &(geom->height) );
#else
    sscanf (BAD_TSAC2 (val), "%f", &(geom->height) );
#endif
    xmlFree (val);
}

bool
umlClass::parseDiagram (char *diafile, std::list <umlClassNode *> & res) {
    xmlDocPtr ptr;
    xmlChar *end1 = nullptr;
    xmlChar *end2 = nullptr;

    xmlNodePtr object = NULL;
    std::list <umlPackage *> packagelst;

    ptr = xmlParseFile (diafile);

    if (ptr == NULL) {
        return false;
    }

    // we search for the first "object" node
    recursiveSearch (ptr->xmlRootNode->xmlChildrenNode->next, &object);

    while (object != NULL) {
        xmlChar *objtype = xmlGetProp (object, BAD_CAST2 ("type"));
        // Here we have a Dia object
        if (strcmp ("UML - Class", BAD_TSAC2 (objtype)) == 0) {
            // Here we have a class definition
            umlClassNode * tmplist = new umlClassNode ();
            tmplist->parseClass (object);
            // We get the ID of the object here
            xmlChar *objid = xmlGetProp (object, BAD_CAST2 ("id"));
            tmplist->id.assign (BAD_TSAC2 (objid));
            free (objid);

            // We insert it here
            res.push_back (tmplist);
        } else if (!strcmp ("UML - LargePackage", BAD_TSAC2 (objtype)) ||
                   !strcmp ("UML - SmallPackage", BAD_TSAC2 (objtype))) {
            xmlChar *objid = xmlGetProp (object, BAD_CAST2 ("id"));
            umlPackage *tmppck = new umlPackage (object, BAD_TSAC2 (objid));
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
            Visibility visibility_a = Visibility::PUBLIC;
            Visibility visibility_b = Visibility::PUBLIC;
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
                    else if (!strcmp ("visibility_a", attrtype)) {
                        xmlChar *tmptype = xmlGetProp (child,
                                                       BAD_CAST2 ("val"));
                        switch (tmptype[0]) {
                            case '0' : {
                                visibility_a = Visibility::PUBLIC;
                                break;
                            }
                            case '1' : {
                                visibility_a = Visibility::PRIVATE;
                                break;
                            }
                            case '2' : {
                                visibility_a = Visibility::PROTECTED;
                                break;
                            }
                            case '3' : {
                                visibility_a = Visibility::IMPLEMENTATION;
                                break;
                            }
                            default : {
                                std::cout << "Unknown visibility : "
                                          << tmptype[0] << "\n";
                                return false;
                            }
                        }
                        free (tmptype);
                    }
                    else if (!strcmp ("visibility_b", attrtype)) {
                        xmlChar *tmptype = xmlGetProp (child,
                                                       BAD_CAST2 ("val"));
                        switch (tmptype[0]) {
                            case '0' : {
                                visibility_b = Visibility::PUBLIC;
                                break;
                            }
                            case '1' : {
                                visibility_b = Visibility::PRIVATE;
                                break;
                            }
                            case '2' : {
                                visibility_b = Visibility::PROTECTED;
                                break;
                            }
                            case '3' : {
                                visibility_b = Visibility::IMPLEMENTATION;
                                break;
                            }
                            default : {
                                std::cout << "Unknown visibility : "
                                          << tmptype[0] << "\n";
                                return false;
                            }
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
                    if ((thisname == NULL) || (!*thisname) ||
                        (!strcmp ("##", thisname))) {
                        thisname = name_a;
                    }
                    associate (res,
                               thisname,
                               composite,
                               BAD_TSAC2 (end1),
                               BAD_TSAC2 (end2),
                               multiplicity_a,
                               visibility_a);
                } else {
                    if ((thisname == NULL) || (!*thisname) ||
                        (!strcmp ("##", thisname))) {
                        thisname = name_b;
                    }
                    associate (res,
                               thisname,
                               composite,
                               BAD_TSAC2 (end2),
                               BAD_TSAC2 (end1),
                               multiplicity_b,
                               visibility_b);
                }
            }
            free (end2);
            end2 = nullptr;
            free (end1);
            end1 = nullptr;

        } else if (!strcmp ("UML - Dependency", BAD_TSAC2 (objtype))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            bool noLoop = false;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode->next,
                                       BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                }
                else if (!strcmp ("attribute", BAD_TSAC2 (attribute->name))) {
                    xmlChar *name = xmlGetProp (attribute, BAD_CAST2 ("name"));

                    if (!strcmp ("stereotype", BAD_TSAC2 (name)))
                    {
                        std::string stereo;

                        parseDiaNode (attribute->xmlChildrenNode, stereo);
                        if (isInside (stereo, "NoLoop") ||
                            isInside (stereo, "noloop")) {
                            noLoop = true;
                        }
                    }
                    free (name);
                }
                attribute = attribute->next;
            }
            if ((BAD_TSAC2 (end1) == nullptr) &&
                (BAD_TSAC2 (end2) != nullptr)) {
                umlClassNode * node = umlClassNode::find (
                                                res, BAD_TSAC2 (end2));
                std::cerr << "One dependency of "
                          << node->getName ()
                          << " is broken.\n";
            }
            else if ((BAD_TSAC2 (end2) == nullptr) &&
                     (BAD_TSAC2 (end1) != nullptr)) {
                umlClassNode * node = umlClassNode::find (
                                                res, BAD_TSAC2 (end1));
                std::cerr << "One dependency of "
                          << node->getName ()
                          << " is broken.\n";
            }
            else {
                uint8_t flag = 0;
                if (noLoop) {
                    flag = flag | 1;
                }
                makeDepend (res,
                            packagelst,
                            BAD_TSAC2 (end1),
                            BAD_TSAC2 (end2),
                            flag);
            }
            free (end1);
            end1 = nullptr;
            free (end2);
            end2 = nullptr;
        } else if (!strcmp ("UML - Implements", BAD_TSAC2 (objtype))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            Visibility visible = Visibility::PUBLIC;
            end1 = nullptr;
            end2 = nullptr;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end2 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                }
                else if (!strcmp ("attribute", BAD_TSAC2 (attribute->name))) {
                    xmlChar *name = xmlGetProp (attribute, BAD_CAST2 ("name"));

                    if (!strcmp ("text", BAD_TSAC2 (name)))
                    {
                        std::string stereo;
                        umlClassNode * tmpnode;

                        parseDiaNode (attribute->xmlChildrenNode, stereo);
                        tmpnode = findByName (res, stereo);
                        if (tmpnode != NULL) {
                            end1 = reinterpret_cast <xmlChar *>
                                         (strdup (tmpnode->getId ().c_str ()));
                        }
                    }
                    free (name);
                }
                attribute = attribute->next;
            }
            if ((end1 != nullptr) && (end2 != nullptr)) {
                umlClassNode *umlend;

                inheritRealize (res,
                                BAD_TSAC2 (end1),
                                BAD_TSAC2 (end2),
                                visible);

                umlend = umlClassNode::find (res, BAD_TSAC2 (end1));
                if (umlend != NULL) {
                    umlend->stereotypeInterface = true;
                }
            }
            free (end2);
            end2 = nullptr;
            free (end1);
            end1 = nullptr;
        } else if ((!strcmp ("UML - Generalization", BAD_TSAC2 (objtype))) ||
                   (!strcmp ("UML - Realizes", BAD_TSAC2 (objtype)))) {
            xmlNodePtr attribute = object->xmlChildrenNode;
            Visibility visible = Visibility::PUBLIC;
            end1 = nullptr;
            end2 = nullptr;
            while (attribute != NULL) {
                if (!strcmp ("connections", BAD_TSAC2 (attribute->name))) {
                    end1 = xmlGetProp (attribute->xmlChildrenNode,
                                       BAD_CAST2 ("to"));
                    end2 = xmlGetProp (attribute->xmlChildrenNode->next,
                                       BAD_CAST2 ("to"));
                }
                else if (!strcmp ("attribute", BAD_TSAC2 (attribute->name))) {
                    xmlChar *name = xmlGetProp (attribute, BAD_CAST2 ("name"));

                    if (!strcmp ("stereotype", BAD_TSAC2 (name)))
                    {
                        std::string stereo;
                        parseDiaNode (attribute->xmlChildrenNode, stereo);
                        if (stereo.empty ()) {
                            visible = Visibility::PUBLIC;
                        }
                        else if (isInside (stereo, "public")) {
                            visible = Visibility::PUBLIC;
                        }
                        else if (isInside (stereo, "private")) {
                            visible = Visibility::PRIVATE;
                        }
                        else if (isInside (stereo, "protected")) {
                            visible = Visibility::PROTECTED;
                        }
                        else if (isInside (stereo,
                                           "implementation")) {
                            visible = Visibility::IMPLEMENTATION;
                        }
                        else {
                            std::cerr << "Unknown stereotype for Generalization or Realizes: "
                                      << stereo << ".\n"
                                      << "Allow stereotype is: \"public\", \"private\", \"protected\" and \"implementation\".\n";
                        }
                    }
                    free (name);
                }
                attribute = attribute->next;
            }
            if ((end1 != nullptr) && (end2 != nullptr)) {
                inheritRealize (res,
                                BAD_TSAC2 (end1),
                                BAD_TSAC2 (end2),
                                visible);
                if (!strcmp ("UML - Realizes", BAD_TSAC2 (objtype))) {
                    umlClassNode *umlend;

                    umlend = umlClassNode::find (res, BAD_TSAC2 (end1));
                    if (umlend != NULL) {
                        umlend->stereotypeInterface = true;
                    }
                }
            }
            free (end2);
            end2 = nullptr;
            free (end1);
            end1 = nullptr;
        }
        free (objtype);
        object = getNextObject (object);
    }

    /* Packages: we should scan the packagelist and then the res.
       Scanning the packagelist we'll build all relationships between
       packages.  Scanning the res we'll associate its own package
       to each class. */

    // Build the relationships between packages
    for (umlPackage * dummypcklst : packagelst) {
        for (umlPackage * tmppcklst : packagelst) {
            if (isInside (dummypcklst->getGeometry (),
                           tmppcklst->getGeometry ())) {
                if ((tmppcklst->getParent () == NULL) ||
                     (! isInside (dummypcklst->getGeometry (),
                                  tmppcklst->getParent ()->getGeometry ()))) {
                    tmppcklst->setParent (dummypcklst);
                }
            }
        }
    }

    // Associate packages to classes
    for (umlPackage * dummypcklst : packagelst) {
        for (umlClassNode * it : res) {
            if (isInside (dummypcklst->getGeometry (), it->geom)) {
                if ((it->package == NULL) ||
                     (! isInside (dummypcklst->getGeometry (),
                                  it->package->getGeometry ()))) {
                    it->package = dummypcklst;
                }
            }
        }
    }

    xmlFreeDoc (ptr);

    return true;
}

void
umlClass::parseClass (xmlNodePtr class_) {
    xmlNodePtr attribute;

    package = nullptr;

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
            parseGeomPosition (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("elem_width", BAD_TSAC2 (attrname))) {
            parseGeomWidth (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("elem_height", BAD_TSAC2 (attrname))) {
            parseGeomHeight (attribute->xmlChildrenNode, &geom);
        } else if (!strcmp ("comment", BAD_TSAC2 (attrname)))  {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
               parseDiaNode (attribute->xmlChildrenNode, comment);
            }  else {
               comment.clear ();
            }
        } else if (!strcmp ("stereotype", BAD_TSAC2 (attrname))) {
            if (attribute->xmlChildrenNode->xmlChildrenNode != NULL) {
                std::string stereotype;

                parseDiaNode (attribute->xmlChildrenNode, stereotype);

                stereotypeTypedef = isTypedefStereo (stereotype);
                stereotypeEnum = isEnumStereo (stereotype);
                stereotypeConst = isConstStereo (stereotype);
                stereotypeStruct = isStructStereo (stereotype);
                stereotypeGetSet = isGetSetStereo (stereotype);
                stereotypeExtern = isInside (stereotype, "extern");
                stereotypeInterface = isInterfaceStereo (stereotype);
                stereotypeDllExport = isDllExportStereo (stereotype);
#ifdef ENABLE_CORBA
                stereotypeCorba = isCorbaStereo (stereotype);
#endif
            }
        } else if (!strcmp ("abstract", BAD_TSAC2 (attrname))) {
            abstract = parseBoolean (attribute->xmlChildrenNode);
        } else if (!strcmp ("attributes", BAD_TSAC2 (attrname))) {
            parseAttributes (attribute->xmlChildrenNode, attributes);
        } else if (!strcmp ("operations", BAD_TSAC2 (attrname))) {
            umlOperation::parseOperations (attribute->xmlChildrenNode,
                                            operations);
            if (stereotypeGetSet) {
                makeGetSetMethods ();
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
