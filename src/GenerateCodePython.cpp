/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright(C) 2015 Vincent Le Garrec

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


/*  Converted from GenerateCodeJava.cpp
    Wolf Ó Spealáin 18 March 2017.
    For Python 3, PEP 8, with type hints.
    Clumsy workaround to implement __init__ method seperately to class attributes without changing parent code:
        writing buffer to initialisation_method_parameters and initialisation_method_assignments,
        writeInit(), check on first_operation in writeFunction() and writeClassEnd()
    Doesn't implement inherited attributes in __init__().
    Parent code could benefit from presenting class attributes separately to instance variables.
    Requires lowercase filenames - added to GenerateCode.cpp.
    Unimplemented code, which may be Java specific, is highlighted with std::cerr << "DEBUG:" output.
    Some rationalisation to functions; but some unused functions left declared with std::cerr << "DEBUG:" output.
*/

#include "config.h"

#include <iostream>

#include "GenerateCodePython.hpp"
#include "string2.hpp"
#include "scan_tree.hpp"

std::string initialisation_method_parameters;
std::string initialisation_method_assignments;
bool first_operation = true;

void
GenerateCodePython::writeInit(){
    getFile() << spc() << "def __init__(self";
    getFile() << initialisation_method_parameters << "):\n";
    getFile() << spc() << spc() << "# Any inherited parameters not implemented in __init__().\n";
    getFile() << initialisation_method_assignments;
    getFile() << spc() << spc() << "return\n\n";
    initialisation_method_assignments = "";
    initialisation_method_parameters = "";
}

GenerateCodePython::GenerateCodePython(DiaGram & diagram) :
    GenerateCode(diagram, "py", 3, true, false) {
}

std::string
GenerateCodePython::strPointer(const std::string & type) const {
    std::string retour(type);
    retour.append(" *");
    return retour;
}

std::string
GenerateCodePython::strPackage(const char * package) const {
    std::string retour(package);
    retour.append(".");
    return retour;
}

const char *
GenerateCodePython::visibility(std::string desc,
                              const Visibility & vis) {
        switch(vis) {
        case Visibility::PUBLIC :
            return "";
        case Visibility::PRIVATE :
            return "_";
        case Visibility::PROTECTED :
            return "__";
        case Visibility::IMPLEMENTATION :
            std::cerr << "DEBUG: " + desc + ": implementation not implemented\n";
            return "";
        default :
            return "";
        }
}

void
GenerateCodePython::writeLicense() {
    writeLicense1("/*", " */");
}

void
GenerateCodePython::writeStartHeader(std::string &) {
}

void
GenerateCodePython::writeEndHeader() {
}

