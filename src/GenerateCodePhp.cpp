/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2015 Vincent Le Garrec

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

#include <iostream>

#include "GenerateCodePhp.hpp"
#include "umlClassNode.hpp"
#include "string2.hpp"

GenerateCodePhp::GenerateCodePhp (DiaGram & diagram) :
    GenerateCodeJava (diagram) {
    setFileExt ("php");
    setHandleIncludePackage (false);
}

void
GenerateCodePhp::writeStartHeader (std::string & name) {
    getFile () << "<?php" << std::endl;
}

void
GenerateCodePhp::writeEndHeader () {
    getFile () << "?>\n";
}

bool
GenerateCodePhp::writeInclude (const std::list <std::pair <
                                                   std::list <umlPackage *>,
                                                   umlClassNode * > > & name) {
    return writeInclude1 (name,
                          "require_once '",
                          "';\n",
                          "require_once '",
                          "';\n");
}

void
GenerateCodePhp::writeInclude (const char * name) {
    getFile () << spc () << "require_once '" << name << "';\n";
}

void
GenerateCodePhp::writeFunctionComment (const umlOperation & ope) {
    writeFunctionComment1 (ope, true, '$');
}

void
GenerateCodePhp::writeFunction (const umlClassNode & node,
                                const umlOperation & ope,
                                Visibility & curr_visibility) {
    writeFunction1 (node, ope, curr_visibility);

    getFile () << spc ();
    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::FINAL) {
        std::cerr << "Class \"" << node.getName () << "\", operation \""
                  << ope.getName ()
                  << "\": this generator doesn't support final operation."
                  << " Default: virtual." << std::endl;
    }
    getFile () << visibility ("Class, \"" + node.getName () +
                                    "\", operation \"" + ope.getName () + "\"",
                              ope.getVisibility ()) << " ";

    getFile () << "function ";
    writeFunction2 (node, ope, curr_visibility, false);
}

void
GenerateCodePhp::writeClassStart (const umlClassNode & node) {
    writeClassStart1 (node, " extends ", true, false);
}

void
GenerateCodePhp::writeAttributeComment (const umlAttribute & attr) {
    if (!attr.getComment ().empty ()) {
        getFile () << comment (attr.getComment (),
                               std::string (spc () + "/** @var " +
                                            attr.getType () + " "),
                               std::string (spc () + " *       "),
                               " */\n");
    }
}

void
GenerateCodePhp::writeAttribute (const umlClassNode & node,
                                 const umlAttribute & attr,
                                 Visibility & curr_visibility,
                                 const std::string & nameClass) {
    writeAttributeComment (attr);
    writeAttribute1 (node, attr, curr_visibility, nameClass, false, "$");
}

void
GenerateCodePhp::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage *> pkglist;
        std::list <umlPackage *>::const_iterator it;

        umlPackage::makePackageList (node->getPackage (), pkglist);
        getFile () << spc () << "namespace ";
        it = pkglist.begin ();
        while (it != pkglist.end ()) {
            getFile () << (*it)->getName ();
            ++it;
            if (it != pkglist.end ()) {
                getFile () << "/";
            }
        }
        getFile () << spc () << " {\n";
        incIndentLevel ();
    }
}

void
GenerateCodePhp::writeNameSpaceEnd (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        decIndentLevel ();
        getFile () << spc () << "}\n";
    }
}

GenerateCodePhp::~GenerateCodePhp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
