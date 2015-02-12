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

#ifndef UMLCLASSNODE_HPP
#define UMLCLASSNODE_HPP

#include "config.h"

#include "decls.hpp"
#include "umlClass.hpp"
#include "umlAssoc.hpp"

class umlClassNode : public umlClass {
    private :
        std::list <std::pair <umlClass *, Visibility> > parents;
        std::list <umlassoc> associations;
        std::list <umlClassNode> classDep;
        std::list <umlPackage> packageDep;
    public :
        static umlClassNode * find (std::list <umlClassNode> & list,
                                    const char *id);
        umlClassNode ();
        umlClassNode (const umlClassNode & classnode);
        umlClassNode (umlClass * _key,
                      std::list <std::pair <umlClass *,
                                            Visibility> > & parents_,
                      std::list <umlassoc> & associations_,
                      std::list <umlClassNode> & classDep_,
                      std::list <umlPackage> & packageDep_);
        umlClassNode (umlClass & _key);

        const std::list <std::pair <umlClass *, Visibility> > &
                                                           getParents () const;
        const std::list <umlassoc> & getAssociations () const;
        const std::list <umlClassNode> & getDependencies () const;

        declaration * findClass (std::list <declaration> & decl) const;
        void addParent (umlClass * key, Visibility inh);
        void addDependency (umlClassNode & dependent);
        void addAggregate (const char *name,
                           char composite,
                           umlClassNode & base,
                           const char *multiplicity,
                           Visibility visibility);

        ~umlClassNode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
