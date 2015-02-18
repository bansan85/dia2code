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
        const char * visibility (std::string desc, const Visibility & vis);
        void check_visibility (std::string desc,
                               Visibility & curr_vis,
                               Visibility new_vis);

        void writeLicense ();
        void writeStartHeader (std::string & name);
        void writeEndHeader ();
        bool writeInclude (const std::list <std::pair <std::list <umlPackage*>,
                                                    umlClassNode * > > & name);
        void writeInclude (const char * name);
        void writeFunctionComment (const umlOperation & ope);
        void writeFunction (const umlClassNode & node,
                            const umlOperation & ope,
                            Visibility & curr_visibility);
        void writeFunctionGetSet (const umlClassNode & node,
                                  const umlOperation & ope,
                                  Visibility & curr_visibility);
        void writeComment (const std::string & text);
        void writeComment (const char * text);
        void writeClassComment (const std::string & nom);
        void writeClassStart (const umlClassNode & node);
        void writeClassEnd ();
        void writeAttribute (const umlClassNode & node,
                             const umlAttribute & attr,
                             Visibility & curr_visibility,
                             const std::string & nameClass);
        void writeNameSpaceStart (const umlClassNode * node);
        void writeNameSpaceEnd (const umlClassNode * node);
        void writeConst (const umlClassNode & node);
        void writeEnum (const umlClassNode & node);
        void writeStruct (const umlClassNode & node);
        void writeTypedef (const umlClassNode & node);
        void writeAssociation (const umlClassNode & node,
                               const umlassoc & asso,
                               Visibility & curr_visibility);
        void writeTemplates (
               const std::list <std::pair <std::string, std::string> > & tmps);

        void writeFunction1 (const umlOperation & ope);
        void writeFunction2 (const umlOperation & ope);
        void writeFunction3 (const umlOperation & ope);
        void writeConst1 (const umlClassNode & node, const char * constAbbr);
        void writeEnum1 (const umlClassNode & node, const char * enumAbbr);

        ~GenerateCodeCpp ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
