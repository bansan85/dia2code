/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2017 Oliver M. Kellogg

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

#include "GenerateCodeAda.hpp"
#include "string2.hpp"
#include "scan_tree.hpp"

GenerateCodeAda::GenerateCodeAda (DiaGram & diagram) :
    GenerateCode (diagram, "ads", 12,  // using Ada2012 only because of "in out" for function parameters
                  true, true), m_needPrivatePart (false)
{
}

std::string
GenerateCodeAda::strPointer (const std::string & type) const {
    std::string retour (type);
    retour.append (" *");
    return retour;
}

std::string
GenerateCodeAda::strPackage (const char * package) const {
    std::string retour (package);
    retour.append (".");
    return retour;
}

const char *
GenerateCodeAda::visibility (std::string desc,
                              const Visibility & vis) {
    return visibility1 (desc, vis);
}

void
GenerateCodeAda::writeLicense () {
    // TODO: Ada does not have multi line comment, just single line
    // writeLicense1 ("/*", " */");
}

void
GenerateCodeAda::check_visibility (std::string desc,
                                   Visibility & curr_vis,
                                   const Visibility new_vis) {
    if (curr_vis == new_vis) {
        return;
    }
    decIndentLevel ();
    getFile () << spc () << visibility (desc, new_vis) << "\n";
    curr_vis = new_vis;
    incIndentLevel ();
}

void
GenerateCodeAda::writeStartHeader (std::string &) {
}

void
GenerateCodeAda::writeEndHeader () {
}

bool
GenerateCodeAda::writeInclude (const std::list <std::pair <
                                                      std::list <umlPackage *>,
                                             const umlClassNode * > > & name) {
    bool ret = false;
    std::list <std::string> incs;

    for (const std::pair <std::list <umlPackage *>, const umlClassNode *> &
                                                                   it : name) {
        std::string include;

        if (!it.first.empty ()) {
            for (umlPackage * namei : it.first) {
                include.append (namei->getName ());
                include.append (".");
            }
        }

        if (it.second == NULL) {
            include.append ("*");
        }
        else {
            include.append (it.second->getName ());
        }

        bool found = false;
        std::list <std::string>::iterator inc = incs.begin ();
        size_t taille = include.length ();

        while (inc != incs.end ()) {
            size_t taille2 = (*inc).length ();

            if (taille == taille2) {
                if (include.compare (*inc) == 0) {
                    found = true;
                }
                ++inc;
            }
            else {
                // Check if include = "pack.class" or "pack.pack2.*" and
                // *inc = "pack.*"
                // if so, no need to include.
                if (taille > taille2) {
                    if ((include.compare (0,
                                          taille2 - 1,
                                          *inc,
                                          0,
                                          taille2 - 1) == 0) &&
                        ((*inc)[taille2 - 1] == '*')) {
                        found = true;
                    }
                    ++inc;
                }
                // Check if include = "pack.*" and
                // *inc = "pack.class"
                // if so, remove *inc.
                else {
                    if ((include.compare (0,
                                          taille - 1,
                                          *inc,
                                          0,
                                          taille - 1) == 0) &&
                        (include[taille - 1] == '*')) {
                        inc = incs.erase (inc);
                    }
                    else {
                        ++inc;
                    }
                }
            }
        }
        if (!found) {
            incs.push_back (include);
        }
    }

    for (std::string it : incs) {
        getFile () << spc () << "with " << it << ";\n";
    }

    if (!incs.empty ()) {
        getFile () << "\n";
    }

    return ret;
}

void
GenerateCodeAda::writeInclude (const char * name) {
    getFile () << spc () << "with " << name << ";\n";
}

void
GenerateCodeAda::writeFunctionComment (const umlOperation & ope) {
    getFile () << spc () << "--\n";
    getFile () << comment (ope.getComment (),
                           std::string (spc () + "-- "),
                           std::string (spc () + "-- "),
                           "\n");
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        std::string comment_;

        comment_.append (tmpa2.getName ());

        comment_.append (" (" + std::string (kindStr (tmpa2.getKind ())) +
                         (tmpa2.getComment ().empty () ?
                           ")" :
                           ") " +
                         tmpa2.getComment ()));
        getFile () << comment (comment_,
                               std::string (spc () + "-- @param "),
                               std::string (spc () + "--        "),
                               "\n");
    }
    if (!ope.getType ().empty ()) {
        getFile () << spc () << "-- @return " << ope.getType () << "\n";
    }
    // getFile () << spc () << "--\n";
}

