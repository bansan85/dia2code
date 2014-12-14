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

#include <iostream>

#include "GenerateCodeCpp.hpp"

#define SPEC_EXT "h"
#define BODY_EXT "cpp"

#define eq  !strcmp

GenerateCodeCpp::GenerateCodeCpp (DiaGram & diagram) :
    GenerateCode (diagram, "hpp") {
}

void
GenerateCodeCpp::writeLicense () {
    FILE *licensefile = NULL;

    if (getLicense ().empty ()) {
        return;
    }

    licensefile = fopen (getLicense ().c_str (), "r");
    if (!licensefile) {
        fprintf (stderr, "Can't open the license file.\n");
        exit (1);
    }

    getFile () << "/*\n";
    int lc;
    rewind (licensefile);
    while ((lc = fgetc (licensefile)) != EOF) {
        getFile () << static_cast <char> (lc);
    }
    getFile () << "*/\n\n";

    fclose (licensefile);
}

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
