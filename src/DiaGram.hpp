/*
Copyright (C) 2014-2014 Vincent Le Garrec <legarrec.vincent@gmail.com>

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

#ifndef DIA_GRAM_HPP
#define DIA_GRAM_HPP

#include <string>
#include <list>

#include "dia2code.hpp"
#include "decls.hpp"
#include "source_parser.hpp"

class DiaGram {
    private:
        // Diagram under uml format.
        umlclasslist uml;
        // Selection of classes to generate code for.
        std::list <std::string> genClasses;
        // Flag that inverts the above selection.
        bool        invertsel : 1;
        // Set by push() if CORBA stereotypes in use.
        bool        usecorba : 1;
        
        std::list <std::string> tmp_classes;
        std::list <std::string> includes;
        
        std::list <std::string> scan_tree_classes ();
        std::list <std::string> find_classes (umlclasslist current_class);
        umlclasslist            list_classes (umlclasslist current_class);

        /**
         * open_outfile() returns NULL if the file exists and is not rewritten
         * due to a clobber prohibition. Does an exit(1) if serious problems happen.
        */
        int have_include (const char *name);
        void add_include (const char *name);
        void push_include (umlclassnode *node);

        void source_preserve(umlclass *class_, const char *filename, sourcecode *source );
        void generate_operation_comment( FILE *outfile, umloperation *ope );
        void generate_attribute_comment( FILE *outfile, umlattribute *attr );
    public:
        DiaGram ();
//        DiaGram (DiaGram & diagram) = delete;

        umlclasslist getUml ();
        void setUml (umlclasslist diagram);

        void addGenClasses (std::list <std::string> classes);
        bool genGenClasses (char * class_);
        std::list <std::string> getGenClasses ();

        bool getInvertSel ();
        void setInvertSel (bool invert);

        bool getUseCorba ();
        void setUseCorba (bool corba);

        void push (umlclassnode *node);
        std::list <std::string> getIncludes ();
        void cleanIncludes ();
        void determine_includes (declaration *d);
        
        ~DiaGram ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
