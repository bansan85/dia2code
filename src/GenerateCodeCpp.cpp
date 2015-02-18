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

#include <algorithm>
#include <iostream>

#include "GenerateCodeCpp.hpp"
#include "GenerateCodeJava.hpp"
#include "string2.hpp"
#include "scan_tree.hpp"

GenerateCodeCpp::GenerateCodeCpp (DiaGram & diagram) :
    GenerateCode (diagram, "hpp", false) {
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
GenerateCodeCpp::visibility (std::string desc,
                             const Visibility & vis) {
    return visibility1 (desc, vis);
}

void
GenerateCodeCpp::writeLicense () {
    writeLicense1 ("/*", " */");
}

void
GenerateCodeCpp::check_visibility (std::string desc,
                                   Visibility & curr_vis,
                                   const Visibility new_vis) {
    if (curr_vis == new_vis) {
        return;
    }
    decIndentLevel ();
    getFile () << spc () << visibility (desc, new_vis) << " :\n";
    curr_vis = new_vis;
    incIndentLevel ();
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
GenerateCodeCpp::writeInclude (const std::list <std::pair <
                                                    std::list <umlPackage *>,
                                                    umlClassNode *> > & name) {
    bool ret = false;
    // List of include then if (true) the class is system and should not be
    // generated.
    std::list <std::pair <std::string, bool> > incs;

    for (const std::pair <std::list <umlPackage *>, umlClassNode *> & it :
                                                                        name) {
        std::string include;
        std::pair <std::string, bool> add;

        if (it.second == NULL) {
            continue;
        }

        ret = true;
    
        if (getBuildTree ()) {
            if (!it.first.empty ()) {
                for (const umlPackage * pack : it.first) {
                    include.append (pack->getName ());
                    include.append (1, SEPARATOR);
                }
            }
            include.append (it.second->getName ());
            include.append (".");
            include.append (getFileExt ());
        }
        else if (getOneClass ()) {
            include.append (it.second->getName ());
            include.append (".");
            include.append (getFileExt ());
        }
        else {
            if (!it.first.empty ()) {
                include.append ((*it.first.begin ())->getName ());
                include.append (".");
                include.append (getFileExt ());
            }
            else {
                include.append (it.second->getName ());
                include.append (".");
                include.append (getFileExt ());
            }
        }
        add.first = include;
        add.second = it.second->isStereotypeExtern ();
        if (std::find (incs.begin (),
                       incs.end (),
                       add) == incs.end ()) {
            if (add.second) {
                incs.push_front (add);
            }
            else {
                incs.push_back (add);
            }
        }
    }

    for (const std::pair <std::string, bool> & add : incs) {
        if (add.second) {
            getFile () << spc () << "#include <" << add.first << ">\n";
        }
        else {
            getFile () << spc () << "#include \"" << add.first << "\"\n";
        }
    }
    if (!incs.empty ()) {
        getFile () << "\n";
    }

    return ret;
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
                           std::string (spc () + " *        "),
                           "\n");
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        std::string comment_ (tmpa2.getName () + " (" +
                              kindStr (tmpa2.getKind ()) +
                              (tmpa2.getComment ().empty () ?
                                ")" :
                                ") " +
                              tmpa2.getComment ()));
        getFile () << comment (comment_,
                               std::string (spc () + " * \\param "),
                               std::string (spc () + " *        "),
                               "\n");
    }
    getFile () << spc () << " * \\return " << ope.getType () << "\n";
    getFile () << spc () << " */\n";
}

void
GenerateCodeCpp::writeFunction1 (const umlOperation & ope) {
    if (ope.getName ().empty ()) {
        std::cerr << "An unamed operation is found.\n";
    }
    incIndentLevel ();
}

void
GenerateCodeCpp::writeFunction2 (const umlOperation & ope) {
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
            std::cerr << "CORBAValue " << ope.getName ()
                      << ": static not supported.\n";
        }
        else
#endif
        {
            getFile () << "static ";
        }
    }
}

void
GenerateCodeCpp::writeFunction3 (const umlOperation & ope) {
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
                std::cerr << "CORBAValue " << ope.getName () << ": param default not supported.\n";
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
}

