/*
Copyright (C) 2014-2014

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef GENERATE_CODE_HPP
#define GENERATE_CODE_HPP

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
        FILE *      file;
        uint8_t     indent : 3;
        uint8_t     indentlevel : 3;
        // Overwrite files while generating code.
        bool        overwrite : 1;
        // Convert package names to a directory tree.
        bool        buildtree : 1;
        
        int pass_by_reference (umlclass *cl);
        void gen_class (umlclassnode *node);
        char * cppname (char *name);
        void check_visibility (int *curr_vis, int new_vis);
        void pbody (char *msg, ...);
        void pboth (char *msg, ...);
        char * spc();
        void emit (char *msg, ...);
        void eboth (char *msg, ...);
        void open_outfile (char *filename);
        char * create_package_dir (umlpackage *pkg);
    public:
        GenerateCode (DiaGram & diagram, const char * ext);

        DiaGram & getDia ();
        void generate_code ();
        
        char * getFileExt ();
        void   setFileExt (char * ext);

        uint32_t getIndent ();
        void     setIndent (uint32_t spaces);

        char * getLicense ();
        void   setLicense (char * lic);

        char * getOutdir ();
        const  std::string * getOutdirS ();
        void   setOutdir (char * dir);

        bool getOverwrite ();
        void setOverwrite (bool over);

        bool getBuildTree ();
        void setBuildTree (bool build);

        
        void print (char *msg, ...);
        void gen_decl (declaration *d);
        
        virtual ~GenerateCode ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
