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

#include "GenerateCodeCSharp.hpp"
#include "umlClassNode.hpp"

GenerateCodeCSharp::GenerateCodeCSharp (DiaGram & diagram) :
    GenerateCodeJava (diagram) {
    setFileExt ("cs");
}

bool
GenerateCodeCSharp::writeInclude (std::pair <std::list <umlPackage *>,
                                  umlClassNode * > & name) {
    getFile () << spc () << "using ";

    if (!name.first.empty ()) {
        std::list <umlPackage *>::const_iterator namei;

        namei = name.first.begin ();
        while (namei != name.first.end ()) {
            getFile () << (*namei)->getName ();
            ++namei;
            if (namei != name.first.end ()) {
                getFile () << ".";
            }
        }
        // We don't add the name of the class.
    }
    else if (name.second != NULL) {
        getFile () << name.second->getName ();
    }
    else {
        return false;
    }

    getFile () << ";\n";

    return true;
}

void
GenerateCodeCSharp::writeFunction (const umlOperation & ope,
                                   Visibility & curr_visibility) {
    writeFunction1 (ope, curr_visibility);

    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::VIRTUAL) {
        getFile () << "virtual ";
    }

    writeFunction2 (ope, curr_visibility);
}

void
GenerateCodeCSharp::writeClassComment (const std::string & nom) {
    if (!nom.empty ()) {
        getFile () << spc () << "/// <summary>\n";
        getFile () << comment (nom,
                               std::string (spc () + "///  "),
                               std::string (spc () + "///  "));
        getFile () << spc () << "/// </summary>\n";
    }
}

void
GenerateCodeCSharp::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage *> pkglist;

        umlPackage::makePackageList (node->getPackage (), pkglist);
        for (const umlPackage * it : pkglist) {
            if (getOpenBraceOnNewline ()) {
                getFile () << spc () << "namespace " << it->getName () << "\n"
                           << spc () << "{\n";
            }
            else {
                getFile () << spc () << "namespace " << it->getName ()
                           << " {\n";
            }
            incIndentLevel ();
        }
    }
}

void
GenerateCodeCSharp::writeNameSpaceEnd (const umlClassNode * node) {
    const umlPackage *pack = node->getPackage ();

    while (pack != NULL) {
        decIndentLevel ();
        getFile () << spc () << "};\n";
        pack = pack->getParent ();
    }
    getFile () << spc () << "\n";
}

GenerateCodeCSharp::~GenerateCodeCSharp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
