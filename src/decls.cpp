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

module *
create_nested_modules_from_pkglist (std::list <umlpackage> &pkglist,
                                    module        *m)
{
    std::list <umlpackage>::iterator it = std::next (pkglist.begin ());
    /* Expects pkglist and m to be non-NULL and m->contents to be NULL.
       Returns a reference to the innermost module created.  */
    while (it != pkglist.end ()) {
        declaration d;
        d.decl_kind = dk_module;
        d.u.this_module = new module;
        m->contents.push_back(d);
        m = d.u.this_module;
        m->pkg = *it;
        ++it;
    }
    return m;
}


module *
find_or_add_module (std::list <declaration> &dptr,
                    std::list <umlpackage> &pkglist)
{
    declaration d;
    std::list <declaration>::iterator it;
    module *m;

    if (pkglist.empty ())
        return NULL;
    
    it = dptr.begin ();
    while (it != dptr.end ()) {
        if ((*it).decl_kind == dk_module &&
            (*it).u.this_module->pkg.name.compare ((*pkglist.begin ()).name) == 0) {
            m = (*it).u.this_module;
            if (pkglist.size () == 1)
                return m;
            if (m->contents.empty ()) {
                return create_nested_modules_from_pkglist (pkglist, m);
            }
            return find_or_add_module (m->contents, pkglist);
        }
        ++it;
    }
    d.decl_kind = dk_module;
    d.u.this_module = new module;
    m = d.u.this_module;
    m->pkg = *pkglist.begin ();
    dptr.push_back (d);

    return create_nested_modules_from_pkglist (pkglist, m);
}


module *
find_module (std::list <declaration> &dptr,
             std::list <umlpackage>::iterator begin,
             std::list <umlpackage>::iterator end)
{
    std::list <declaration>::iterator it = dptr.begin ();
    while (it != dptr.end ()) {
        if ((*it).decl_kind == dk_module) {
            module *m = (*it).u.this_module;
            if (m->pkg.name.compare ((*begin).name) == 0) {
                if (std::next (begin) != end)
                {
                    std::list<declaration> liste;

                    if (m->contents.empty ())
                        return NULL;
                    else
                    {
                        liste.push_back (*m->contents.begin ());
                        return find_module (liste, std::next (begin), end);
                    }
                }
                else
                    return m;
            }
        }
        ++it;
    }
    return NULL;
}


declaration *
find_class (umlclassnode &node, std::list <declaration> &decl)
{
    std::list <declaration> *d;
    std::list <declaration>::iterator it;

    if (node.key.package) {
        std::list <umlpackage> pkglist;
        make_package_list (node.key.package, pkglist);
        module *m = find_module (decl, pkglist.begin (), pkglist.end ());
        if (m == NULL || m->contents.empty ())
            return 0;
        d = &m->contents;
    } else {
        d = &decl;
    }

    it = d->begin ();
    while (it != d->end ()) {
        if ((*it).decl_kind == dk_class) {
            umlclassnode *cl = (*it).u.this_class;
            if (cl->key.name.compare (node.key.name) == 0)
                return (&*it);
        }
        ++it;
    }
    return NULL;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
