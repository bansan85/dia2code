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

#include "umlClassNode.hpp"
#include "parse_diagram.hpp"

umlClassNode *
umlClassNode::find (std::list <umlClassNode> & list, const char *id) {
    if (id != NULL) {
        for (umlClassNode & it : list) {
            if (it.getId ().compare (id) == 0) {
                return &it;
            }
        }
    }
    return NULL;
}


umlClassNode::umlClassNode () :
    umlClass (),
    parents (),
    associations (),
    classDep (),
    packageDep ()
{
}

umlClassNode::umlClassNode (const umlClassNode & classnode) :
    umlClass (classnode),
    parents (classnode.parents),
    associations (classnode.associations),
    classDep (classnode.classDep),
    packageDep (classnode.packageDep)
{
}

umlClassNode::umlClassNode (umlClass * _key,
                            std::list <std::pair <umlClass *,
                                                  Visibility> > & parents_,
                            std::list <umlassoc> & associations_,
                            std::list <umlClassNode> & classDep_,
                            std::list <umlPackage *> & packageDep_) :
    umlClass (*_key),
    parents (parents_),
    associations (associations_),
    classDep (classDep_),
    packageDep (packageDep_)
{
}

umlClassNode::umlClassNode (umlClass & _key) :
    umlClass (_key),
    parents (),
    associations (),
    classDep (),
    packageDep ()
{
}

const std::list <std::pair <umlClass *, Visibility> > &
umlClassNode::getParents () const {
    return parents;
}

const std::list <umlassoc> &
umlClassNode::getAssociations () const {
    return associations;
}

const std::list <umlClassNode> &
umlClassNode::getDependencies () const {
    return classDep;
}

std::list <umlPackage *> &
umlClassNode::getDependenciesPack () {
    return packageDep;
}

module *
findModule (std::list <declaration> &dptr,
             std::list <umlPackage *>::iterator begin,
             std::list <umlPackage *>::iterator end) {
    std::list <declaration>::iterator it = dptr.begin ();
    while (it != dptr.end ()) {
        if ((*it).decl_kind == dk_module) {
            module *m = (*it).u.this_module;
            if (m->pkg->getName ().compare ((*begin)->getName ()) == 0) {
                if (std::next (begin) != end) {
                    std::list<declaration> liste;

                    if (m->contents.empty ()) {
                        return nullptr;
                    }
                    else {
                        liste.push_back (*m->contents.begin ());
                        return findModule (liste, std::next (begin), end);
                    }
                }
                else {
                    return m;
                }
            }
        }
        ++it;
    }
    return nullptr;
}

declaration *
umlClassNode::findClass (std::list <declaration> &decl) const {
    std::list <declaration> *d;

    if (getPackage ()) {
        std::list <umlPackage *> pkglist;

        umlPackage::makePackageList (getPackage (), pkglist);
        module *m = findModule (decl, pkglist.begin (), pkglist.end ());
        if (m == NULL || m->contents.empty ()) {
            return nullptr;
        }
        d = &m->contents;
    } else {
        d = &decl;
    }

    for (declaration & it : *d) {
        if (it.decl_kind == dk_class) {
            umlClassNode *cl = it.u.this_class;
            if (cl->getName ().compare (getName ()) == 0) {
                return &it;
            }
        }
    }
    return nullptr;
}

void
umlClassNode::addParent (umlClass * key, Visibility inh) {
    parents.push_front (std::make_pair (key, inh));
}

void
umlClassNode::addDependency (umlClassNode & dependent) {
    classDep.push_front (dependent);
}

void
umlClassNode::addDependency (umlPackage * dependent) {
    packageDep.push_front (dependent);
}

void
umlClassNode::addAggregate (const char *name_,
        char composite,
        umlClassNode & base,
        const char *multiplicity,
        Visibility visibility) {
    umlassoc tmp;
    if (name_ != NULL && strlen (name_) > 2) {
        parseDiaString(name_, tmp.name);
    }
    if (multiplicity != NULL) {
#if defined(_WIN32) || defined(_WIN64)
        strncpy_s (tmp.multiplicity,
                   10,
                   multiplicity + 1,
                   strlen (multiplicity) - 2);
#else
        strncpy (tmp.multiplicity,
                 multiplicity + 1,
                 strlen (multiplicity) - 2);
#endif
    }
    else {
        tmp.multiplicity[0] = '1';
        tmp.multiplicity[1] = 0;
    }
    tmp.key = &base;
    tmp.composite = composite;
    tmp.visibility = visibility;
    associations.push_front (tmp);
}

umlClassNode::~umlClassNode ()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
