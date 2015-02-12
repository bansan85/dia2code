/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2014-2014 Vincent Le Garrec

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


#ifndef DIA_GRAM_HPP
#define DIA_GRAM_HPP

#include "config.h"

#include <string>
#include <list>

#include "decls.hpp"

class DiaGram {
    private:
        // Diagram under uml format.
        std::list <umlClassNode> uml;
        // Selection of classes to generate code for.
        std::list <std::string> genClasses;
        // Flag that inverts the above selection.
        bool        invertsel : 1;
#ifdef ENABLE_CORBA
        bool        usecorba : 1;
#endif
        
        std::list <std::string> tmp_classes;
        std::list <std::list <std::string> > includes;
        std::list <declaration> decl;
        
        void listClasses (umlClassNode & current_class,
                          std::list <umlClassNode> & res);

        /**
         * openOutfile() returns NULL if the file exists and is not rewritten
         * due to a clobber prohibition. Does an exit(1) if serious problems happen.
        */
        bool haveInclude (const std::list <std::string> & name) const;
        void addInclude (const std::list <std::string> & name);
        void pushInclude (umlClassNode & node);
    public:
        DiaGram ();
//        DiaGram (DiaGram & diagram) = delete;

        std::list <umlClassNode> & getUml ();

        void addGenClasses (std::list <std::string> classes);
        std::list <std::string> getGenClasses () const;

        bool getInvertSel () const;
        void setInvertSel (bool invert);

#ifdef ENABLE_CORBA
        bool getUseCorba () const;
        void setUseCorba (bool corba);
#endif

        void push (umlClassNode &node);
        std::list <std::list <std::string> > getIncludes () const;
        void cleanIncludes ();
        void determineIncludes (declaration &d, bool oneClass, bool buildtree);
        
        std::list <declaration>::iterator getDeclBegin ();
        std::list <declaration>::iterator getDeclEnd ();
        ~DiaGram ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
