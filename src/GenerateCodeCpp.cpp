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
        umlPackage::makePackageList (node.getPackage (), pkglist);
        for (const umlPackage & it : pkglist) {
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

const char *
GenerateCodeCpp::visibility (const Visibility & vis) {
    switch (vis) {
        case Visibility::PUBLIC :
            return "public";
            break;
        case Visibility::PRIVATE:
            return "private";
            break;
        case Visibility::PROTECTED :
            return "protected";
            break;
        case Visibility::IMPLEMENTATION :
            fprintf (stderr, "Implementation not applicable in C++.\n"
                             "Default: public.\n");
            return "public";
            break;
        default :
            throw std::string ("Unknown visibility.\n");
            break;
    }
}

void
GenerateCodeCpp::check_visibility (Visibility & curr_vis,
                                   const Visibility new_vis) {
    if (curr_vis == new_vis) {
        return;
    }
    decIndentLevel ();
    getFile () << spc () << visibility (new_vis) << " :\n";
    curr_vis = new_vis;
    incIndentLevel ();
}

void
GenerateCodeCpp::writeLicense () {
    if (getLicense ().empty ()) {
        return;
    }

    getFile () << "/*\n";
    writeFile ();
    getFile () << " */\n\n";
}

void
GenerateCodeCpp::writeStartHeader (std::string & name) {
    getFile () << spc () << "#ifndef " << name << "__HPP\n";
    getFile () << spc () << "#define " << name << "__HPP\n\n";
}

void
GenerateCodeCpp::writeEndHeader () {
    getFile () << spc () << "#endif\n";
}

bool
GenerateCodeCpp::writeInclude (std::pair <std::list <umlPackage>,
                               umlClassNode * > & name) {
    std::list <umlPackage>::const_iterator namei;

    if (name.second == NULL) {
        return false;
    }

    getFile () << spc () << "#include \"";
    
    if (getBuildTree ()) {
        if (!name.first.empty ()) {
            for (const umlPackage & pack : name.first) {
                getFile () << pack.getName () << SEPARATOR;
            }
        }
        getFile () << name.second->getName () << "." << getFileExt ()
                   << "\"\n";
    }
    else if (getOneClass ()) {
        getFile () << name.second->getName () << "." << getFileExt ()
                   << "\"\n";
    }
    else {
        if (!name.first.empty ()) {
            namei = name.first.begin ();
            getFile () << (*namei).getName () << "." << getFileExt ()
                       << "\"\n";
        }
        else {
            getFile () << name.second->getName () << "." << getFileExt ()
            << "\"\n";
        }
    }

    return true;
}

void
GenerateCodeCpp::writeInclude (const char * name) {
    getFile () << spc () << "#include \"" << name << "\"\n";
}

void
GenerateCodeCpp::writeFunctionComment (const umlOperation & ope) {
    getFile () << spc () << "/**\n";
    getFile () << comment (ope.getComment (),
                           std::string (spc () + " * \\brief "),
                           std::string (spc () + " *        "));
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        std::string comment_ (tmpa2.getName () + " (" +
                              kindStr (tmpa2.getKind ()) +
                              (tmpa2.getComment ().empty () ?
                                ")" :
                                ") " +
                              tmpa2.getComment ()));
        getFile () << comment (comment_,
                               std::string (spc () + " * \\param "),
                               std::string (spc () + " *        "));
    }
    getFile () << spc () << " * \\return " << ope.getType () << "\n";
    getFile () << spc () << " */\n";
}

