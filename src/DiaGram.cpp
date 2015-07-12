/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2014-2014 Vincent Le Garrec

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
#include <cassert>
#include <algorithm>

#include "DiaGram.hpp"
#include "scan_tree.hpp"
#include "string2.hpp"

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    invertsel (false),
#ifdef ENABLE_CORBA
    usecorba (false),
#endif
    tmp_classes (),
    includes (),
    decl () {
}


std::list <umlClassNode> &
DiaGram::getUml () {
    return uml;
}


void
DiaGram::addGenClasses (std::list <std::string> classes) {
    genClasses.merge (classes);
}


std::list <std::string>
DiaGram::getGenClasses () const {
    return genClasses;
}


bool
DiaGram::getInvertSel () const {
    return invertsel;
}


void
DiaGram::setInvertSel (bool invert) {
    invertsel = invert;
}


#ifdef ENABLE_CORBA
bool
DiaGram::getUseCorba () const {
    return usecorba;
}


void
DiaGram::setUseCorba (bool corba) {
    usecorba = corba;
}
#endif


/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
void
DiaGram::listClasses (umlClassNode & current,
                      std::list <umlClassNode *> & resCla,
                      bool expandPackages) {
    umlClassNode * tmpnode;

    // Type may be unknown.
    for (const umlAttribute & umla : current.getAttributes ()) {
        if (!umla.getType ().empty ()) {
            tmpnode = findByName (uml, umla.getType ());
            if (tmpnode && ! findByName (resCla, umla.getType ())) {
                resCla.push_back (tmpnode);
            }
        }
    }

    for (const umlOperation & umlo : current.getOperations ()) {
        tmpnode = findByName (uml, umlo.getType ());
        if (tmpnode && ! findByName (resCla, umlo.getType ())) {
            resCla.push_back (tmpnode);
        }
        for (const umlAttribute & umla : umlo.getParameters ()) {
            tmpnode = findByName (uml, umla.getType ());
            if (tmpnode && !findByName (resCla, umla.getType ())) {
                resCla.push_back (tmpnode);
            }
        }
    }

    // But not parents, dependencies and associations.
    for (const std::pair <umlClass *, Visibility> classit :
                                                 current.getParents ()) {
        tmpnode = findByName (uml, classit.first->getName ());
        assert (tmpnode != NULL);
        if (!findByName (resCla, classit.first->getName ())) {
            resCla.push_back (tmpnode);
        }
    }

    for (const umlClassNode & classit : current.getDependencies ()) {
        tmpnode = findByName (uml, classit.getName ());
        assert (tmpnode != NULL);
        if (!findByName (resCla, classit.getName ())) {
            resCla.push_back (tmpnode);
        }
    }

    for (const umlassoc & associations : current.getAssociations ()) {
        tmpnode = findByName (uml, associations.key->getName ());
        assert (tmpnode != NULL);
        if (!findByName (resCla, associations.key->getName ())) {
            resCla.push_back (tmpnode);
        }
    }

    if (expandPackages) {
        for (umlClassNode & umlc : uml) {
            umlPackage * parent = umlc.getPackage ();
            while (parent != NULL) {
                for (const umlPackage *umlp : current.getDependenciesPack ()) {
                    if (umlp == parent) {
                        if (!findByName (resCla, umlc.getName ())) {
                            resCla.push_back (&umlc);
                        }
                        parent = nullptr;
                        break;
                    }
                }
                if (parent != nullptr) {
                    parent = parent->getParent ();
                }
            }
        }
    }

    return;
}

module *
createNestedModulesFromPkglist (const std::list <umlPackage*>::iterator &debut,
                                const std::list <umlPackage*>::iterator &fin,
                                module *m) {
    bool first = true;
    std::list <umlPackage *>::iterator it;
    assert (m != NULL);
    /* Expects pkglist and m to be non-NULL and m->contents to be NULL.
       Returns a reference to the innermost module created.  */
    for (it = debut; it != fin; ++it) {
        if (first) {
            first = false;
            continue;
        }
        declaration d;
        d.decl_kind = dk_module;
        d.u.this_module = new module;
        m->contents.push_back (d);
        m = d.u.this_module;
        m->pkg = *it;
    }
    return m;
}

module *
findOrAddModule (std::list <declaration> &dptr,
                 const std::list <umlPackage *>::iterator &debut,
                 const std::list <umlPackage *>::iterator &fin) {
    declaration d;
    module *m;

    if (debut == fin) {
        return NULL;
    }
    
    for (declaration & it : dptr) {
        if (it.decl_kind == dk_module &&
            it.u.this_module->pkg->getName ().compare (
                                                  (*debut)->getName ()) == 0) {
            m = it.u.this_module;
            if (std::next (debut) == fin) {
                return m;
            }
            if (m->contents.empty ()) {
                return createNestedModulesFromPkglist (debut, fin, m);
            }
            return findOrAddModule (m->contents, std::next (debut), fin);
        }
    }
    d.decl_kind = dk_module;
    d.u.this_module = new module;
    m = d.u.this_module;
    m->pkg = *debut;
    dptr.push_back (d);

    return createNestedModulesFromPkglist (debut, fin, m);
}