bool
GenerateCodePython::writeInclude(const std::list <std::pair <
                                                      std::list <umlPackage *>,
                                             const umlClassNode * > > & name) {
    bool ret = false;
    std::list <std::string> incs;

    for(const std::pair <std::list <umlPackage *>, const umlClassNode *> &
                                                                   it : name) {
        std::string include;

        if(!it.first.empty()) {
            for(umlPackage * namei : it.first) {
                include.append(namei->getName());
                include.append(".");
            }
        }

        if(it.second == NULL) {
            include.append("*");
        }
        else {
            include.append(it.second->getName());
        }

        bool found = false;
        std::list <std::string>::iterator inc = incs.begin();
        size_t taille = include.length();

        while(inc != incs.end()) {
            size_t taille2 =(*inc).length();

            if(taille == taille2) {
                if(include.compare(*inc) == 0) {
                    found = true;
                }
                ++inc;
            }
            else {
                // Check if include = "pack.class" or "pack.pack2.*" and
                // *inc = "pack.*"
                // if so, no need to include.
                if(taille > taille2) {
                    if((include.compare(0,
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
                    if((include.compare(0,
                                          taille - 1,
                                          *inc,
                                          0,
                                          taille - 1) == 0) &&
                       (include[taille - 1] == '*')) {
                        inc = incs.erase(inc);
                    }
                    else {
                        ++inc;
                    }
                }
            }
        }
        if(!found) {
            incs.push_back(include);
        }
    }

    std::string lowercase;
    for(std::string it : incs) {
        lowercase = it;
        lowercase[0] = tolower(lowercase[0]);
        getFile() << spc() << "from " << lowercase << " import " << it << "\n";
    }

    if(!incs.empty()) {
        getFile() << "\n";
    }

    return ret;
}

void
GenerateCodePython::writeInclude(const char * name) {
    std::cerr << "DEBUG: writeInclude " << name << "not implemented\n";
}

void
GenerateCodePython::writeFunctionComment(const umlOperation & ope) {

    //https://www.python.org/dev/peps/pep-0257/

    getFile() << spc() << spc() << "\"\"\"";
    if(!ope.getComment().empty()){
        getFile() << comment(ope.getComment(), "", spc(), "\n");
    }
    if(!ope.getParameters().empty()){
        getFile() << spc() << spc() << "Keyword Arguments:" << "\n";
    }
    for(const umlAttribute & tmpa2 : ope.getParameters()) {
        std::string comment_;
        comment_.append(" " + tmpa2.getName() + ": ");
        comment_.append(tmpa2.getType());
        if(!tmpa2.getValue().empty()) {
            comment_.append("(default " + tmpa2.getValue() + ")");
        }
        if(!tmpa2.getComment().empty()) {
            comment_.append(" -- " + tmpa2.getComment());
        }
        getFile() << comment(comment_, spc()+spc(), spc(), "\n");
    }
    if(!ope.getType().empty()) {
        getFile() << spc() << spc() << "Returns: " << ope.getType() << "\n";
    }
    getFile() << spc() << spc() << "\"\"\"";
}

void
GenerateCodePython::writeFunction(const umlClassNode & node,
                                 const umlOperation & ope,
                                 Visibility & currVisibility) {

    // check for initialisation method insertion
    if(first_operation){
        writeInit();
        first_operation = false;
    }

    if(ope.isStereotypeDllExport()) {
        std::cerr << "DEBUG: isStereotypeDllExport not implemented\n";
    }
    getFile() << spc() << "def ";
    getFile() << visibility("Class \"" + node.getName() + "\", operator \"" + ope.getName() + "\"", ope.getVisibility());
    getFile() << ope.getName() << "(self, ";

    if(ope.getInheritance() == Inheritance::ABSTRACT) {
        std::cerr << "DEBUG: Inheritance::ABSTRACT not implemented\n";
    }
    else if(ope.getInheritance() == Inheritance::FINAL) {
        //std::cerr << "DEBUG: Inheritance::FINAL not implemented\n";
    }
    if(ope.isStatic()) {
        std::cerr << "DEBUG: isStatic not implemented\n";
    }

    // parameters
    std::list <umlAttribute>::const_iterator tmpa;
    tmpa = ope.getParameters().begin();
    while(tmpa != ope.getParameters().end()) {
        getFile() <<(*tmpa).getName();
        if(!(*tmpa).getType().empty()) {
            getFile() << ": " <<(*tmpa).getType();
        }
        if(!(*tmpa).getValue().empty()) {
            getFile() << " = " <<(*tmpa).getValue();
        }
        ++tmpa;
        if(tmpa != ope.getParameters().end()) {
            getFile() << ", ";
        }
    }
    if(ope.isConstant()) {
        std::cerr << "DEBUG: isConstant not implemented\n";
    }
    if(ope.getInheritance() == Inheritance::ABSTRACT) {
        std::cerr << "DEBUG: Inheritance::ABSTRACT not implemented\n";
    }
    getFile() << ")";
    if((!ope.getType().empty())) {
        getFile() << " -> " << ope.getType();
    }
    getFile() << ":\n";

    // comments
    writeFunctionComment(ope);

    // return
    getFile() << "\n\n" << spc() << spc() << "return\n\n";
}

void
GenerateCodePython::writeFunctionGetSet(const umlClassNode & node,
                                       const umlOperation & ope,
                                       Visibility & currVisibility) {
    writeFunctionGetSet1(node, ope, currVisibility);
}

void
GenerateCodePython::writeComment(const std::string & text) {
    getFile() << spc() << "# " << text << "\n";
}

void
GenerateCodePython::writeComment(const char * text) {
    getFile() << spc() << "# " << text << "\n";
}

void
GenerateCodePython::writeClassComment(const std::string & nom) {
    if(!nom.empty()) {
        getFile() << spc() << "\"\"\"";
        getFile() << comment(nom,spc(),spc(),"\n");
        getFile() << spc() << "\"\"\"\n\n\n";
    }
}

void
GenerateCodePython::writeClassStart(const umlClassNode & node) {
    getFile() << spc() << "class " << node.getName();

    if(!node.getTemplates().empty()) {
        writeTemplates(node.getTemplates());
    }
    if(!node.getParents().empty()) {
        std::list <std::pair <umlClass *, Visibility> >::const_iterator parent;
        getFile() << "(";
        parent = node.getParents().begin();
        while(parent != node.getParents().end()) {
            if(parent != node.getParents().begin()) {
                getFile() << ", ";
            }
            if((*parent).second != Visibility::PUBLIC) {
                std::cerr << "DEBUG: != Visibility::PUBLIC not implemented\n";            }
            if((*parent).first->isStereotypeInterface()) {
                std::cerr << "DEBUG: isStereotypeInterface not implemented\n";
            }
            getFile() << fqname(*(*parent).first, false);
            ++parent;
        }
        getFile() << ")";
    }
    getFile() << ":\n";
}

void
GenerateCodePython::writeClassEnd() {

    // check if any operations have been added
    if(first_operation){
            incIndentLevel ();
            getFile() << "\n";
            writeComment ("Operations:");
            getFile() << "\n";
            writeInit();
            decIndentLevel ();
    }
    first_operation = true;
 }

void
GenerateCodePython::writeAttributeComment(const umlAttribute & attr) {
    std::cerr << "DEBUG: writeAttributeComment not implemented\n";
}

void
GenerateCodePython::writeAttribute(const umlClassNode & node,
                                  const umlAttribute & attr,
                                  Visibility & currVisibility) {
    if(attr.isStatic()) {

        // class attributes only
        getFile() << spc() << visibility("Class \"" + node.getName() + "\", attribute \"" + attr.getName() + "\"", attr.getVisibility());
        getFile() << attr.getName();
        if(!attr.getType().empty()) {
            getFile() << ": " + attr.getType();
        }
        if(!attr.getValue().empty()) {
            getFile() << " = " << attr.getValue();
        }
        if(!attr.getComment().empty()) {
        getFile() << " # " << attr.getComment();
        }
        getFile() << "\n";
    }
    else {

        // buffer initialisation method parameters
        std::string underscore(visibility("Class \"" + node.getName() + "\", attribute \"" + attr.getName() + "\"", attr.getVisibility()));
        initialisation_method_parameters += ", " + underscore;
        initialisation_method_parameters += attr.getName();
        if(!attr.getType().empty()) {
            initialisation_method_parameters += ": " + attr.getType();
        }
        if(!attr.getValue().empty()) {
            initialisation_method_parameters += " = " + attr.getValue();
        }
        initialisation_method_assignments += spc() + spc() + "self." + underscore + attr.getName();
        initialisation_method_assignments += " = " + underscore + attr.getName();
        if(!attr.getComment().empty()) {
            initialisation_method_assignments += " # " + attr.getComment();
        }
        initialisation_method_assignments += "\n";
    }
    if(node.isStereotypeConst()) {
        std::cerr << "DEBUG: isStereotypeConst not implemented\n";
    }
}

void
GenerateCodePython::writeNameSpaceStart(const umlClassNode * node) {
    //std::cerr << "DEBUG: writeNameSpaceStart " << node << " not implemented\n";
}

void
GenerateCodePython::writeNameSpaceEnd(const umlClassNode *) {
   // std::cerr << "DEBUG: writeNameSpaceEnd not implemented\n";
}

void
GenerateCodePython::writeEnum(const umlClassNode & node) {
    std::list <umlAttribute>::const_iterator umla;
    std::cerr << "DEBUG: writeEnum not implemented\n";
}

void
GenerateCodePython::writeStruct(const umlClassNode & node) {
    umlClassNode & nodetmp = const_cast <umlClassNode &>(node);
    std::cerr << "DEBUG: writeStruct not implemented\n";
}

void
GenerateCodePython::writeTypedef(const umlClassNode & node) {
    std::cerr << "DEBUG: writeTypedef not implemented\n";
}

void
GenerateCodePython::writeAssociation(const umlClassNode & node,
                                    const umlassoc & asso,
                                    Visibility &) {
    //std::cerr << "DEBUG: writeAssociation not implemented\n";
}

void
GenerateCodePython::writeTemplates(
              const std::list <std::pair <std::string, std::string> > & tmps) {
    std::list <std::pair <std::string, std::string> >::const_iterator
                                                     template_ = tmps.begin();
    std::cerr << "DEBUG: writeTemplates not implemented\n";
}

GenerateCodePython::~GenerateCodePython() {
}
