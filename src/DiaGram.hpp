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
        std::list <umlClassNode *> uml;
        // Selection of classes to generate code for.
        std::list <std::string> genClasses;
        // Flag that inverts the above selection.
        bool        invertsel : 1;
#ifdef ENABLE_CORBA
        static bool usecorba : 1;
#endif
        
        // To detect circular loop.
        std::list <umlClassNode *> tmp_classes;
        std::list <std::pair <std::list <umlPackage *>, const umlClassNode *> >
                                                                      includes;
        std::list <declaration> decl;
        
        // if expandPackages then all classes in the package and in the
        // sub-packages are add to resCla
        void listClasses (umlClassNode & current,
                          std::list <umlClassNode *> & resCla,
                          // bit 0 : expandPackages
                          // bit 1 : do not include connection with NoLoop
                          //         stereotype.
                          uint8_t flag);

        bool haveInclude (std::list <umlPackage *> & packages,
                          const umlClassNode * cla) const;
        void addInclude (std::list <umlPackage *> & packages,
                         const umlClassNode * cla);
        void pushInclude (const umlClassNode * node);
        void pushInclude (umlPackage * node);
        void pushTmp (umlClassNode * node);
        void popTmp ();
    public:
        DiaGram ();
//        DiaGram (DiaGram & diagram) = delete;

        std::list <umlClassNode *> & getUml ();

        void addGenClasses (std::list <std::string> classes);
        std::list <std::string> getGenClasses () const;

        bool getInvertSel () const;
        void setInvertSel (bool invert);

#ifdef ENABLE_CORBA
        static bool getUseCorba ();
#endif

        void push (umlClassNode * node);
        const std::list <std::pair <std::list <umlPackage *>,
                               const umlClassNode * > > & getIncludes () const;
        void cleanIncludes ();
        void determineIncludes (declaration &d,
                                bool expandPackages,
                                bool noLoop);
        
        std::list <declaration>::iterator getDeclBegin ();
        std::list <declaration>::iterator getDeclEnd ();
        ~DiaGram ();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
