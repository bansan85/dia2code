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
#include <memory>

#include "DiaGram.hpp"
#include "scan_tree.hpp"

#ifdef ENABLE_CORBA
bool DiaGram::usecorba = false;  // static
#endif

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    invertsel (false),
    tmp_classes (),
    includes (),
    decl () {
}


std::list <umlClassNode *> &
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
DiaGram::getUseCorba () {  // static
    return usecorba;
}
#endif


/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
void
DiaGram::listClasses (umlClassNode & current,
                      std::list <umlClassNode *> & resCla,
                      uint8_t flag) {
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
    for (const umlClassNode::ClassAndVisibility& classit :
                                                 current.getParents ()) {
        tmpnode = findByName (uml, classit.first->getName ());
        assert (tmpnode != NULL);
        if (!findByName (resCla, classit.first->getName ())) {
            resCla.push_back (tmpnode);
        }
    }

    for (const umlClassNode::ClassNodeAndFlags & classit :
                                                 current.getDependencies ()) {
        if (!(((classit.second & 1) == 1) && ((flag & 2) == 2))) {
            tmpnode = findByName (uml, classit.first->getName ());
            assert (tmpnode != NULL);
            if (!findByName (resCla, classit.first->getName ())) {
                resCla.push_back (tmpnode);
            }
        }
    }

    for (const umlassoc & associations : current.getAssociations ()) {
        tmpnode = findByName (uml, associations.key->getName ());
        assert (tmpnode != NULL);
        if (!findByName (resCla, associations.key->getName ())) {
            resCla.push_back (tmpnode);
        }
    }

    if ((flag & 1) == 1) {
        for (umlClassNode * umlc : uml) {
            umlPackage * parent = umlc->getPackage ();
            while (parent != NULL) {
                for (const umlPackage *umlp : current.getDependenciesPack ()) {
                    if (umlp == parent) {
                        if (!findByName (resCla, umlc->getName ())) {
                            resCla.push_back (umlc);
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

void DiaGram::pushTmp (umlClassNode * node) {
    tmp_classes.push_back (node);
}

void DiaGram::popTmp () {
    tmp_classes.pop_back ();
}

void
DiaGram::push (umlClassNode * node) {
    std::list <umlClassNode *> usedClasses;
    declaration d;

    if (node->findClass (decl) != NULL) {
        return;
    }

    pushTmp (node);

    listClasses (*node, usedClasses, 1);
    // Make sure all classes that this one depends on are already pushed.
    for (umlClassNode * it : usedClasses) {
        // don't push this class
        if (!it->isPushed ()) {
            // Circular dependency
            std::list <umlClassNode *>::iterator it2;
            it2 = find (tmp_classes.begin (),
                       tmp_classes.end (),
                       it);
            if (it2 != tmp_classes.end ()) {
                auto it3 = it2;
                std::cout << "Loop detected : ";
                while (it3 != tmp_classes.end ()) {
                    std::cout << (*it3)->getName () << ", ";
                    auto it4 = it2;
                    while (it4 != tmp_classes.end ()) {
                        if (it3 != it4)
                            (*it3)->addCircularLoop (*it4);
                        ++it4;
                    }
                    ++it3;
                }
                std::cout << (*it2)->getName () << ".\n";
            }
            else {
                push (it);
            }
        }
    }

    node->setPushed ();
    popTmp ();

    d.decl_kind = dk_class;
    d.u.this_class = node;

    if (node->getPackage () != NULL) {
        std::list <umlPackage *> pkglist;
        module *m;

        umlPackage::makePackageList (node->getPackage (), pkglist);
        m = findOrAddModule (decl, pkglist.begin (), pkglist.end ());
        m->contents.push_back (d);
    } else {
        decl.push_back (d);
    }

#ifdef ENABLE_CORBA
    if (node->isStereotypeCorba ()) {
        usecorba = true;
    }
#endif
}

bool
DiaGram::haveInclude (std::list <umlPackage *> & packages,
                      const umlClassNode * cla) const
{
    for (std::pair <std::list <umlPackage *>,
                    const umlClassNode * > inc : includes) {
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
                     const umlClassNode * cla) {
    if (haveInclude (packages, cla)) {
        return;
    }

    includes.push_back (make_pair (packages, cla));
}

void
DiaGram::pushInclude (const umlClassNode * node) {
    std::list <umlPackage *> pkglist;

    if (node->getPackage () != NULL) {
        umlPackage::makePackageList (node->getPackage (), pkglist);
    }
    addInclude (pkglist, node);
}

void
DiaGram::pushInclude (umlPackage * node) {
    std::list <umlPackage *> pkglist;

    umlPackage::makePackageList (node, pkglist);
    addInclude (pkglist, nullptr);
}

const std::list <std::pair <std::list <umlPackage *>, const umlClassNode *> > &
DiaGram::getIncludes () const {
    return includes;
}

void
DiaGram::cleanIncludes () {
    includes.clear ();
}

void
DiaGram::determineIncludes (declaration &d,
                            bool expandPackages,
                            bool noLoop) {
    if (d.decl_kind == dk_module) {
        for (declaration & it : d.u.this_module->contents) {
            determineIncludes (it, expandPackages, noLoop);
        }
    } else {
        std::list <umlClassNode *> cl;
        uint8_t flag = 0;

        if (expandPackages) {
            flag = flag | 1;
        }
        if (noLoop) {
            flag = flag | 2;
        }

        listClasses (*d.u.this_class, cl, flag);
        for (const umlClassNode * it : cl) {
            pushInclude (it);
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
        if (d.decl_kind == dk_module) {
            delete d.u.this_module;
        }
    }
    cleanIncludes ();
    std::for_each (uml.begin (),
                   uml.end (),
                   std::default_delete <umlClassNode> ());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
