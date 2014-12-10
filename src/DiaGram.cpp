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

#include "DiaGram.hpp"
#include "scan_tree.hpp"

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    invertsel (false),
    usecorba (false),
    tmp_classes (),
    includes (),
    decl (){
}


std::list <umlClassNode> &
DiaGram::getUml () {
    return uml;
}


void
DiaGram::addGenClasses (std::list <std::string> classes) {
    genClasses.merge (classes);

    return;
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

    return;
}


bool
DiaGram::getUseCorba () const {
    return usecorba;
}


void
DiaGram::setUseCorba (bool corba) {
    usecorba = corba;

    return;
}


/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
void
DiaGram::list_classes(umlClassNode & current_class, std::list <umlClassNode> & res) {
    std::list <umlClassNode> classes = getUml ();
    umlClassNode * tmpnode;

    for (const umlAttribute & umla : current_class.getAttributes ()) {
        if (!umla.getType ().empty ()) {
            tmpnode = find_by_name(classes, umla.getType ().c_str ());
            if ( tmpnode && ! find_by_name(res, umla.getType ().c_str ())) {
                res.push_back (*tmpnode);
            }
        }
    }

    for (const umlOperation & umlo : current_class.getOperations ()) {
        tmpnode = find_by_name(classes, umlo.getType ().c_str ());
        if ( tmpnode && ! find_by_name(res, umlo.getType ().c_str ())) {
            res.push_back (*tmpnode);
        }
        for (const umlAttribute & umla : umlo.getParameters ()) {
            tmpnode = find_by_name(classes, umla.getType ().c_str ());
            if ( tmpnode && ! find_by_name(res, umla.getType ().c_str ())) {
                res.push_back (*tmpnode);
            }
        }
    }

    for (const umlClassNode & classit : current_class.getParents ()) {
        tmpnode = find_by_name(classes, classit.getName().c_str ());
        if ( tmpnode && ! find_by_name(res, classit.getName().c_str ()) ) {
            res.push_back (*tmpnode);
        }
    }

    for (const umlClassNode & classit : current_class.getDependencies ()) {
        tmpnode = find_by_name(classes, classit.getName().c_str ());
        if ( tmpnode && ! find_by_name(res, classit.getName().c_str ()) ) {
            res.push_back (*tmpnode);
        }
    }

    for (const umlassoc & associations : current_class.getAssociations ()) {
        tmpnode = find_by_name(classes, associations.key.getName().c_str ());
        if ( tmpnode && ! find_by_name(res, associations.key.getName().c_str ()) ) {
            res.push_back (*tmpnode);
        }
    }

    return;
}

void
DiaGram::push (umlClassNode & node)
{
    std::list <umlClassNode> used_classes;
    declaration d;

    if (node.find_class (decl) != NULL) {
        return;
    }

    tmp_classes.push_back (node.getName());
    
    list_classes (node, used_classes);
    /* Make sure all classes that this one depends on are already pushed. */
    for (umlClassNode & it : used_classes) {
        /* don't push this class !*/
        if (! !node.getName().compare (it.getName()) &&
            ! (is_present (tmp_classes, it.getName().c_str ()) ^ invertsel)) {
            push (it);
        }
    }

    d.decl_kind = dk_class;
    d.u.this_class = new umlClassNode (node);

    if (node.getPackage() != NULL) {
        std::list <umlPackage> pkglist;
        module *m;
        umlPackage::make_package_list (node.getPackage(), pkglist);
        m = find_or_add_module (decl, pkglist);
        m->contents.push_back (d);
    } else {
        decl.push_back (d);
    }

    if (node.getStereotype().compare (0, 5, "CORBA") == 0)
        usecorba = true;
}


int
DiaGram::have_include (const char *name) const
{
    for (std::string inc : includes) {
        if (!inc.compare (name)) {
            return 1;
        }
    }
    return 0;
}

void
DiaGram::add_include (const char *name)
{
    if (have_include (name))
        return;
    
    includes.push_back (name);
}

void
DiaGram::push_include (umlClassNode &node)
{
    if (node.getPackage() != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node.getPackage(), pkglist);
        add_include ((*pkglist.begin ()).getName ().c_str ());
    } else {
        add_include (node.getName().c_str ());
    }
}

std::list <std::string>
DiaGram::getIncludes () const {
    return includes;
}


void
DiaGram::cleanIncludes () {
    includes.clear ();
}

void
DiaGram::determine_includes (declaration &d)
{
    if (d.decl_kind == dk_module) {
        for (declaration & it : d.u.this_module->contents) {
            determine_includes (it);
        }
    } else {
        std::list <umlClassNode> cl;
        list_classes (*d.u.this_class, cl);
        for (umlClassNode & it : cl) {
            push_include (it);
        }
    }
}

std::list <declaration>::iterator
DiaGram::getDeclBegin ()
{
    return decl.begin ();
}

std::list <declaration>::iterator
DiaGram::getDeclEnd ()
{
    return decl.end ();
}

DiaGram::~DiaGram () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
