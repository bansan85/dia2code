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
GenerateCodeCpp11::writeEnum (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "enum class " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "enum class " << node.getName () << " {\n";
    }
    incIndentLevel ();
    while (umla != node.getAttributes ().end ()) {
        const char *literal = (*umla).getName ().c_str ();
        (*umla).check (node.getName ().c_str ());
        getFile () << spc () << "/// " << (*umla).getComment () << "\n";
        if (!(*umla).getType ().empty ()) {
            fprintf (stderr,
                     "%s/%s: ignoring type\n",
                     node.getName ().c_str (),
                     literal);
        }
        getFile () << spc () << literal;
        if (!(*umla).getValue ().empty ()) {
            getFile () << " = " << (*umla).getValue ();
        }
        ++umla;
        if (umla != node.getAttributes ().end ()) {
            getFile () << ",";
        }
        getFile () << "\n";
    }
    decIndentLevel ();
    getFile () << spc () << "};\n";
}

GenerateCodeCpp11::~GenerateCodeCpp11 () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