void
GenerateCodeAda::writeFunction1 (const umlClassNode &,
                                  const umlOperation & ope,
                                  Visibility &) {
#ifdef ENABLE_CORBA
    if (getCorba ()) {
        if (ope.getVisibility () != '0') {
            std::cerr << "CORBAValue " << ope.getName ()
                      << ": must be public.\n";
        }
    }
#endif

    /* print comments on operation */
    if (!ope.getComment ().empty ()) {
        writeFunctionComment (ope);
    }
}

void
GenerateCodeAda::writeFunction (const umlClassNode & node,
                                 const umlOperation & ope,
                                 Visibility & currVisibility) {
    writeFunction1 (node, ope, currVisibility);

    if (ope.isStereotypeDllExport ()) {
        std::cerr << "Class \"" << node.getName () << "\", "
                  << "Operation \"" << ope.getName ()
                  << "\": this generator doesn't support DllExport feature.\n";
    }

    if (ope.getVisibility () != Visibility::PUBLIC)
        getFile () << spc () << "-- vis: " << visibility ("", ope.getVisibility ()) << "\n";
    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << spc () << "-- inheritance: abstract\n";
    } else if (ope.getInheritance () == Inheritance::FINAL) {
        getFile () << spc () << "-- inheritance: final\n";
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
            getFile () << spc () << "-- static\n";
        }
    }

    bool isFunction = false;
    if (ope.getType ().empty () || ope.getType () == "void") {
        getFile () << spc() << "procedure ";
    } else {
        getFile () << spc() << "function ";
        isFunction = true;
    }
    getFile () << ope.getName ();
    if (!ope.isStatic ()) {
        getFile () << " (Self : in ";
        if (!ope.isConstant ())
            getFile () << "out ";
        getFile () << "Object";
    }

    if (!ope.getParameters ().empty ()) {
        if (ope.isStatic ())
            getFile () << " (";
        else
            getFile () << "; ";
        std::list <umlAttribute>::const_iterator tmpa;
        tmpa = ope.getParameters ().begin ();
        while (tmpa != ope.getParameters ().end ()) {
            getFile () << (*tmpa).getName () << " : in ";
            if (!(*tmpa).isConstant ())
                getFile () << "out ";
            getFile () << (*tmpa).getType ();
            if (!(*tmpa).getValue ().empty ()) {
                getFile () << " := " << (*tmpa).getValue ();
            }
            ++tmpa;
            if (tmpa != ope.getParameters ().end ()) {
                getFile () << "; ";
            }
        }
    }
    if (!(ope.isStatic () && ope.getParameters ().empty ()))
        getFile () << ")";
    if (isFunction)
        getFile () << " return " << cppName (ope.getType ());
    if (node.isStereotypeInterface () || ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << " is abstract";
    }
    getFile () << ";\n\n";
}

void
GenerateCodeAda::writeFunctionGetSet (const umlClassNode & node,
                                       const umlOperation & ope,
                                       Visibility & currVisibility) {
    writeFunctionGetSet1 (node, ope, currVisibility);
}

void
GenerateCodeAda::writeComment (const std::string & text) {
    getFile () << spc () << "-- " << text << "\n";
}

void
GenerateCodeAda::writeComment (const char * text) {
    getFile () << spc () << "-- " << text << "\n";
}

void
GenerateCodeAda::writeClassComment (const std::string & nom) {
    if (!nom.empty ()) {
        getFile () << spc () << "--\n";
        getFile () << comment (nom,
                               std::string (spc () + "-- "),
                               std::string (spc () + "-- "),
                               "\n");
        // getFile () << spc () << "--\n";
    }
}

