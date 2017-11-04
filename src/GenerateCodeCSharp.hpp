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


#ifndef GENERATE_CODE_CSHARP_HPP
#define GENERATE_CODE_CSHARP_HPP

#include "config.h"

#include "GenerateCodeJava.hpp"

class GenerateCodeCSharp : public GenerateCodeJava {
    public:
        GenerateCodeCSharp (DiaGram & diagram);

        virtual std::string getDestructorName(const std::string & name) const;
        bool writeInclude (const std::list <std::pair <std::list <umlPackage*>,
                                              const umlClassNode * > > & name);
        void writeFunctionComment (const umlOperation & ope);
        void writeFunction (const umlClassNode & node,
                            const umlOperation & ope,
                            Visibility & curr_visibility);
        void writeFunctionGetSet (const umlClassNode & node,
                                  const umlOperation & ope,
                                  Visibility & curr_visibility);
        void writeClassComment (const std::string & nom);
        void writeClassStart (const umlClassNode & node);
        void writeAttribute (const umlClassNode & node,
                             const umlAttribute & attr,
                             Visibility & currVisibility);
        void writeNameSpaceStart (const umlClassNode * node);
        void writeNameSpaceEnd (const umlClassNode * node);
        void writeStruct (const umlClassNode & node);
        void writeTypedef (const umlClassNode & node);
        void writeTemplates ( const std::list <std::pair
                                          <std::string, std::string> > & tmps);

        void writeClassStart2 (const umlClassNode & node,
                               const char * inheritance,
                               const char * implement,
                               bool compName);

        ~GenerateCodeCSharp ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
