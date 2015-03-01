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


#ifndef GENERATE_CODE_JAVA_HPP
#define GENERATE_CODE_JAVA_HPP

#include "config.h"

#include "GenerateCode.hpp"

class GenerateCodeJava : public GenerateCode {
    public:
        GenerateCodeJava (DiaGram & diagram);

        std::string strPackage (const char * package) const;
        std::string strPointer (const std::string & type) const;
        const char * visibility (std::string desc, const Visibility & vis);

        void writeLicense ();
        void writeStartHeader (std::string & name);
        void writeEndHeader ();
        bool writeInclude (const std::list <std::pair <std::list <umlPackage*>,
                                                    umlClassNode * > > & name);
        void writeInclude (const char * name);
        void writeFunctionComment (const umlOperation & ope);
        void writeFunction (const umlClassNode & node,
                            const umlOperation & ope,
                            Visibility & currVisibility);
        void writeFunctionGetSet (const umlClassNode & node,
                                  const umlOperation & ope,
                                  Visibility & currVisibility);
        void writeComment (const std::string & text);
        void writeComment (const char * text);
        void writeClassComment (const std::string & nom);
        void writeClassStart (const umlClassNode & node);
        void writeClassEnd ();
        void writeAttributeComment (const umlAttribute & attr);
        void writeAttribute (const umlClassNode & node,
                             const umlAttribute & attr,
                             Visibility & currVisibility,
                             const std::string & nameClass);
        void writeNameSpaceStart (const umlClassNode * node);
        void writeNameSpaceEnd (const umlClassNode * node);
        void writeConst (const umlClassNode & node);
        void writeEnum (const umlClassNode & node);
        void writeStruct (const umlClassNode & node);
        void writeTypedef (const umlClassNode & node);
        void writeAssociation (const umlClassNode & node,
                               const umlassoc & asso,
                               Visibility & currVisibility);
        void writeTemplates (
               const std::list <std::pair <std::string, std::string> > & tmps);
        
        void writeFunctionComment1 (const umlOperation & ope,
                                    bool showType,
                                    const char prefixName);
        void writeFunction1 (const umlClassNode & node,
                             const umlOperation & ope,
                             Visibility & currVisibility);
        void writeFunction2 (const umlClassNode & node,
                             const umlOperation & ope,
                             Visibility & currVisibility,
                             bool defaultParam,
                             bool showType,
                             char prefix);
        void writeClassStart1 (const umlClassNode & node,
                               const char * inheritance,
                               bool compName,
                               bool visible);
        void writeAttribute1 (const umlClassNode & node,
                              const umlAttribute & attr,
                              Visibility & currVisibility,
                              const std::string & nameClass,
                              bool showType,
                              const char * prefix);
        void writeTypedef1 (const umlClassNode & node,
                            const char * extends,
                            bool compName);

        ~GenerateCodeJava ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
