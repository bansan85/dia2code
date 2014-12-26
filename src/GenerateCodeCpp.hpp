/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2014-2014 Javier O'Hara - Oliver Kellogg

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


#ifndef GENERATE_CODE_CPP_HPP
#define GENERATE_CODE_CPP_HPP

#include "config.h"

#include "GenerateCode.hpp"

class GenerateCodeCpp : public GenerateCode {
    public:
        GenerateCodeCpp (DiaGram & diagram);

        std::string strPackage (const char * package) const;
        std::string strPointer (const std::string & type) const;
        const char * fqname (const umlClassNode & node, bool use_ref_type);
        void check_visibility (int *curr_vis, int new_vis);

        void writeLicense ();
        void writeStartHeader (std::string & name);
        void writeEndHeader ();
        void writeInclude (std::basic_string <char> name);
        void writeInclude (const char * name);
        void writeFunctionComment (const umlOperation & ope);
        void writeFunction (const umlOperation & ope, int * curr_visibility);
        void writeComment (const std::string & text);
        void writeComment (const char * text);
        void writeClassComment (const umlClassNode & node);
        void writeClassStart (const umlClassNode & node);
        void writeClassEnd (const umlClassNode & node);
        void writeAttribute (const umlAttribute & attr,
                             int * curr_visibility);
        void writeNameSpaceStart (const std::string & name);
        void writeNameSpaceEnd ();
        void writeConst (const umlClassNode & node);
        void writeEnum (const umlClassNode & node);
        
        ~GenerateCodeCpp ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
