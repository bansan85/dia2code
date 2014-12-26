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


/* NB: If you use CORBA stereotypes, you will need the file p_orb.h
   found in the runtime/cpp directory.  */

#include "config.h"

#include "GenerateCodeCpp.hpp"
#include "umlClassNode.hpp"
#include "string2.hpp"

GenerateCodeCpp::GenerateCodeCpp (DiaGram & diagram) :
    GenerateCode (diagram, "hpp") {
}

std::string
GenerateCodeCpp::strPointer (const std::string & type) const {
    std::string retour (type);
    retour.append (" *");
    return retour;
}

std::string
GenerateCodeCpp::strPackage (const char * package) const {
    std::string retour (package);
    retour.append ("::");
    return retour;
}

const char *
GenerateCodeCpp::fqname (const umlClassNode &node, bool use_ref_type) {
    static std::string buf;

    buf.clear ();
    if (node.getPackage () != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node.getPackage (), pkglist);
        for (umlPackage & it : pkglist) {
            buf.append (strPackage (it.getName ().c_str ()));
        }
    }
    if (use_ref_type) {
        buf.append (strPointer (node.getName ()));
    }
    else {
        buf.append (node.getName ());
    }
    return buf.c_str ();
}

void
GenerateCodeCpp::check_visibility (int *curr_vis, int new_vis) {
    if (*curr_vis == new_vis) {
        return;
    }
    decIndentLevel ();
    switch (new_vis) {
        case '0':
            getFile () << spc () << "public :\n";
            break;
        case '1':
            getFile () << spc () << "private :\n";
            break;
        case '2':
            getFile () << spc () << "protected :\n";
            break;
        case '3':
            fprintf (stderr,
                     "Implementation not applicable in C++.\n",
                     new_vis);
            exit (1);
            break;
        default :
            fprintf (stderr, "Unknown visibility %d\n", new_vis);
            exit (1);
            break;
    }
    *curr_vis = new_vis;
    incIndentLevel ();
}

void
GenerateCodeCpp::writeLicense () {
    if (getLicense ().empty ()) {
        return;
    }

    getFile () << "/*\n";
    writeLicenseAll ();
    getFile () << "*/\n\n";
}

void
GenerateCodeCpp::writeStartHeader (std::string & name) {
    getFile () << spc () << "#ifndef " << name << "__HPP\n";
    getFile () << spc () << "#define " << name << "__HPP\n\n";
}

void
GenerateCodeCpp::writeEndHeader () {
    getFile () << "#endif\n";
}

void
GenerateCodeCpp::writeInclude (std::basic_string <char> name) {
    getFile () << "#include \"" << name << "\"\n";
}

void
GenerateCodeCpp::writeInclude (const char * name) {
    getFile () << "#include \"" << name << "\"\n";
}

void
GenerateCodeCpp::writeFunctionComment (const umlOperation & ope) {
    getFile () << spc () << "/** \\brief " << ope.getComment () << "\n";
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        getFile () << spc () << "    \\param " << tmpa2.getName ()
                   << "\t(" << kind_str (tmpa2.getKind ()) << ") "
                   << tmpa2.getComment () << "\n";
    }
    getFile () << spc () << "*/\n";
}

void
GenerateCodeCpp::writeFunction (const umlOperation & ope) {
    getFile () << spc ();
    if (ope.isAbstract () || getCorba ()) {
        getFile () << "virtual ";
    }
    if (ope.isStatic ()) {
        if (getCorba ()) {
            fprintf (stderr,
                     "CORBAValue %s: static not supported\n",
                     ope.getName ().c_str ());
        }
        else {
            getFile () << "static ";
        }
    }
    if (!ope.getType ().empty ()) {
        getFile () << cppname (ope.getType ()) << " ";
    }
    getFile () << ope.getName () << " (";

    std::list <umlAttribute>::const_iterator tmpa;
    tmpa = ope.getParameters ().begin ();
    while (tmpa != ope.getParameters ().end ()) {
        getFile () << (*tmpa).getType () << " " << (*tmpa).getName ();
        if (!(*tmpa).getValue ().empty ()) {
            if (getCorba ()) {
                fprintf (stderr,
                         "CORBAValue %s: param default not supported\n",
                         ope.getName ().c_str ());
            }
            else {
               getFile () << " = " << (*tmpa).getValue ();
            }
        }
        ++tmpa;
        if (tmpa != ope.getParameters ().end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ")";
    if (ope.isConstant ()) {
        getFile () << " const";
    }
    // virtual
    if ((ope.isAbstract () || getCorba ()) &&
        ope.getName ()[0] != '~') {
        getFile () << " = 0";
    }
    getFile () << ";\n";
}

void
GenerateCodeCpp::writeComment (const std::string & text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeCpp::writeComment (const char * text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeCpp::writeClassComment (const umlClassNode & node) {
    getFile () << spc () << "/** \\class " << node.getName () << "\n";
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "    \\brief " << node.getComment () << "\n";
    }
    getFile () << spc () << "*/\n";
}

void
GenerateCodeCpp::writeClassStart (const umlClassNode & node) {
    getFile () << spc () << "class " << node.getName ();
    if (!node.getParents ().empty ()) {
        std::list <umlClass *>::const_iterator parent;
        parent = node.getParents ().begin ();
        getFile () << " : ";
        while (parent != node.getParents ().end ()) {
            getFile () << "public " << fqname (**parent, false);
            ++parent;
            if (parent != node.getParents ().end ()) {
                getFile () << ", ";
            }
        }
    } else if (getCorba ()) {
        getFile () << " : " << strPackage ("CORBA") << "ValueBase";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << "\n"
                   << spc () << "{\n";
    }
    else {
        getFile () << " {\n";
    }
}

void
GenerateCodeCpp::writeClassEnd (const umlClassNode & node) {
    getFile () << spc () << "};\n\n";
}

void
GenerateCodeCpp::writeAttribute (const umlAttribute & attr,
                                 int * curr_visibility) {
    check_visibility (curr_visibility, attr.getVisibility ());
    if (!attr.getComment ().empty ()) {
        getFile () << spc () << "/// " << attr.getComment () << "\n";
    }
    if (attr.isStatic ()) {
        getFile () << spc () << "static " << attr.getType () << " "
                   << attr.getName ();
    }
    else {
        getFile () << spc () << attr.getType () << " "
                   << attr.getName ();
    }
    getFile () << ";\n";
}

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
