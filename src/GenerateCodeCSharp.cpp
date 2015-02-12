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
#ifdef ENABLE_CORBA
    if (getCorba ()) {
        if (ope.getVisibility () != '0') {
            fprintf (stderr,
                     "CORBAValue %s: must be public\n",
                     ope.getName ().c_str ());
        }
    }
#endif

    /* print comments on operation */
    if (!ope.getComment ().empty ()) {
        writeFunctionComment (ope);
    }

    getFile () << spc ();
    getFile () << visibility (ope.getVisibility ()) << " ";
    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::VIRTUAL) {
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
    if (!ope.getType ().empty ()) {
        getFile () << cppName (ope.getType ()) << " ";
    }
    getFile () << ope.getName () << " (";

    std::list <umlAttribute>::const_iterator tmpa;
    tmpa = ope.getParameters ().begin ();
    while (tmpa != ope.getParameters ().end ()) {
        getFile () << (*tmpa).getType () << " " << (*tmpa).getName ();
        if (!(*tmpa).getValue ().empty ()) {
            fprintf (stderr, "Java does not support param default.\n");
        }
        ++tmpa;
        if (tmpa != ope.getParameters ().end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ")";
    if (ope.isConstant ()) {
        fprintf (stderr, "Java does not support const method.\n");
    }
    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << ";\n";
    }
    else
    {
        if (getOpenBraceOnNewline ()) {
            getFile () << "\n";
            getFile () << spc () << "{\n";
        }
        else {
            getFile () << " {\n";
        }
        getFile () << spc () << "}\n";
    }
}

void
GenerateCodeCSharp::writeClassComment (const umlClassNode & node) {
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// <summary>\n";
        getFile () << comment (node.getComment (),
                               std::string (spc () + "///  "),
                               std::string (spc () + "///  "));
        getFile () << spc () << "/// </summary>\n";
    }
}

void
GenerateCodeCSharp::writeAttribute (const umlAttribute & attr,
                                    Visibility & curr_visibility) {
    if (!attr.getComment ().empty ()) {
        getFile () << spc () << "/// <summary>\n";
        getFile () << comment (attr.getComment (),
                               std::string (spc () + "///  "),
                               std::string (spc () + "///  "));
        getFile () << spc () << "/// </summary>\n";
    }
    getFile () << spc () << visibility (attr.getVisibility ()) << " ";
    if (attr.isStatic ()) {
        getFile () << "static " << attr.getType () << " " << attr.getName ();
    }
    else {
        getFile () << attr.getType () << " " << attr.getName ();
    }
    if (!attr.getValue ().empty ()) {
        getFile () << " = " << attr.getValue ();
    }
    getFile () << ";\n";
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

void
GenerateCodeCSharp::writeEnum (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// <summary>\n";
        getFile () << comment (node.getComment (),
                               std::string (spc () + "///  "),
                               std::string (spc () + "///  "));
        getFile () << spc () << "/// </summary>\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "public enum " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "public enum " << node.getName () << " {\n";
    }
    incIndentLevel ();
    while (umla != node.getAttributes ().end ()) {
        const char *literal = (*umla).getName ().c_str ();
        if (!(*umla).getType ().empty ()) {
            fprintf (stderr,
                     "%s/%s: ignoring type\n",
                     node.getName ().c_str (),
                     literal);
        }
        if ((*umla).getName ().empty ()) {
            fprintf (stderr,
                     "%s: an unamed attribute is found.\n",
                     node.getName ().c_str ());
        }
        if ((*umla).getVisibility () != Visibility::PUBLIC) {
            fprintf (stderr,
                     "Enum %s, attribute %s: visibility forced to public.\n",
                     node.getName ().c_str (),
                     (*umla).getName ().c_str ());
        }
        if (!(*umla).getComment ().empty ()) {
            getFile () << spc () << "/// <summary>" << (*umla).getComment ()
                       << "</summary>" << "\n";
        }
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

GenerateCodeCSharp::~GenerateCodeCSharp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