void
DiaGram::cleanTmpClasses () {
    std::cout << "clean" << tmp_classes.size () << "\n";
    for (umlClassNode * it : tmp_classes) {
        for (umlClassNode * it2 : tmp_classes) {
            if (it != it2) {
                std::cout << it->getName () << " " << it2->getName () << "\n";
                it->addCircularLoop (it2);
            }
        }
    }

    tmp_classes.clear ();
}

std::list <umlClassNode *> &
DiaGram::getTmpClasses () {
    return tmp_classes;
}

void
DiaGram::push (umlClassNode & node) {
    std::list <umlClassNode *> usedClasses;
    declaration d;

    if (node.findClass (decl) != NULL) {
        return;
    }

    tmp_classes.push_back (&node);
    node.setPushed ();

    listClasses (node, usedClasses, true);
    // Make sure all classes that this one depends on are already pushed.
    for (umlClassNode * it : usedClasses) {
        // don't push this class
        if ((!node.isPushed ()) &&
            ((genClasses.empty ()) ||
             (isPresent (genClasses,
                         it->getName ().c_str ()) ^ getInvertSel ())) &&
            (find_if (tmp_classes.begin (),
                      tmp_classes.end (),
                      [&it](umlClassNode * it2)
                      {
                        return it2->getName ().compare (it->getName ()) == 0;
                      }) == tmp_classes.end ())) {
            push (*it);
        }
    }

    d.decl_kind = dk_class;
    d.u.this_class = new umlClassNode (node);

    if (node.getPackage () != NULL) {
        std::list <umlPackage *> pkglist;
        module *m;

        umlPackage::makePackageList (node.getPackage (), pkglist);
        m = findOrAddModule (decl, pkglist.begin (), pkglist.end ());
        m->contents.push_back (d);
    } else {
        decl.push_back (d);
    }

#ifdef ENABLE_CORBA
    if (node.getStereotype ().compare (0, 5, "CORBA") == 0) {
        usecorba = true;
    }
#endif
}

bool
DiaGram::haveInclude (std::list <umlPackage *> & packages,
                      umlClassNode * cla) const
{
    for (std::pair <std::list <umlPackage *>,
                    umlClassNode * > inc : includes) {
        bool idem = true;
        std::list <umlPackage *>::const_iterator namei = packages.begin ();

        if ((inc.first.size () != packages.size ()) || (cla != inc.second)) {
            continue;
        }
        for (umlPackage * inc2 : inc.first) {
            if (inc2->getId ().compare ((*namei)->getId ()) != 0) {
                idem = false;
                break;
            }
            ++namei;
        }
        if (idem) {
            return true;
        }
    }
    return false;
}

void
DiaGram::addInclude (std::list <umlPackage *> & packages,
                     umlClassNode * cla) {
    if (haveInclude (packages, cla)) {
        return;
    }

    includes.push_back (make_pair (packages, cla));
}

void
DiaGram::pushInclude (const umlClassNode & node) {
    std::list <umlPackage *> pkglist;

    if (node.getPackage () != NULL) {
        umlPackage::makePackageList (node.getPackage (), pkglist);
    }
    addInclude (pkglist, new umlClassNode (node));
}

void
DiaGram::pushInclude (umlPackage * node) {
    std::list <umlPackage *> pkglist;

    umlPackage::makePackageList (node, pkglist);
    addInclude (pkglist, nullptr);
}

const std::list <std::pair <std::list <umlPackage *>, umlClassNode * > > &
DiaGram::getIncludes () const {
    return includes;
}

void
DiaGram::cleanIncludes () {
    for (std::pair <std::list <umlPackage *>, umlClassNode * > & it :
                                                                    includes) {
        delete it.second;
    }
    includes.clear ();
}

void
DiaGram::determineIncludes (declaration &d,
                            bool expandPackages) {
    if (d.decl_kind == dk_module) {
        for (declaration & it : d.u.this_module->contents) {
            determineIncludes (it, expandPackages);
        }
    } else {
        std::list <umlClassNode *> cl;

        listClasses (*d.u.this_class, cl, expandPackages);
        for (const umlClassNode * it : cl) {
            pushInclude (*it);
        }
        
        for (umlPackage * it : d.u.this_class->getDependenciesPack ()) {
            pushInclude (it);
        }
    }
}

std::list <declaration>::iterator
DiaGram::getDeclBegin () {
    return decl.begin ();
}

std::list <declaration>::iterator
DiaGram::getDeclEnd () {
    return decl.end ();
}

DiaGram::~DiaGram () {
    for (declaration & d : decl) {
        if (d.decl_kind == dk_class) {
            delete d.u.this_class;
        }
        else if (d.decl_kind == dk_module) {
            delete d.u.this_module;
        }
    }
    cleanIncludes ();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
