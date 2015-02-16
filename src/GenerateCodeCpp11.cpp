/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014 Javier O'Hara - Oliver Kellogg

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

#include "GenerateCodeCpp11.hpp"
#include "string2.hpp"
#include "scan_tree.hpp"

GenerateCodeCpp11::GenerateCodeCpp11 (DiaGram & diagram) :
    GenerateCodeCpp (diagram) {
}

void
GenerateCodeCpp11::writeFunction (const umlOperation & ope,
                                  Visibility & curr_visibility) {
    // Check validity of ope and indent.
    writeFunction1 (ope);

#ifdef ENABLE_CORBA
    if (getCorba ()) {
        if (ope.getVisibility () != '0') {
            fprintf (stderr,
                     "CORBAValue %s: must be public\n",
                     ope.getName ().c_str ());
        }
    }
    else
#endif
    {
        check_visibility (curr_visibility, ope.getVisibility ());
    }

    // Write comment and start function with virtual and static.
    writeFunction2 (ope);

    if (ope.isConstant ()) {
        getFile () << "constexpr ";
    }

    // Write the reste of the function until the ")"
    writeFunction3 (ope);

    if (ope.isStereotypeDelete ()) {
        getFile () << " = delete";
    }
    // virtual
    else if ((ope.getInheritance () == Inheritance::ABSTRACT)
#ifdef ENABLE_CORBA
        || (getCorba ())
#endif
       ) {
        getFile () << " = 0";
    }
    getFile () << ";\n";
    decIndentLevel ();
}

void
GenerateCodeCpp11::writeConst (const umlClassNode & node) {
    writeConst1 (node, "constexpr ");
}

void
GenerateCodeCpp11::writeEnum (const umlClassNode & node) {
    writeEnum1 (node, "enum class ");
}

GenerateCodeCpp11::~GenerateCodeCpp11 () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
