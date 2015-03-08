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

#ifndef GENERATE_CODE_HPP
#define GENERATE_CODE_HPP

#include "config.h"

#include <fstream>
#include <cstdint>

#include "DiaGram.hpp"
#include "umlClass.hpp"
#include "umlAssoc.hpp"

class GenerateCode {
    private:
        // Diagram to generate into code.
        DiaGram &   dia;
        // License file.
        std::string license;
        // Output directory.
        std::string outdir;
        std::string file_ext;
        std::string body_file_ext;
        std::list <std::ofstream *> file;
        uint8_t     version;
        uint8_t     indent : 4;
        uint8_t     indentlevel : 3;
        // Overwrite files while generating code.
        bool        overwrite : 1;
        // Convert package names to a directory tree.
        bool        buildtree : 1;
        bool        bOpenBraceOnNewline : 1;
        bool        oneClassOneHeader : 1;
        // If the language handles include package. If not, include ALL classes
        // inside the package and their children.
        bool        handleIncludePackage : 1;
#ifdef ENABLE_CORBA
        bool        isCorba : 1;
#endif

        bool passByReference (umlClass &cl) const;
        void openOutfile (const std::string & filename, declaration & d);
        void closeOutfile ();
        void genDecl (declaration &d, bool forceOpen);
    public:
        GenerateCode (DiaGram & diagram,
                      const char * ext,
                      uint8_t version_,
                      bool handleIncludePackage_);

        DiaGram & getDia ();
        void generate_code ();
        
        const char * getFileExt () const;
        void         setFileExt (const char * ext);

        const char * getBodyFileExt () const;
        void         setBodyFileExt (const char * ext);

        std::ofstream & getFile ();

        uint8_t getVersion () const;
        void    setVersion (uint8_t version_);
        
        uint32_t getIndent () const;
        void     setIndent (uint8_t spaces);
        void     incIndentLevel ();
        void     decIndentLevel ();

        const std::string & getLicense () const;
        void                setLicense (const char * lic);

        const char * getOutdir () const;
        const        std::string * getOutdirS () const;
        void         setOutdir (const char * dir);

        bool getOverwrite () const;
        void setOverwrite (bool over);

        bool getBuildTree () const;
        void setBuildTree (bool build);

        bool getOpenBraceOnNewline () const;
        void setOpenBraceOnNewline (bool newline);

        bool getOneClass () const;
        void setOneClass (bool val);

        void setHandleIncludePackage (bool val);

#ifdef ENABLE_CORBA
        bool getCorba () const;
#endif

        void genClass (const umlClassNode & node);

        static const char * cppName (std::string name);

        const char * fqname (const umlClassNode & node,
                             bool use_ref_type) const;
        virtual const char * visibility (std::string desc,
                                         const Visibility & vis) = 0;
        std::string & spc () const;

        virtual std::string strPackage (const char * package) const = 0;
        virtual std::string strPointer (const std::string & type) const = 0;

        void writeFile ();
        static const char * comment (const std::string & comment_,
                                     const std::string & startFirstLine,
                                     const std::string & startOtherLines,
                                     const char * endLastLine);
        virtual void writeLicense () = 0;
        virtual void writeStartHeader (std::string & name) = 0;
        virtual void writeEndHeader () = 0;
        // return true if at least one include has been written.
        virtual bool writeInclude (const std::list <std::pair <
                                     std::list <umlPackage *>,
                                                umlClassNode * > > & name) = 0;
        virtual void writeInclude (const char * name) = 0;
        virtual void writeFunctionComment (const umlOperation & ope) = 0;
        virtual void writeFunction (const umlClassNode & node,
                                    const umlOperation & ope,
                                    Visibility & curr_visibility) = 0;
        virtual void writeFunctionGetSet (const umlClassNode & node,
                                          const umlOperation & ope,
                                          Visibility & curr_visibility) = 0;
        virtual void writeComment (const std::string & text) = 0;
        virtual void writeComment (const char * text) = 0;
        virtual void writeClassComment (const std::string & com) = 0;
        virtual void writeClassStart (const umlClassNode & node) = 0;
        virtual void writeClassEnd () = 0;
        virtual void writeAttributeComment (const umlAttribute & attr) = 0;
        virtual void writeAttribute (const umlClassNode & node,
                                     const umlAttribute & attr,
                                     Visibility & curr_visibility) = 0;
        virtual void writeNameSpaceStart (const umlClassNode * name) = 0;
        virtual void writeNameSpaceEnd (const umlClassNode * node) = 0;
        virtual void writeEnum (const umlClassNode & node) = 0;
        virtual void writeStruct (const umlClassNode & node) = 0;
        virtual void writeTypedef (const umlClassNode & node) = 0;
        virtual void writeAssociation (const umlClassNode & node,
                                       const umlassoc & asso,
                                       Visibility & curr_visibility) = 0;
        virtual void writeTemplates (
           const std::list <std::pair <std::string, std::string> > & tmps) = 0;

        static const char * visibility1 (std::string desc,
                                         const Visibility & vis);
        void writeLicense1 (const char * start, const char * end);
        void writeFunctionGetSet1 (const umlClassNode & node,
                                   const umlOperation & ope,
                                   Visibility & curr_visibility);
        bool writeInclude1 (const std::list <std::pair <
                                                     std::list <umlPackage *>,
                                                     umlClassNode *> > & name,
                            const char * startIncludeSystem,
                            const char * endIncludeSystem,
                            const char * startIncludeFile,
                            const char * endIncludeFile,
                            bool forceExtExtern);

        virtual ~GenerateCode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