void
GenerateCodeCpp::writeFunction (const umlClassNode & node,
                                const umlOperation & ope,
                                Visibility & curr_visibility) {
    // Check validity of ope and indent.
    writeFunction1 (ope);

#ifdef ENABLE_CORBA
    if (getCorba ()) {
        if (ope.getVisibility () != '0') {
            std::cerr << "CORBAValue " << ope.getName ()
                      << ": must be public.\n";
        }
    }
    else
#endif
    {
        if (ope.isStereotypeDelete ()) {
            check_visibility ("Class \"" + node.getName () + "\", operation \""
                                         + ope.getName () + "\"",
                              curr_visibility,
                              Visibility::PRIVATE);
        } else {
            check_visibility ("Class \"" + node.getName () + "\", operation \""                                         + ope.getName () + "\"",
                              curr_visibility,
                              ope.getVisibility ());
        }
    }

    // Write comment and start function with virtual and static.
    writeFunction2 (ope);
    // Write the reste of the function until the ")"
    writeFunction3 (ope);

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
GenerateCodeCpp::writeFunctionGetSet (const umlClassNode & node,
                                      const umlOperation & ope,
                                      Visibility & curr_visibility) {
    writeFunctionGetSet1 (node, ope, curr_visibility);
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
GenerateCodeCpp::writeClassComment (const std::string & nom) {
    if (!nom.empty ()) {
        getFile () << spc () << "/**\n";
        getFile () << comment (nom,
                               std::string (spc () + " * \\brief "),
                               std::string (spc () + " *        "),
                               "\n");
        getFile () << spc () << "*/\n";
    }
}

void
GenerateCodeCpp::writeClassStart (const umlClassNode & node) {
    if (!node.getTemplates ().empty ()) {
#ifdef ENABLE_CORBA
        if (isCorba) {
            std::cerr << "CORBAValue " << name << ": template ignored.\n";
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
            getFile () << visibility ("Class \"" + node.getName () + "\"",
                                      (*parent).second) << " "
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
GenerateCodeCpp::writeAttribute (const umlClassNode & node,
                                 const umlAttribute & attr,
                                 Visibility & curr_visibility,
                                 const std::string & nameClass) {
    if (!attr.getValue ().empty ()) {
        std::cerr << "Class \"" << nameClass << "\", attribute \""
                  << attr.getName ()
                  << "\": default value for attribut in class is not applicable with this generator.\n";
    }

    incIndentLevel ();
    check_visibility ("Class \"" + node.getName () + "\", attribute \""
                                 + attr.getName () + "\"",
                      curr_visibility,
                      attr.getVisibility ());
    if (!attr.getComment ().empty ()) {
        getFile () << comment (attr.getComment (),
                               std::string (spc () + "/// "),
                               std::string (spc () + "/// "),
                               "\n");
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
GenerateCodeCpp::writeConst1 (const umlClassNode & node,
                              const char * constAbbr) {
    if (node.getAttributes ().size () != 1) {
        throw std::string ("Error: first attribute not set at " +
                           node.getName () + "\n");
    }

    const umlAttribute & umla = *node.getAttributes ().begin ();

    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
    if (!umla.getName ().empty ()) {
        std::cerr << "Ignoring attribute name at " << node.getName () << ".\n";
    }

    getFile () << spc () << constAbbr << cppName (umla.getType ()) << " "
               << node.getName () << " = " << umla.getValue () << ";\n";
}

void
GenerateCodeCpp::writeConst (const umlClassNode & node) {
    writeConst1 (node, "const ");
}

void
GenerateCodeCpp::writeEnum1 (const umlClassNode & node,
                             const char * enumAbbr) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// " << node.getComment () << "\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << enumAbbr << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << enumAbbr << node.getName () << " {\n";
    }
    incIndentLevel ();
    while (umla != node.getAttributes ().end ()) {
        const char *literal = (*umla).getName ().c_str ();
        if (!(*umla).getComment ().empty ()) {
            getFile () << spc () << "/// " << (*umla).getComment () << "\n";
        }
        (*umla).check (node);
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
GenerateCodeCpp::writeEnum (const umlClassNode & node) {
    writeEnum1 (node, "enum ");
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
        umla.check (node);
        // Use of a tmp value to ignore visibility.
        Visibility vis = umla.getVisibility ();
        writeAttribute (node, umla, vis, node.getName ());
    }
    for (const umlOperation & umlo : node.getOperations ()) {
        if (umlo.getVisibility () != Visibility::PUBLIC) {
            std::cerr << "Struct " << node.getName () << ", operation "
                      << umlo.getName ()
                      << ": visibility forced to visible.\n";
        }
        // Use of a tmp value to ignore visibility.
        Visibility vis = umlo.getVisibility ();
        writeFunction (node, umlo, vis);
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

    (*umla).check (node);

    const umlClassNode * umlc = findByName (getDia ().getUml (),
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
GenerateCodeCpp::writeAssociation (const umlClassNode & node,
                                   const umlassoc & asso,
                                   Visibility & curr_visibility) {
    if (!asso.name.empty ()) {
        incIndentLevel ();
        const umlClassNode *ref;
        ref = findByName (getDia ().getUml (),
                          asso.key.getName ().c_str ());
        check_visibility ("Class \"" + node.getName () + "\"",
                          curr_visibility,
                          asso.visibility);
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
        const umlClassNode * umlc = findByName (getDia ().getUml (),
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
