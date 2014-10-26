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

#ifndef DIA_GRAM_HPP
#define DIA_GRAM_HPP

#include <string>
#include <list>

#include "dia2code.hpp"

class DiaGram {
    private:
        // Diagram under uml format.
        umlclasslist uml;
        // Selection of classes to generate code for.
        std::list <std::string> genClasses;
        // License file.
        std::string license;
        // Output directory.
        std::string outdir;
        // Overwrite files while generating code.
        bool        overwrite : 1;
        // Convert package names to a directory tree.
        bool        buildtree : 1;
        // Flag that inverts the above selection.
        bool        invertsel : 1;
    public:
        DiaGram ();
//        DiaGram (DiaGram & diagram) = delete;

        void setUml (umlclasslist diagram);

        void addGenClasses (std::list <std::string> classes);
        bool genGenClasses (char * class_);

        char * getLicense ();
        void   setLicense (char * lic);

        char * getOutdir ();
        const  std::string * getOutdirS ();
        void   setOutdir (char * dir);

        bool getOverwrite ();
        void setOverwrite (bool over);

        bool getBuildTree ();
        void setBuildTree (bool build);

        bool getInvertSel ();
        void setInvertSel (bool invert);

        ~DiaGram ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
