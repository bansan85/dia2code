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

#include <iostream>
#include <stdexcept>

#include "GenerateCodePhp.hpp"
#include "scan_tree.hpp"

GenerateCodePhp::GenerateCodePhp (DiaGram & diagram) :
    GenerateCodeJava (diagram) {
    setFileExt ("php");
    setHandleIncludePackage (false);
}

std::string
GenerateCodePhp::strPackage (const char * package) const {
    std::string retour;

    if (package[0] != '\\') {
        retour.append ("\\");
    }
    retour.append (package);
    retour.append ("\\");

    return retour;
}

std::string
GenerateCodePhp::getConstructorName(const std::string &) const {
    return "__construct";
}

std::string
GenerateCodePhp::getDestructorName(const std::string &) const {
    return "__destruct";
}

void
GenerateCodePhp::writeStartHeader (std::string &) {
    getFile () << "<?php" << std::endl;
}

void
GenerateCodePhp::writeEndHeader () {
    getFile () << "?>\n";
}

bool
GenerateCodePhp::writeInclude (const std::list <std::pair <
                                                      std::list <umlPackage *>,
                                             const umlClassNode * > > & name) {
    return writeInclude1 (name,
                          "require_once '", "';\n",
                          "require_once '", "';\n",
                          true);
}

void
GenerateCodePhp::writeInclude (const char * name) {
    getFile () << spc () << "require_once '" << name << "';\n";
}

void
GenerateCodePhp::writeFunctionComment (const umlOperation & ope) {
    writeFunctionComment1 (ope, true, '$');
}

void
GenerateCodePhp::writeFunction (const umlClassNode & node,
                                const umlOperation & ope,
                                Visibility & curr_visibility) {
    writeFunction1 (node, ope, curr_visibility);

    if (ope.isStereotypeDllExport ()) {
        std::cerr << "Class \"" << node.getName () << "\", "
                  << "Operation \"" << ope.getName ()
                  << "\": this generator doesn't support DllExport feature.\n";
    }

    getFile () << spc ();
    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::FINAL) {
        std::cerr << "Class \"" << node.getName () << "\", operation \""
                  << ope.getName ()
                  << "\": this generator doesn't support final operation."
                  << " Default: virtual." << std::endl;
    }
    getFile () << visibility ("Class, \"" + node.getName () +
                                    "\", operation \"" + ope.getName () + "\"",
                              ope.getVisibility ()) << " ";

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
    getFile () << "function ";
    writeFunction2 (node, ope, curr_visibility, false, false, '$');
}

void
GenerateCodePhp::writeClassStart (const umlClassNode & node) {
    writeClassStart1 (node, false);
    writeClassStart2 (node, " extends ", " implements ", true);
}

void
GenerateCodePhp::writeAttributeComment (const umlAttribute & attr) {
    if (!attr.getComment ().empty ()) {
        getFile () << comment (attr.getComment (),
                               std::string (spc () + "/** @var " +
                                            attr.getType () + " "),
                               std::string (spc () + " *       "),
                               " */\n");
    }
}

void
GenerateCodePhp::writeAttribute (const umlClassNode & node,
                                 const umlAttribute & attr,
                                 Visibility & curr_visibility) {
    writeAttributeComment (attr);
    writeAttribute1 (node,
                     attr,
                     curr_visibility,
                     false,
                     node.isStereotypeConst () ? "" : "$",
                     "const");
}

void
GenerateCodePhp::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage *> pkglist;
        std::list <umlPackage *>::const_iterator it;

        umlPackage::makePackageList (node->getPackage (), pkglist);
        getFile () << spc () << "namespace ";
        it = pkglist.begin ();
        while (it != pkglist.end ()) {
            getFile () << (*it)->getName ();
            ++it;
            if (it != pkglist.end ()) {
                getFile () << "/";
            }
        }
        getFile () << spc () << " {\n";
        incIndentLevel ();
    }
}

void
GenerateCodePhp::writeNameSpaceEnd (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        decIndentLevel ();
        getFile () << spc () << "}\n";
    }
}

void
GenerateCodePhp::writeEnum (const umlClassNode & node) {
    int32_t val = 0;

    writeClassComment (node.getComment ());
    getFile () << spc () << "class " << node.getName () << " extends SplEnum";
    if (getOpenBraceOnNewline ()) {
        getFile () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << " {\n";
    }
    incIndentLevel ();
    if (!node.getAttributes ().empty ()) {
        getFile () << spc () << "const __default = self::"
                   << (*node.getAttributes ().begin ()).getName () << ";\n";
        getFile () << "\n";
    }
    for (const umlAttribute & umla : node.getAttributes ()) {
        umla.check (node);

        writeClassComment (umla.getComment ());
        getFile () << spc () << "const " << umla.getName ();
        if (!umla.getValue ().empty ()) {
            getFile () << " = " << umla.getValue ();
            try {
                val = std::stoi (umla.getValue ()) + 1;
            }
            catch (const std::invalid_argument &) {
            }
        }
        else {
            getFile () << " = " << val;
            val++;
        }
        getFile () << ";\n";
    }
    decIndentLevel ();
    getFile () << spc () << "}\n";
}

void
GenerateCodePhp::writeTypedef (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (umla == node.getAttributes ().end ()) {
        std::cerr << "Error: first attribute (impl type) not set at typedef "
            << node.getName () << "." << std::endl;
        return;
    }

    (*umla).check (node);

    getFile () << spc () << "use ";
    const umlClassNode * umlc = findByName (getDia ().getUml (),
                                            (*umla).getType ());
    if (umlc == NULL) {
        getFile () << cppName ((*umla).getType ());
    }
    else {
        getFile () << fqname (*umlc, false);
    }
    getFile () << (*umla).getValue () << " as " << cppName (node.getName ())
               << ";\n";
}

void
GenerateCodePhp::writeAssociation (const umlClassNode & node,
                                   const umlassoc & asso,
                                   Visibility &) {
    if (!asso.name.empty ()) {
        getFile () << spc ()
                   << visibility ("Class \"" + node.getName () +
                                    "\", association \"" + asso.name + "\"",
                                  asso.visibility)
                   << " " << fqname (*asso.key, false) << " $" << asso.name
                   << ";\n";
    }
}

void
GenerateCodePhp::writeTemplates (const std::list <std::pair <std::string,
                                                            std::string> > &) {
}

GenerateCodePhp::~GenerateCodePhp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
