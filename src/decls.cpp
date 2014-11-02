/*
Copyright (C) 2000-2014

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

#include "decls.hpp"


declaration *decls = NULL;


module *
create_nested_modules_from_pkglist (umlpackagelist pkglist,
                                    module        *m)
{
    /* Expects pkglist and m to be non-NULL and m->contents to be NULL.
       Returns a reference to the innermost module created.  */
    while (pkglist->next != NULL) {
        declaration *d = new declaration;
        d->decl_kind = dk_module;
        d->prev = d->next = NULL;
        d->u.this_module = new module;
        m->contents = d;
        pkglist = pkglist->next;
        m = d->u.this_module;
        m->pkg = pkglist->key;
        m->contents = NULL;
    }
    return m;
}


module *
find_or_add_module (declaration  **dptr,
                    umlpackagelist pkglist)
{
    declaration *d = *dptr;
    module *m;

    if (pkglist == NULL)
        return NULL;
    if (d == NULL) {
        *dptr = new declaration;
        d = *dptr;
    } else {
        declaration *dprev = NULL;
        while (d != NULL) {
            if (d->decl_kind == dk_module &&
                d->u.this_module->pkg->name.compare (pkglist->key->name) == 0) {
                m = d->u.this_module;
                if (pkglist->next == NULL)
                    return m;
                if (m->contents == NULL) {
                    return create_nested_modules_from_pkglist (pkglist, m);
                }
                return find_or_add_module (&m->contents, pkglist->next);
            }
            dprev = d;
            d = d->next;
        }
        if (dprev != NULL) {
            dprev->next = new declaration;
            d = dprev->next;
        }
    }
    d->decl_kind = dk_module;
    d->next = NULL;
    d->u.this_module = new module;
    m = d->u.this_module;
    m->pkg = pkglist->key;
    m->contents = NULL;
    return create_nested_modules_from_pkglist (pkglist, m);
}


module *
find_module (declaration   *d,
             umlpackagelist pkglist)
{
    while (d != NULL) {
        if (d->decl_kind == dk_module) {
            module *m = d->u.this_module;
            if (m->pkg->name.compare (pkglist->key->name) == 0) {
                if (pkglist->next != NULL)
                    return find_module (m->contents, pkglist->next);
                else
                    return m;
            }
        }
        d = d->next;
    }
    return NULL;
}


declaration *
find_class (umlclassnode *node)
{
    declaration *d;

    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        module *m = find_module (decls, pkglist);
        if (m == NULL || m->contents == NULL)
            return 0;
        d = m->contents;
    } else {
        d = decls;
    }

    while (d != NULL) {
        if (d->decl_kind == dk_class) {
            umlclassnode *cl = d->u.this_class;
            if (cl->key->name.compare (node->key->name) == 0)
                return d;
        }
        d = d->next;
    }
    return NULL;
}


declaration *
append_decl (declaration *d)
{
    while (d->next != NULL) {
        d = d->next;
    }
    d->next = new declaration;
    d->next->prev = d;
    d = d->next;
    return d;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
