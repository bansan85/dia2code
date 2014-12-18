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

#include "GenerateCodeCpp.hpp"

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

void
GenerateCodeCpp::writeLicense () {
    if (getLicense ().empty ()) {
        return;
    }

    getFile () << "/*\n";
    writeLicenseAll ();
    getFile () << "*/\n\n";
}

void
GenerateCodeCpp::writeStartHeader (std::string & name) {
    getFile () << spc () << "#ifndef " << name << "__HPP\n";
    getFile () << spc () << "#define " << name << "__HPP\n\n";
}

void
GenerateCodeCpp::writeEndHeader () {
    getFile () << "#endif\n";
}

void
GenerateCodeCpp::writeInclude (std::basic_string <char> name) {
    getFile () << "#include \"" << name << "\"\n";
}

void
GenerateCodeCpp::writeInclude (const char * name) {
    getFile () << "#include \"" << name << "\"\n";
}

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
