/*
Copyright (C) 2014-2014 Vincent Le Garrec <legarrec.vincent@gmail.com>

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

#include "config.h"

#include "DiaGram.hpp"

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    invertsel (false),
    usecorba (false),
    tmp_classes (),
    includes () {
}


umlclasslist
DiaGram::getUml () {
    return uml;
}


void
DiaGram::setUml (umlclasslist diagram) {
    uml = diagram;

    return;
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
std::list <std::string>
DiaGram::scan_tree_classes () {
    std::list <std::string> result;
    umlclasslist tmplist = uml;
    
    while ( tmplist != NULL ) {
        result.push_back (tmplist->key->name);
        tmplist = tmplist->next;
    }
    
    return result;
}


/* Creates a new umlclassnode with class as the key, then appends it to
  the end of list */
umlclasslist append ( umlclasslist list, umlclassnode * class_ ) {
    umlclasslist tmplist = list;
    umlclassnode *tmpnode = NULL;

    tmpnode = new umlclassnode;
    tmpnode->key = class_->key;
    tmpnode->parents = class_->parents;
    tmpnode->associations = class_->associations;
    tmpnode->dependencies = class_->dependencies;
    tmpnode->next = NULL;

    if ( tmplist != NULL ) {
        while ( tmplist->next != NULL ) {
            tmplist = tmplist->next;
        }
        tmplist->next = tmpnode;
        return list;
    } else {
        return tmpnode;
    }
}

/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
umlclasslist
DiaGram::list_classes(umlclasslist current_class) {
    umlclasslist parents, dependencies;
    umlassoclist associations;
    std::list <umlattribute>::iterator umla;
    std::list <umloperation>::iterator umlo;
    umlclasslist result = NULL;
    umlclasslist classes = uml;
    umlclasslist tmpnode = NULL;

    umla = current_class->key->attributes.begin ();
    while (umla != current_class->key->attributes.end ()) {
        if (!(*umla).type.empty ()) {
            tmpnode = find_by_name(classes, (*umla).type.c_str ());
            if ( tmpnode && ! find_by_name(result, (*umla).type.c_str ())) {
                result = append(result, tmpnode);
            }
        }
        ++umla;
    }

    umlo = current_class->key->operations.begin ();
    while ( umlo != current_class->key->operations.end ()) {
        tmpnode = find_by_name(classes, (*umlo).attr.type.c_str ());
        if ( tmpnode && ! find_by_name(result, (*umlo).attr.type.c_str ())) {
            result = append(result, tmpnode);
        }
        umla = (*umlo).parameters.begin ();
        while (umla != (*umlo).parameters.end ()) {
            tmpnode = find_by_name(classes, (*umla).type.c_str ());
            if ( tmpnode && ! find_by_name(result, (*umla).type.c_str ())) {
                result = append(result, tmpnode);
            }
            ++umla;
        }
        ++umlo;
    }

    parents = current_class->parents;
    while ( parents != NULL ) {
        tmpnode = find_by_name(classes, parents->key->name.c_str ());
        if ( tmpnode && ! find_by_name(result, parents->key->name.c_str ()) ) {
            result = append(result, tmpnode);
        }
        parents = parents->next;
    }

    dependencies = current_class->dependencies;
    while (dependencies != NULL) {
        tmpnode = find_by_name(classes, dependencies->key->name.c_str ());
        if ( tmpnode && ! find_by_name(result, dependencies->key->name.c_str ()) ) {
            result = append(result, tmpnode);
        }
        dependencies = dependencies->next;
    }

    associations = current_class->associations;
    while (associations != NULL) {
        tmpnode = find_by_name(classes, associations->key->name.c_str ());
        if ( tmpnode && ! find_by_name(result, associations->key->name.c_str ()) ) {
            result = append(result, tmpnode);
        }
        associations = associations->next;
    }

    return result;

}

void
DiaGram::push (umlclassnode *node)
{
    umlclasslist used_classes, tmpnode;
    module *m;
    declaration *d;

    if (node == NULL || find_class (node) != NULL) {
        return;
    }

    tmp_classes.push_back (node->key->name);
    
    used_classes = list_classes (node);
    /* Make sure all classes that this one depends on are already pushed. */
    tmpnode = used_classes;
    while (tmpnode != NULL) {
        /* don't push this class !*/
        if (! !node->key->name.compare (tmpnode->key->name) &&
            ! (is_present (tmp_classes, tmpnode->key->name.c_str ()) ^ invertsel)) {
            push (tmpnode);
        }
        tmpnode = tmpnode->next;
    }

    while (used_classes != NULL) {
        tmpnode = used_classes->next;
        delete used_classes;
        used_classes = tmpnode;
    }

    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        m = find_or_add_module (&decls, pkglist);
        if (m->contents == NULL) {
            m->contents = new declaration;
            d = m->contents;
            d->prev = NULL;
        } else {
            /* We can simply append because all classes that we depend on
               are already pushed. */
            d = append_decl (m->contents);
        }
    } else {
        if (decls == NULL) {
            decls = new declaration;
            d = decls;
            d->prev = NULL;
        } else {
            d = append_decl (decls);
            /* We can simply append because all classes that we depend on
               are already pushed. */
        }
    }
    d->decl_kind = dk_class;
    d->next = NULL;
    d->u.this_class = new umlclassnode;
    memcpy (d->u.this_class, node, sizeof(umlclassnode));
    if (node->key->stereotype.compare (0, 5, "CORBA") == 0)
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
DiaGram::push_include (umlclassnode *node)
{
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        add_include (pkglist->key->name.c_str ());
    } else {
        add_include (node->key->name.c_str ());
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
DiaGram::determine_includes (declaration *d)
{
    if (d->decl_kind == dk_module) {
        declaration *inner = d->u.this_module->contents;
        while (inner != NULL) {
            determine_includes (inner);
            inner = inner->next;
        }
    } else {
        umlclasslist cl = list_classes (d->u.this_class);
        umlclasslist tmp = cl;
        while (cl != NULL) {
            push_include (cl);
            cl = cl->next;
        }
        cl = tmp;
        while (cl != NULL) {
            tmp = cl->next;
            delete cl;
            cl = tmp;
        }
    }
}

DiaGram::~DiaGram () {
    umlclasslist tmplist = uml;
    
    while ( tmplist != NULL ) {
        umlclasslist tmplist2 = tmplist;

        umlclasslist list2 = tmplist->parents;
        while (list2 != NULL) {
            umlclasslist list2_ = list2;
            list2 = list2->next;
            delete list2_;
        }

        umlassoclist list3 = tmplist->associations;
        while (list3 != NULL) {
            umlassoclist list3_ = list3;
            list3 = list3->next;
            delete list3_;
        }

        list2 = tmplist->dependencies;
        while (list2 != NULL) {
            umlclasslist list2_ = list2;
            list2 = list2->next;
            delete list2_;
        }

        tmplist = tmplist->next;
        delete tmplist2->key;
        delete tmplist2;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
