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
    GenerateCode (diagram, "java", true) {
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
GenerateCodeJava::visibility (const Visibility & vis) {
    return visibility1 (vis);
}

void
GenerateCodeJava::writeLicense () {
    writeLicense1 ("/*", " */");
}

void
GenerateCodeJava::writeStartHeader (std::string & name) {
}

void
GenerateCodeJava::writeEndHeader () {
}

bool
GenerateCodeJava::writeInclude (std::pair <std::list <umlPackage *>,
                                umlClassNode * > & name) {
    getFile () << spc () << "import ";

    if (!name.first.empty ()) {
        for (umlPackage * namei : name.first) {
            getFile () << namei->getName () << ".";
        }
    }

    if (name.second == NULL) {
        getFile () << "*;\n";
    }
    else {
        getFile () << name.second->getName () << ";\n";
    }

    return true;
}

void
GenerateCodeJava::writeInclude (const char * name) {
    getFile () << spc () << "import " << name << ";\n";
}

void
GenerateCodeJava::writeFunctionComment (const umlOperation & ope) {
    getFile () << spc () << "/**\n";
    getFile () << comment (ope.getComment (),
                           std::string (spc () + " * "),
                           std::string (spc () + " * "));
    for (const umlAttribute & tmpa2 : ope.getParameters ()) {
        std::string comment_ (tmpa2.getName () + " (" +
                              kindStr (tmpa2.getKind ()) +
                              (tmpa2.getComment ().empty () ?
                                ")" :
                                ") " +
                              tmpa2.getComment ()));
        getFile () << comment (comment_,
                               std::string (spc () + " * @param "),
                               std::string (spc () + " *        "));
    }
    getFile () << spc () << " * @return " << ope.getType () << "\n";
    getFile () << spc () << "*/\n";
}

void
GenerateCodeJava::writeFunction1 (const umlOperation & ope,
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
}

void
GenerateCodeJava::writeFunction2 (const umlOperation & ope,
                                  Visibility & curr_visibility) {
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
GenerateCodeJava::writeFunction (const umlOperation & ope,
                                 Visibility & curr_visibility) {
    writeFunction1 (ope, curr_visibility);

    if (ope.getInheritance () == Inheritance::ABSTRACT) {
        getFile () << "abstract ";
    }
    else if (ope.getInheritance () == Inheritance::FINAL) {
        getFile () << "final ";
    }

    writeFunction2 (ope, curr_visibility);
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
GenerateCodeJava::writeClassComment (const std::string & nom) {
    if (!nom.empty ()) {
        getFile () << spc () << "/**\n";
        getFile () << comment (nom,
                               std::string (spc () + " * "),
                               std::string (spc () + " * "));
        getFile () << spc () << " */\n";
    }
}

void
GenerateCodeJava::writeClassStart (const umlClassNode & node) {
    getFile () << spc () << "public ";
    if (node.isAbstract ()) {
        getFile () << "abstract ";
    }
    getFile () << "class " << node.getName ();

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

    if (!node.getParents ().empty ()) {
        std::list <std::pair <umlClass *, Visibility> >::const_iterator
                                                                        parent;
        parent = node.getParents ().begin ();
        while (parent != node.getParents ().end ()) {
            if ((*parent).second != Visibility::PUBLIC) {
                fprintf (stderr, "Only a public visibility is supported by Java generator.\n");
            }
            getFile () << " extends " << fqname (*(*parent).first, false);
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
                                  Visibility & curr_visibility) {
    writeClassComment (attr.getComment ());
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
GenerateCodeJava::writeNameSpaceStart (const umlClassNode * node) {
    if (node->getPackage () != NULL) {
        std::list <umlPackage*> pkglist;

        umlPackage::makePackageList (node->getPackage (), pkglist);
        std::list <umlPackage *>::const_iterator it = pkglist.begin ();

        getFile () << spc () << "package ";
        while (it != pkglist.end ()) {
            getFile () << (*it)->getName ();
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
    writeClassComment (node.getComment ());
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
        writeClassComment ((*umla).getComment ());
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
    getFile () << spc () << "class " << cppName (node.getName ())
               << " extends ";
    const umlClassNode * umlc = findByName (getDia ().getUml (),
                                            (*umla).getType ().c_str ());
    if (umlc == NULL) {
        getFile () << cppName ((*umla).getType ());
    }
    else {
        getFile () << fqname (*umlc, false);
    }
    getFile () << (*umla).getValue () << " {}\n";
}

void
GenerateCodeJava::writeAssociation (const umlassoc & asso,
                                    Visibility & curr_visibility) {
    if (!asso.name.empty ()) {
        getFile () << spc () << visibility (asso.visibility) << " "
                   << cppName (asso.key.getName ());
        getFile () << " " << asso.name << ";\n";
    }
}

void
GenerateCodeJava::writeTemplates (
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

GenerateCodeJava::~GenerateCodeJava () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
