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
#include "string2.hpp"
#include "scan_tree.hpp"

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
            throw std::string ("Implementation not applicable in C++.\n");
            break;
        default :
            throw std::string (new_vis + " : Unknown visibility.\n");
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
    writeFile ();
    getFile () << "*/\n\n";
}

void
GenerateCodeCpp::writeStartHeader (std::string & name) {
    getFile () << spc () << "#ifndef " << name << "__HPP\n";
    getFile () << spc () << "#define " << name << "__HPP\n";
}

void
GenerateCodeCpp::writeEndHeader () {
    getFile () << spc () << "\n";
    getFile () << spc () << "#endif\n";
}

void
GenerateCodeCpp::writeInclude (std::basic_string <char> name) {
    getFile () << spc () << "#include \"" << name << "\"\n";
}

void
GenerateCodeCpp::writeInclude (const char * name) {
    getFile () << spc () << "#include \"" << name << "\"\n";
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
GenerateCodeCpp::writeFunction (const umlOperation & ope,
                                int * curr_visibility) {
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
    if (ope.isAbstract ()
#ifdef ENABLE_CORBA
        || getCorba ()
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
    if (ope.isConstant ()) {
        getFile () << " const";
    }
    // virtual
    if ((ope.isAbstract ()
#ifdef ENABLE_CORBA
        || getCorba ()
#endif
        ) &&
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
GenerateCodeCpp::writeClass (const umlClassNode & node) {
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
GenerateCodeCpp::writeClassEnd () {
    getFile () << spc () << "};\n";
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

void
GenerateCodeCpp::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node->getPackage (), pkglist);
        for (const umlPackage & it : pkglist) {
            if (getOpenBraceOnNewline ()) {
                getFile () << spc () << "namespace " << it.getName () << "\n"
                           << spc () << "{\n";
            }
            else {
                getFile () << spc () << "namespace " << it.getName ()
                           << " {\n";
            }
            incIndentLevel ();
        }
    }
}

void
GenerateCodeCpp::writeNameSpaceEnd (const umlClassNode * node) {
    const umlPackage *pack = node->getPackage ();

    while (pack != NULL) {
        decIndentLevel ();
        getFile () << spc () << "};\n";
        pack = pack->getParent ();
    }
}

void
GenerateCodeCpp::writeConst (const umlClassNode & node) {
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

    getFile () << spc () << "const " << cppName ((*umla).getType ())
               << " " << node.getName () << " = " << (*umla).getValue ()
               << ";\n";
}

void
GenerateCodeCpp::writeEnum (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "enum " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "enum " << node.getName () << " {\n";
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
GenerateCodeCpp::writeStruct (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    getFile () << spc () << "/// " << node.getComment () << "\n";
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "struct " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "struct " << node.getName () << " {\n";
    }
    incIndentLevel ();
    while (umla != node.getAttributes ().end ()) {
        (*umla).check (node.getName ().c_str ());
        getFile () << spc () << "/// " << (*umla).getComment () << "\n";
        getFile () << spc () << cppName ((*umla).getType ()) << " "
             << (*umla).getName ();
        if (!(*umla).getValue ().empty ()) {
            fprintf (stderr,
                     "%s/%s: ignoring value %s\n",
                     node.getName ().c_str (),
                     (*umla).getName ().c_str (),
                     (*umla).getValue ().c_str ());
        }
        getFile () << ";\n";
        ++umla;
    }
    decIndentLevel ();
    getFile () << spc () << "};\n";
}

void
GenerateCodeCpp::writeTypedef (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (umla == node.getAttributes ().end ()) {
        throw std::string ("Error: first attribute (impl type) not set at typedef " + node.getName () + ".\n");
    }
    if (!(*umla).getName ().empty ())  {
        fprintf (stderr,
                 "Warning: typedef %s: ignoring name field in implementation type attribute\n",
                 node.getName ().c_str ());
    }
    getFile () << spc () << "typedef " << cppName ((*umla).getType ()) << " "
               << node.getName () << (*umla).getValue () << ";\n";
}

void
GenerateCodeCpp::writeAssociation (const umlassoc & asso) {
    if (!asso.name.empty ()) {
        umlClassNode *ref;
        ref = find_by_name (getDia ().getUml (),
                            asso.key.getName ().c_str ());
        if (ref != NULL) {
            getFile () << spc () << fqname (*ref, !asso.composite);
        }
        else {
            getFile () << spc () << cppName (asso.key.getName ());
        }
        getFile () << " " << asso.name << ";\n";
    }
}

void
GenerateCodeCpp::writeTemplates (
              const std::list <std::pair <std::string, std::string> > & tmps) {
    std::list <std::pair <std::string, std::string> >::const_iterator
                                                     template_ = tmps.begin ();
    getFile () << spc () << "template <";
    while (template_ != tmps.end ()) {
        getFile () << (*template_).second << " " << (*template_).first;
        ++template_;
        if (template_ != tmps.end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ">\n";
}

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
