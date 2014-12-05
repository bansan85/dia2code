/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014

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

#include "decls.hpp"

module *
create_nested_modules_from_pkglist (std::list <umlpackage> &pkglist,
                                    module        *m)
{
    bool first = true;
    /* Expects pkglist and m to be non-NULL and m->contents to be NULL.
       Returns a reference to the innermost module created.  */
    for (umlpackage & it : pkglist) {
        if (first)
        {
            first = false;
            continue;
        }
        declaration d;
        d.decl_kind = dk_module;
        d.u.this_module = new module;
        m->contents.push_back(d);
        m = d.u.this_module;
        m->pkg = it;
    }
    return m;
}


module *
find_or_add_module (std::list <declaration> &dptr,
                    std::list <umlpackage> &pkglist)
{
    declaration d;
    module *m;

    if (pkglist.empty ())
        return NULL;
    
    for (declaration & it : dptr) {
        if (it.decl_kind == dk_module &&
            it.u.this_module->pkg.name.compare ((*pkglist.begin ()).name) == 0) {
            m = it.u.this_module;
            if (pkglist.size () == 1)
                return m;
            if (m->contents.empty ()) {
                return create_nested_modules_from_pkglist (pkglist, m);
            }
            return find_or_add_module (m->contents, pkglist);
        }
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

    for (declaration & it : *d) {
        if (it.decl_kind == dk_class) {
            umlclassnode *cl = it.u.this_class;
            if (cl->key.name.compare (node.key.name) == 0)
                return &it;
        }
    }
    return NULL;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