void
GenerateCodeCpp::writeFunction (const umlOperation & ope,
                                Visibility & curr_visibility) {
    if (ope.getName ().empty ()) {
        fprintf (stderr, "An unamed operation is found.\n");
    }
    if (ope.getStereotype ().compare ("delete") == 0) {
        const_cast <umlOperation &> (ope).setVisibility (Visibility::PRIVATE);
    }
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
    if ((ope.getInheritance () == Inheritance::ABSTRACT)
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
GenerateCodeCpp::writeComment (const std::string & text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeCpp::writeComment (const char * text) {
    getFile () << spc () << "// " << text << "\n";
}

void
GenerateCodeCpp::writeClassComment (const umlClassNode & node) {
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/**\n";
        getFile () << spc () << " * \\class " << node.getName () << "\n";

        getFile () << comment (node.getComment (),
                               std::string (spc () + " * \\brief "),
                               std::string (spc () + " *        "));
        getFile () << spc () << "*/\n";
    }
}

void
GenerateCodeCpp::writeClassStart (const umlClassNode & node) {
    if (!node.getTemplates ().empty ()) {
#ifdef ENABLE_CORBA
        if (isCorba) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else
#endif
        {
            writeTemplates (node.getTemplates ());
        }
    }

    getFile () << spc () << "class " << node.getName ();
    if (!node.getParents ().empty ()) {
        std::list <std::pair <umlClass *, Visibility> >::const_iterator
                                                                        parent;
        parent = node.getParents ().begin ();
        getFile () << " : ";
        while (parent != node.getParents ().end ()) {
            getFile () << visibility ((*parent).second) << " "
                       << fqname (*(*parent).first, false);
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
                                 Visibility & curr_visibility) {
    if (!attr.getValue ().empty ()) {
        fprintf (stderr,
                 "Default value for attribut in class is not applicable in C++.\n");
    }

    incIndentLevel ();
    check_visibility (curr_visibility, attr.getVisibility ());
    if (!attr.getComment ().empty ()) {
        if (attr.getComment ().find ("\n", 0) == std::string::npos) {
            getFile () << spc () << "/// " << attr.getComment () << "\n";
        }
        else {
            getFile () << comment (attr.getComment (),
                                   std::string (spc () + "/// "),
                                   std::string (spc () + "/// "));
        }
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
    decIndentLevel ();
}

void
GenerateCodeCpp::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::makePackageList (node->getPackage (), pkglist);
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
    getFile () << spc () << "\n";
}

void
GenerateCodeCpp::writeConst (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
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
        if (!(*umla).getComment ().empty ()) {
            getFile () << spc () << "/// " << (*umla).getComment () << "\n";
        }
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
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "struct " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "struct " << node.getName () << " {\n";
    }
    for (const umlAttribute & umla : node.getAttributes ()) {
        if (umla.getName ().empty ()) {
            fprintf (stderr,
                     "%s: an unamed attribute is found.\n",
                     node.getName ().c_str ());
        }
        if (umla.getVisibility () != Visibility::PUBLIC) {
            fprintf (stderr,
                    "Struct %s, attribute %s: visibility forced to visible.\n",
                     node.getName ().c_str (),
                     umla.getName ().c_str ());
        }
        // Use of a tmp value to ignore visibility.
        Visibility vis = umla.getVisibility ();
        writeAttribute (umla, vis);
    }
    for (const umlOperation & umlo : node.getOperations ()) {
        if (umlo.getVisibility () != Visibility::PUBLIC) {
            fprintf (stderr,
                    "Struct %s, operation %s: visibility forced to visible.\n",
                     node.getName ().c_str (),
                     umlo.getName ().c_str ());
        }
        // Use of a tmp value to ignore visibility.
        Visibility vis = umlo.getVisibility ();
        writeFunction (umlo, vis);
    }
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
    const umlClassNode * umlc = find_by_name (getDia ().getUml (),
                                              (*umla).getType ().c_str ());
    getFile () << spc () << "typedef ";
    if (umlc == NULL) {
        getFile () << cppName ((*umla).getType ());
    }
    else {
        getFile () << fqname (*umlc, false);
    }
    getFile () << (*umla).getValue () << " " << node.getName () << ";\n";
}

void
GenerateCodeCpp::writeAssociation (const umlassoc & asso,
                                   Visibility & curr_visibility) {
    if (!asso.name.empty ()) {
        incIndentLevel ();
        const umlClassNode *ref;
        ref = find_by_name (getDia ().getUml (),
                            asso.key.getName ().c_str ());
        check_visibility (curr_visibility, asso.visibility);
        getFile () << spc ();
        if (ref != NULL) {
            getFile () << fqname (*ref, !asso.composite);
        }
        else {
            getFile () << cppName (asso.key.getName ());
        }
        getFile () << " " << asso.name << ";\n";
        decIndentLevel ();
    }
}

void
GenerateCodeCpp::writeTemplates (
              const std::list <std::pair <std::string, std::string> > & tmps) {
    std::list <std::pair <std::string, std::string> >::const_iterator
                                                     template_ = tmps.begin ();
    getFile () << spc () << "template <";
    while (template_ != tmps.end ()) {
        const umlClassNode * umlc = find_by_name (getDia ().getUml (),
                                                 (*template_).second.c_str ());
        if (umlc == NULL) {
            getFile () << cppName ((*template_).second);
        }
        else {
            getFile () << fqname (*umlc, false);
        }
        getFile () << " " << (*template_).first;
        ++template_;
        if (template_ != tmps.end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ">\n\n";
}

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
