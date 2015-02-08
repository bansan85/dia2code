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
    incIndentLevel ();
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

    /* print comments on operation */
    if (!ope.getComment ().empty ()) {
        writeFunctionComment (ope);
    }

    getFile () << spc ();
    if ((ope.getInheritance () != Inheritance::FINAL)
#ifdef ENABLE_CORBA
        || (getCorba ())
#endif
    ) {
        getFile () << "virtual ";
    }
    if (ope.isStatic ()) {
#ifdef ENABLE_CORBA
        if (getCorba ()) {
            fprintf (stderr,
                     "CORBAValue %s: static not supported\n",
                     ope.getName ().c_str ());
        }
        else
#endif
        {
            getFile () << "static ";
        }
    }
    if (ope.isConstant ()) {
        getFile () << "constexpr ";
    }
    if (!ope.getType ().empty ()) {
        getFile () << cppName (ope.getType ()) << " ";
    }
    getFile () << ope.getName () << " (";

    std::list <umlAttribute>::const_iterator tmpa;
    tmpa = ope.getParameters ().begin ();
    while (tmpa != ope.getParameters ().end ()) {
        getFile () << (*tmpa).getType () << " " << (*tmpa).getName ();
        if (!(*tmpa).getValue ().empty ()) {
#ifdef ENABLE_CORBA
            if (getCorba ()) {
                fprintf (stderr,
                         "CORBAValue %s: param default not supported\n",
                         ope.getName ().c_str ());
            }
            else
#endif
            {
               getFile () << " = " << (*tmpa).getValue ();
            }
        }
        ++tmpa;
        if (tmpa != ope.getParameters ().end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ")";
    if (ope.getStereotype ().compare ("delete") == 0) {
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
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    getFile () << spc () << "/// " << node.getComment () << "\n";
    if (node.getAttributes ().size () != 1) {
        throw std::string ("Error: first attribute not set at " +
                           node.getName () + "\n");
    }
    if (!(*umla).getName ().empty ()) {
        fprintf (stderr,
                 "Warning: ignoring attribute name at %s\n",
                 node.getName ().c_str ());
    }

    getFile () << spc () << "constexpr " << cppName ((*umla).getType ())
               << " " << node.getName () << " = " << (*umla).getValue ()
               << ";\n";
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
        if (!(*umla).getComment ().empty ()) {
            getFile () << spc () << "/// " << (*umla).getComment () << "\n";
        }
        if (!(*umla).getType ().empty ()) {
            fprintf (stderr,
                     "%s/%s: ignoring type\n",
                     node.getName ().c_str (),
                     literal);
        }
        if ((*umla).getVisibility () != Visibility::PUBLIC) {
            fprintf (stderr,
                    "Enum %s, attribute %s: visibility forced to visible.\n",
                     node.getName ().c_str (),
                     (*umla).getName ().c_str ());
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