void
GenerateCodeAda::writeClassStart (const umlClassNode & node) {
    if (node.isStereotypeDllExport ()) {
        std::cerr << "Class \"" << node.getName ()
                  << "\": this generator doesn't support DllExport feature.\n";
    }

    incIndentLevel ();
    m_node = &node;
    m_needPrivatePart = true;
    m_classDecl = "type Object is";
    if (node.isStereotypeInterface ()) {
        m_classDecl += " interface";
        m_needPrivatePart = false;
    } else if (node.isAbstract ()) {
        m_classDecl += " abstract";
    }
    if (!node.getParents ().empty ()) {
        umlClass *nonInterface = nullptr;
        std::list<umlClassNode::ClassAndVisibility>::const_iterator parent;
        parent = node.getParents ().begin ();
        while (parent != node.getParents ().end ()) {
            if (!(*parent).first->isStereotypeInterface ()) {
                nonInterface = (*parent).first;
                break;
            }
            ++parent;
        }
        if (nonInterface) {
            if (node.isStereotypeInterface ()) {
                std::cerr << "Class \"" << node.getName ()
                          << "\": <<interface>> cannot extend non <<interface>>\n";
                decIndentLevel ();
                return;
            }
            m_classDecl.append (" new ").append (fqname (*nonInterface)).append (".Object");
        }
        parent = node.getParents ().begin ();
        bool firstParent = true;
        while (parent != node.getParents ().end ()) {
            if ((*parent).second != Visibility::PUBLIC) {
                std::cerr << "Class \"" << node.getName ()
                          << "\", inheritance \""
                          << (*parent).first->getName ()
                          << "\": only a public visibility is supported.\n";
            }
            umlClass *p = (*parent).first;
            if (!node.isStereotypeInterface () && !nonInterface && firstParent) {
                m_classDecl.append (" new ").append (fqname (*p)).append (".Object");
            } else if (p->isStereotypeInterface ()) {
                m_classDecl.append (" and ").append (fqname (*p)).append (".Object");
            } else if (p != nonInterface) {
                std::cerr << "Class \"" << node.getName ()
                          << "\": Ada does not support multiple inheritance from non interface ("
                          << p->getName () << ")\n";
            }
            firstParent = false;
            ++parent;
        }
        if (!node.isStereotypeInterface ()) {
            m_classDecl += " with";
            if (node.getAttributes ().empty ()) {
                m_classDecl += " null record";
                m_needPrivatePart = false;
            } else {
                m_classDecl += " private";
            }
        }
    } else if (!node.isStereotypeInterface ()) {
        m_classDecl += " tagged";
        if (node.getAttributes ().empty ()) {
            m_classDecl += " null record";
            m_needPrivatePart = false;
        } else {
            m_classDecl += " private";
        }
    }
    getFile () << spc () << m_classDecl << ";\n\n";
    decIndentLevel ();
}

void
GenerateCodeAda::writeClassEnd () {
    if (!m_needPrivatePart)
        return;

    getFile () << spc () << "private\n\n";
    incIndentLevel ();
    // Check for non public static attributes
    for (const umlAttribute & umla : m_node->getAttributes ()) {
        if (!umla.isStatic () || umla.getVisibility () == Visibility::PUBLIC)
            continue;
        getFile () << spc () << "-- static\n";
        getFile () << spc () << umla.getName () << " : " << umla.getType ();
        if (!umla.getValue ().empty ())
            getFile () << " := " << umla.getValue ();
        getFile () << ";\n\n";
    }
    // Generate full view of class
    const size_t len = m_classDecl.length ();
    if (len > 8 && m_classDecl.substr (len - 8) == " private")
        m_classDecl.erase (len - 8);
    getFile () << spc () << m_classDecl << " record\n";
    incIndentLevel ();
    for (const umlAttribute & attr : m_node->getAttributes ()) {
        if (attr.isStatic ())
            continue;
        getFile () << spc () << attr.getName () << " : " << attr.getType ();
        if (!attr.getValue ().empty ())
            getFile () << " := " << attr.getValue ();
        getFile () << ";\n";
    }
    decIndentLevel ();
    getFile () << spc () << "end record;\n\n";
    decIndentLevel ();
}


void
GenerateCodeAda::writeAttributeComment (const umlAttribute & attr) {
    writeClassComment (attr.getComment ());
}

void
GenerateCodeAda::writeAttribute (const umlClassNode &,
                                  const umlAttribute & attr,
                                  Visibility &) {
    writeAttributeComment (attr);
    if (attr.getVisibility () != Visibility::PUBLIC) {
        getFile () << spc () << "-- " << attr.getName () << " visibility: "
                   << visibility ("", attr.getVisibility ()) << "\n\n";
        return;
    }
    if (attr.isStatic ()) {
        getFile () << spc () << "-- static\n";
        getFile () << spc () << attr.getName () << " : " << attr.getType ();
        if (!attr.getValue ().empty ())
            getFile () << " := " << attr.getValue ();
        getFile () << ";\n\n";
        return;
    }
    const std::string upperFirst = strtoupperfirst (attr.getName ());
    getFile () << spc () << "procedure Set_" << upperFirst
               << " (Self : in out Object; Value : in " << attr.getType () << ");\n";
    getFile () << spc () << "function  Get_" << upperFirst
               << " (Self : in Object) return " << attr.getType () << ";\n";
    getFile () << "\n";
}

