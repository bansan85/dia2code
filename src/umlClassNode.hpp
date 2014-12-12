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

#include "dia2code.hpp"
#include "decls.hpp"
#include "umlClass.hpp"
#include "umlAssoc.hpp"

class umlClassNode : public umlClass {
    private :
        std::list <umlClassNode> parents;
        std::list <umlassoc> associations;
        std::list <umlClassNode> dependencies;
    public :
        umlClassNode ();
        umlClassNode (const umlClassNode & classnode);
        umlClassNode (umlClass & _key,
                      std::list <umlClassNode> & parents_,
                      std::list <umlassoc> & associations_,
                      std::list <umlClassNode> & dependencies_);
        umlClassNode (umlClass & _key);
        
        const std::list <umlClassNode> & getParents () const;
        const std::list <umlassoc> & getAssociations () const;
        const std::list <umlClassNode> & getDependencies () const;
        
        declaration * find_class (std::list <declaration> & decl) const;
        void addparent (umlClass & key);
        void adddependency (umlClassNode & dependent);
        void addaggregate (const char *name,
                           char composite,
                           umlClassNode & base,
                           const char *multiplicity);
        
        ~umlClassNode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
