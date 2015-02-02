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

#ifndef UMLOPERATION_HPP
#define UMLOPERATION_HPP

#include "config.h"

#include <string>
#include <list>
#include <libxml/tree.h>

#include "umlAttribute.hpp"

// public umlAttribute will avoid an private attribute + the get/set function.
class umlOperation : public umlAttribute {
    private :
        std::string implementation;
        std::list <umlAttribute> parameters;
    public :
        static void insert_operation (umlOperation &n,
                                      std::list <umlOperation> &l);
        static void parse_operations (xmlNodePtr node,
                                      std::list <umlOperation> &res);
        umlOperation (xmlNodePtr node);
        umlOperation (std::string name_,
                      std::string value_,
                      std::string type_,
                      std::string comment_,
                      Visibility visibility_,
                      Inheritance inheritance_,
                      unsigned char isstatic_,
                      unsigned char isconstant_,
                      Kind kind_,
                      std::string impl_);

        void addParameter (umlAttribute & attr);
        const std::list <umlAttribute> & getParameters () const;

        ~umlOperation ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
