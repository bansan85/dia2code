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
    public :
        typedef std::pair <umlClass *, Visibility> ClassAndVisibility;
        typedef std::pair <umlClassNode *, uint8_t> ClassNodeAndFlags;
    private :
        std::list <ClassAndVisibility> parents;
        std::list <umlassoc> associations;
        // bit 0 : 1 si NoLoop
        std::list <ClassNodeAndFlags> classDep;
        std::list <umlPackage *> packageDep;
    public :
        static umlClassNode * find (std::list <umlClassNode *> & list,
                                    const char *id);
        umlClassNode ();
        umlClassNode (const umlClassNode & classnode);
        umlClassNode (umlClass * _key,
                      std::list <ClassAndVisibility> & parents_,
                      std::list <umlassoc> & associations_,
                      std::list <ClassNodeAndFlags> & classDep_,
                      std::list <umlPackage *> & packageDep_);
        umlClassNode (umlClass & _key);

        const std::list <ClassAndVisibility> & getParents () const;
        const std::list <umlassoc> & getAssociations () const;
        const std::list <ClassNodeAndFlags> & getDependencies () const;
        std::list <umlPackage *> & getDependenciesPack ();

        declaration * findClass (std::list <declaration> & decl) const;
        void addParent (umlClass * key, Visibility inh);
        void addDependency (umlClassNode * dependent, uint8_t flag);
        void addDependency (umlPackage * dependent);
        void addAggregate (const char *name,
                           char composite,
                           umlClassNode * base,
                           const char *multiplicity,
                           Visibility visibility);

        ~umlClassNode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
