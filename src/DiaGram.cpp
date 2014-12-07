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

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    invertsel (false),
    usecorba (false),
    tmp_classes (),
    includes (),
    decl (){
}


std::list <umlclassnode> &
DiaGram::getUml () {
    return uml;
}


void
DiaGram::addGenClasses (std::list <std::string> classes) {
    genClasses.merge (classes);

    return;
}


std::list <std::string>
DiaGram::getGenClasses () {
    return genClasses;
}


bool
DiaGram::getInvertSel () {
    return invertsel;
}


void
DiaGram::setInvertSel (bool invert) {
    invertsel = invert;

    return;
}


bool
DiaGram::getUseCorba () {
    return usecorba;
}


void
DiaGram::setUseCorba (bool corba) {
    usecorba = corba;

    return;
}


/* Returns a list with all the classes declared in the diagram */
void
DiaGram::scan_tree_classes (std::list <std::string> &res) {
    for (umlclassnode & it : uml) {
        res.push_back (it.key.name);
    }
    
    return;
}


/* Creates a new umlclassnode with class as the key, then appends it to
  the end of list */
void append ( std::list <umlclassnode> & list, umlclassnode & class_ ) {
    umlclassnode tmpnode;

    tmpnode.key = class_.key;
    tmpnode.parents = class_.parents;
    tmpnode.associations = class_.associations;
    tmpnode.dependencies = class_.dependencies;

    list.push_back (tmpnode);
}

/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
void
DiaGram::list_classes(umlclassnode & current_class, std::list <umlclassnode> & res) {
    std::list <umlclassnode> classes = getUml ();
    umlclassnode * tmpnode;

    for (umlAttribute & umla : current_class.key.attributes) {
        if (!umla.getType ().empty ()) {
            tmpnode = find_by_name(classes, umla.getType ().c_str ());
            if ( tmpnode && ! find_by_name(res, umla.getType ().c_str ())) {
                append(res, *tmpnode);
            }
        }
    }

    for (umlOperation & umlo : current_class.key.operations) {
        tmpnode = find_by_name(classes, umlo.getType ().c_str ());
        if ( tmpnode && ! find_by_name(res, umlo.getType ().c_str ())) {
            append(res, *tmpnode);
        }
        for (const umlAttribute & umla : umlo.getParameters ()) {
            tmpnode = find_by_name(classes, umla.getType ().c_str ());
            if ( tmpnode && ! find_by_name(res, umla.getType ().c_str ())) {
                append(res, *tmpnode);
            }
        }
    }

    for (umlclassnode & classit : current_class.parents) {
        tmpnode = find_by_name(classes, classit.key.name.c_str ());
        if ( tmpnode && ! find_by_name(res, classit.key.name.c_str ()) ) {
            append(res, *tmpnode);
        }
    }

    for (umlclassnode & classit : current_class.dependencies) {
        tmpnode = find_by_name(classes, classit.key.name.c_str ());
        if ( tmpnode && ! find_by_name(res, classit.key.name.c_str ()) ) {
            append(res, *tmpnode);
        }
    }

    for (umlassoc & associations : current_class.associations) {
        tmpnode = find_by_name(classes, associations.key.name.c_str ());
        if ( tmpnode && ! find_by_name(res, associations.key.name.c_str ()) ) {
            append(res, *tmpnode);
        }
    }

    return;
}

void
DiaGram::push (umlclassnode & node)
{
    std::list <umlclassnode> used_classes;
    module *m;
    declaration d;

    if (find_class (node, decl) != NULL) {
        return;
    }

    tmp_classes.push_back (node.key.name);
    
    list_classes (node, used_classes);
    /* Make sure all classes that this one depends on are already pushed. */
    for (umlclassnode & it : used_classes) {
        /* don't push this class !*/
        if (! !node.key.name.compare (it.key.name) &&
            ! (is_present (tmp_classes, it.key.name.c_str ()) ^ invertsel)) {
            push (it);
        }
    }

    d.decl_kind = dk_class;
    d.u.this_class = new umlclassnode;
    d.u.this_class->key = node.key;
    d.u.this_class->parents = node.parents;
    d.u.this_class->associations = node.associations;
    d.u.this_class->dependencies = node.dependencies;

    if (node.key.package != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node.key.package, pkglist);
        m = find_or_add_module (decl, pkglist);
        m->contents.push_back (d);
    } else {
        decl.push_back (d);
    }

    if (node.key.stereotype.compare (0, 5, "CORBA") == 0)
        usecorba = true;
}


int
DiaGram::have_include (const char *name)
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
DiaGram::push_include (umlclassnode &node)
{
    if (node.key.package != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node.key.package, pkglist);
        add_include ((*pkglist.begin ()).getName ().c_str ());
    } else {
        add_include (node.key.name.c_str ());
    }
}

std::list <std::string>
DiaGram::getIncludes () {
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
        std::list <umlclassnode> cl;
        list_classes (*d.u.this_class, cl);
        for (umlclassnode & it : cl) {
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
