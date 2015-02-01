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

#include "GenerateCodeJava.hpp"
#include "string2.hpp"
#include "scan_tree.hpp"

GenerateCodeJava::GenerateCodeJava (DiaGram & diagram) :
    GenerateCode (diagram, "java") {
}

std::string
GenerateCodeJava::strPointer (const std::string & type) const {
    std::string retour (type);
    retour.append (" *");
    return retour;
}

std::string
GenerateCodeJava::strPackage (const char * package) const {
    std::string retour (package);
    retour.append (".");
    return retour;
}

const char *
GenerateCodeJava::fqname (const umlClassNode &node, bool use_ref_type) {
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

std::string
visibility (int vis) {
    switch (vis) {
        case '0':
            return std::string ("public ");
        case '1':
            return std::string ("private ");
        case '2':
            return std::string ("protected ");
        case '3':
            fprintf (stderr, "Implementation not applicable in Java.\n"
                             "Default: public.");
            return std::string ("public ");
            break;
        default :
            throw std::string (vis + " : Unknown visibility.\n");
            break;
    }
}

void
GenerateCodeJava::writeLicense () {
    if (getLicense ().empty ()) {
        return;
    }

    getFile () << "/*\n";
    writeFile ();
    getFile () << " */\n\n";
}

void
GenerateCodeJava::writeStartHeader (std::string & name) {
}

void
GenerateCodeJava::writeEndHeader () {
}

void
GenerateCodeJava::writeInclude (std::list <std::string> & name) {
    std::list <std::string>::const_iterator namei = name.begin ();

    if (name.empty ()) {
        return;
    }

    getFile () << spc () << "import ";

    while (namei != name.end ()) {
        getFile () << *namei;
        ++namei;
        if (namei != name.end ()) {
            getFile () << ".";
        }
    }
    getFile () << ";\n";
}

void
GenerateCodeJava::writeInclude (const char * name) {
    getFile () << spc () << "import " << name << ";\n";
}

void
GenerateCodeJava::writeFunctionComment (const umlOperation & ope) {
    getFile () << spc () << "/**\n";
    getFile () << spc () << " * " << ope.getComment () << "\n";
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        getFile () << spc () << " * @param " << tmpa2.getName ()
                   << " (" << kind_str (tmpa2.getKind ()) << ") "
                   << tmpa2.getComment () << "\n";
    }
    getFile () << spc () << " * @return " << ope.getType () << "\n";
    getFile () << spc () << "*/\n";
}

void
GenerateCodeJava::writeFunction (const umlOperation & ope,
                                 int * curr_visibility) {
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
    if (ope.getInherence () == 0) {
        getFile () << "abstract ";
    }
    else if (ope.getInherence () == 2) {
        fprintf (stderr,
                 "With Java generator, all function is by default virtual.\n");
    }
    getFile () << visibility (ope.getVisibility ());
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
    if (getOpenBraceOnNewline ()) {
        getFile () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << " {\n";
    }
    getFile () << spc () << "}\n";
}

void
GenerateCodeJava::writeComment (const std::string & text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeJava::writeComment (const char * text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeJava::writeClassComment (const umlClassNode & node) {
    if (!node.getComment ().empty ()) {
        size_t start = 0;
        size_t end;

        std::string replace (spc ());
        replace.append (" * ");

        getFile () << spc () << "/**\n";

        end = node.getComment ().find ("\n", start);
        while (end != std::string::npos)
        {
            getFile () << spc () << " * "
                       << node.getComment ().substr (start, end-start) << "\n";
            start = end + 1;
            end = node.getComment ().find ("\n", start);
        }
        getFile () << spc () << " * "
                   << node.getComment ().substr (start) << "\n";
        getFile () << spc () << "*/\n";
    }
}

void
GenerateCodeJava::writeClassStart (const umlClassNode & node) {
    getFile () << spc () << "public class " << node.getName ();
    if (!node.getParents ().empty ()) {
        std::list <umlClass *>::const_iterator parent;
        parent = node.getParents ().begin ();
        while (parent != node.getParents ().end ()) {
            getFile () << " extends " << fqname (**parent, false);
            ++parent;
        }
    }
#ifdef ENABLE_CORBA
    else if (getCorba ()) {
        getFile () << " : " << strPackage ("CORBA") << "ValueBase";
    }
#endif
    if (getOpenBraceOnNewline ()) {
        getFile () << "\n"
                   << spc () << "{\n";
    }
    else {
        getFile () << " {\n";
    }
}

void
GenerateCodeJava::writeClassEnd () {
    getFile () << spc () << "}\n";
}

void
GenerateCodeJava::writeAttribute (const umlAttribute & attr,
                                 int * curr_visibility) {
    if (!attr.getComment ().empty ()) {
        getFile () << spc () << "/**\n";
        getFile () << spc () << " * " << attr.getComment () << "\n";
        getFile () << spc () << " */\n";
    }
    getFile () << spc () << visibility (attr.getVisibility ());
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
GenerateCodeJava::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node->getPackage (), pkglist);
        std::list <umlPackage>::const_iterator it = pkglist.begin ();

        getFile () << spc () << "package ";
        while (it != pkglist.end ()) {
            getFile () << (*it).getName ();
            ++it;
            if (it != pkglist.end ()) {
                getFile () << ".";
            }
        }
        getFile () << ";\n\n";
    }
}

void
GenerateCodeJava::writeNameSpaceEnd (const umlClassNode * node) {
}

void
GenerateCodeJava::writeConst (const umlClassNode & node) {
    fprintf (stderr, "Const stereotype in not applicable to Java.\n");
}

void
GenerateCodeJava::writeEnum (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/**\n";
        getFile () << spc () << " * " << node.getComment () << "\n";
        getFile () << spc () << " */\n";
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

void
GenerateCodeJava::writeStruct (const umlClassNode & node) {
    umlClassNode & nodetmp = const_cast <umlClassNode &> (node);
    std::string stereo;

    nodetmp.setStereotype (stereo);
    genClass (nodetmp);
}

void
GenerateCodeJava::writeTypedef (const umlClassNode & node) {
    fprintf (stderr, "Typedef stereotype in not applicable to Java.\n");
}

void
GenerateCodeJava::writeAssociation (const umlassoc & asso,
                                    int * curr_visibility) {
    if (!asso.name.empty ()) {
        getFile () << spc () << visibility (asso.visibility)
                   << cppName (asso.key.getName ());
        getFile () << " " << asso.name << ";\n";
    }
}

void
GenerateCodeJava::writeTemplates (
              const std::list <std::pair <std::string, std::string> > & tmps) {
    fprintf (stderr, "Template in not applicable to Java.\n");
}

GenerateCodeJava::~GenerateCodeJava () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
