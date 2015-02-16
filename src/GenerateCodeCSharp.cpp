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

#include "GenerateCodeCSharp.hpp"
#include "umlClassNode.hpp"
#include "string2.hpp"

GenerateCodeCSharp::GenerateCodeCSharp (DiaGram & diagram) :
    GenerateCodeJava (diagram) {
    setFileExt ("cs");
}

bool
GenerateCodeCSharp::writeInclude (const std::list <std::pair <
                                                   std::list <umlPackage *>,
                                                   umlClassNode * > > & name) {
    bool ret = false;
    std::list <std::string> incs;

    for (const std::pair <std::list <umlPackage *>, umlClassNode *> & it : 
                                                                        name) {
        std::string include;

        if (!it.first.empty ()) {
            std::list <umlPackage *>::const_iterator namei;

            namei = it.first.begin ();
            while (namei != it.first.end ()) {
                include.append ((*namei)->getName ());
                ++namei;
                if (namei != it.first.end ()) {
                    include.append (".");
                }
            }
            // We don't add the name of the class.
            ret = true;
        }
        else if (it.second != NULL) {
            include.append (it.second->getName ());
            ret = true;
        }
        else {
            continue;
        }

        bool found = false;
        std::list <std::string>::const_iterator inc = incs.begin ();
        size_t taille = include.length ();

        while (inc != incs.end ()) {
            std::string search;
            size_t taille2 = (*inc).length ();

            if (taille == taille2) {
                if (include.compare (*inc) == 0) {
                    found = true;
                }
                ++inc;
            }
            // Check if include = "pack.class" or "pack.pack2" and
            // *inc = "pack"
            // if so, no need to include.
            else if (taille > taille2) {
                search = *inc + ".";
                if (include.compare (0, taille2 + 1, search) == 0) {
                    found = true;
                }
                ++inc;
            }
            // Check if include = "pack" and
            // *inc = "pack.class"
            // if so, remove *inc.
            else {
                search = include + ".";
                if (search.compare (0, taille + 1, *inc) == 0) {
                    inc = incs.erase (inc);
                }
                else {
                    ++inc;
                }
            }
        }
        if (!found) {
            incs.push_back (include);
        }
    }

    for (std::string it : incs) {
        getFile () << spc () << "using " << it << ";\n";
    }

    if (!incs.empty ()) {
        getFile () << "\n";
    }

    return ret;
}

void
GenerateCodeCSharp::writeFunctionComment (const umlOperation & ope) {
    getFile () << comment (ope.getComment (),
                           std::string (spc () + "/// <summary>"),
                           std::string (spc () + "/// "),
                           "</summary>\n");
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        std::string comment_ ("<param name=\"" + tmpa2.getName () + "\">(" +
                              kindStr (tmpa2.getKind ()) +
                              (tmpa2.getComment ().empty () ?
                                ")" :
                                ") " +
                              tmpa2.getComment ()));
        getFile () << comment (comment_,
                               std::string (spc () + "/// "),
                               std::string (spc () + "/// "),
                               "</param>\n");
    }
    getFile () << spc () << "/// <returns>" << ope.getType ()
               << "</returns>\n";
}

void
GenerateCodeCSharp::writeFunction (const umlOperation & ope,
                                   Visibility & curr_visibility) {
    writeFunction1 (ope, curr_visibility);

    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::VIRTUAL) {
        getFile () << "virtual ";
    }

    writeFunction2 (ope, curr_visibility, true);
}

void
GenerateCodeCSharp::writeClassComment (const std::string & nom) {
    if (!nom.empty ()) {
        getFile () << comment (nom,
                               std::string (spc () + "/// <summary>"),
                               std::string (spc () + "/// "),
                               "</summary>\n");
    }
}

void
GenerateCodeCSharp::writeClassStart (const umlClassNode & node) {
    writeClassStart1 (node, " : ", false);
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
GenerateCodeCSharp::writeStruct (const umlClassNode & node) {
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "/// <summary>" << node.getComment ()
                   << "</summary>\n";
    }
    if (getOpenBraceOnNewline ()) {
        getFile () << spc () << "public struct " << node.getName () << "\n";
        getFile () << spc () << "{\n";
    }
    else {
        getFile () << spc () << "public struct " << node.getName () << " {\n";
    }
    incIndentLevel ();
    for (const umlAttribute & umla : node.getAttributes ()) {
        if (umla.getName ().empty ()) {
            std::cerr << node.getName () << ": an unamed attribute is found.\n";
        }
        if (umla.getVisibility () != Visibility::PUBLIC) {
            std::cerr << "Struct " << node.getName () << ", attribute "
                      << umla.getName ()
                      << ": visibility forced to visible.\n";
        }
        // Use of a tmp value to ignore visibility.
        Visibility vis = Visibility::PUBLIC;
        const_cast <umlAttribute &> (umla).setVisibility (Visibility::PUBLIC);
        writeAttribute (umla, vis);
    }
    for (const umlOperation & umlo : node.getOperations ()) {
        if (umlo.getVisibility () != Visibility::PUBLIC) {
            std::cerr << "Struct " << node.getName () << ", operation "
                      << umlo.getName ()
                      << ": visibility forced to visible.\n";
        }
        // Use of a tmp value to ignore visibility.
        Visibility vis = Visibility::PUBLIC;
        const_cast <umlOperation &> (umlo).setVisibility (Visibility::PUBLIC);
        writeFunction (umlo, vis);
    }
    decIndentLevel ();
    getFile () << spc () << "};\n";
}

void
GenerateCodeCSharp::writeTypedef (const umlClassNode & node) {
    writeTypedef1 (node, " : ", false);
}

void
GenerateCodeCSharp::writeTemplates (
              const std::list <std::pair <std::string, std::string> > & tmps) {
    std::list <std::pair <std::string, std::string> >::const_iterator
                                                     template_ = tmps.begin ();
    getFile () << " <";
    while (template_ != tmps.end ()) {
        getFile () << (*template_).first;
        ++template_;
        if (template_ != tmps.end ()) {
            getFile () << ", ";
        }
    }
    getFile () << "> ";

    template_ = tmps.begin ();
    while (template_ != tmps.end ()) {
        getFile () << "where " << (*template_).first << " : " 
                   << cppName ((*template_).second);
        ++template_;
        if (template_ != tmps.end ()) {
            getFile () << " ";
        }
    }
}

GenerateCodeCSharp::~GenerateCodeCSharp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