void
GenerateCodeAda::writeNameSpaceStart (const umlClassNode * node) {
    if (!node->getTemplates ().empty ()) {
        getFile () << "generic\n";
        getFile () << "   -- ";
        writeTemplates (node->getTemplates ());
        getFile () << "\n";
    }
    getFile () << "package " << fqname (*node) << " is\n\n";
}

void
GenerateCodeAda::writeNameSpaceEnd (const umlClassNode *node) {
    getFile () << "end " << fqname (*node) << ";\n\n";
}

void
GenerateCodeAda::writeEnum (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    writeClassComment (node.getComment ());
    getFile () << spc () << "type " << node.getName () << " is";
    if (getOpenBraceOnNewline ()) {
        getFile () << "\n";
        getFile () << spc () << "(\n";
    } else {
        getFile () << " (\n";
    }
    incIndentLevel ();
    while (umla != node.getAttributes ().end ()) {
        (*umla).check (node);

        writeClassComment ((*umla).getComment ());
        getFile () << spc () << (*umla).getName ();
        ++umla;
        if (umla != node.getAttributes ().end ()) {
            getFile () << ",";
        }
        getFile () << "\n";
    }
    decIndentLevel ();
    getFile () << spc () << ");\n";
}

void
GenerateCodeAda::writeStruct (const umlClassNode & node) {
    incIndentLevel ();
    if (!node.getComment ().empty ()) {
        getFile () << spc () << "-- " << node.getComment () << "\n";
    }
    getFile () << spc () << "type " << node.getName () << " is record\n";
    incIndentLevel ();
    for (const umlAttribute & umla : node.getAttributes ()) {
        umla.check (node);
        if (umla.isStatic ()) {
            std::cerr << "Struct " << node.getName () << ": Ignoring static " << umla.getName () << "\n";
            continue;
        }
        getFile () << spc () << umla.getName () << " : " << umla.getType ();
        if (!umla.getValue ().empty ())
            getFile () << " := " << umla.getValue ();
        getFile () << ";\n";
    }
    if (!node.getOperations ().empty ()) {
        std::cerr << "Struct " << node.getName () << ": Ada cannot handle operations\n";
    }
    decIndentLevel ();
    getFile () << spc () << "end record;\n\n";
    decIndentLevel ();
}

void
GenerateCodeAda::writeTypedef (const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;

    umla = node.getAttributes ().begin ();
    if (umla == node.getAttributes ().end ()) {
        throw std::string ("Error: first attribute (impl type) not set at typedef " + node.getName () + ".\n");
    }

    (*umla).check (node);

    const umlClassNode * umlc = findByName (getDia ().getUml (),
                                            (*umla).getType ().c_str ());
    getFile () << spc () << "  type " << node.getName () << " is new ";
    if (umlc == NULL) {
        getFile () << cppName ((*umla).getType ());
    } else {
        getFile () << fqname (*umlc);
    }
    getFile () << " " << (*umla).getValue () << ";\n";
}

void
GenerateCodeAda::writeAssociation (const umlClassNode & node,
                                    const umlassoc & asso,
                                    Visibility &) {
    if (!asso.name.empty ()) {
        getFile () << spc ()
                   << visibility ("Class \"" + node.getName () +
                                    "\", association \"" + asso.name + "\"",
                                  asso.visibility)
                   << " " << cppName (asso.key->getName ());
        getFile () << " " << asso.name << ";\n";
    }
}

void
GenerateCodeAda::writeTemplates (
              const std::list <std::pair <std::string, std::string> > & tmps) {
    std::list <std::pair <std::string, std::string> >::const_iterator
                                                     template_ = tmps.begin ();
    getFile () << " <";
    while (template_ != tmps.end ()) {
        getFile () << (*template_).first << " extends ";
        const umlClassNode * umlc = findByName (getDia ().getUml (),
                                                (*template_).second.c_str ());
        if (umlc == NULL) {
            getFile () << cppName ((*template_).second);
        }
        else {
            getFile () << fqname (*umlc, false);
        }
        ++template_;
        if (template_ != tmps.end ()) {
            getFile () << ", ";
        }
    }
    getFile () << ">";
}

GenerateCodeAda::~GenerateCodeAda () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
