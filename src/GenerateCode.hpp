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

#include <fstream>

#include "DiaGram.hpp"

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
        std::ofstream file;
        uint8_t     indent : 4;
        uint8_t     indentlevel : 3;
        // Overwrite files while generating code.
        bool        overwrite : 1;
        // Convert package names to a directory tree.
        bool        buildtree : 1;
        bool        bOpenBraceOnNewline : 1;
        
        int pass_by_reference (umlclass &cl);
        void gen_class (umlclassnode *node);
        const char * cppname (std::string name);
        void check_visibility (int *curr_vis, int new_vis);
        std::string spc();
        void open_outfile (const char *filename);
        void gen_decl (declaration &d);
    public:
        GenerateCode (DiaGram & diagram, const char * ext);

        DiaGram & getDia ();
        void generate_code ();
        
        const char * getFileExt ();
        void   setFileExt (char * ext);

        const char * getBodyFileExt ();
        void   setBodyFileExt (char * ext);

        uint32_t getIndent ();
        void     setIndent (uint8_t spaces);

        const char * getLicense ();
        void   setLicense (char * lic);

        const char * getOutdir ();
        const  std::string * getOutdirS ();
        void   setOutdir (char * dir);

        bool getOverwrite ();
        void setOverwrite (bool over);

        bool getBuildTree ();
        void setBuildTree (bool build);

        bool getOpenBraceOnNewline ();
        void setOpenBraceOnNewline (bool newline);

        virtual ~GenerateCode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
