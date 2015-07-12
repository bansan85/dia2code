/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014 Javier O'Hara - Oliver Kellogg

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


/* NB: If you use CORBA stereotypes, you will need the file p_orb.h
   found in the runtime/cpp directory.  */

#include "config.h"

#include <iostream>
#include <cassert>
#include <cstdint>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "GenerateCode.hpp"
#include "scan_tree.hpp"
#include "string2.hpp"

#define eq  !strcmp

GenerateCode::GenerateCode (DiaGram    & diagram,
                            const char * ext,
                            uint8_t      version_,
                            bool         handleIncludePackage_) :
    dia (diagram),
    license (),
    outdir ("."),
    file_ext (ext),
    file (),
    version (version_),
    indent (4),
    indentlevel (0),
    overwrite (true),
    buildtree (false),
    bOpenBraceOnNewline (false),
    oneClassOneHeader (false),
    handleIncludePackage (handleIncludePackage_)
#ifdef ENABLE_CORBA
    , isCorba (false)
#endif
{
}


DiaGram &
GenerateCode::getDia () {
    return dia;
}


const std::string &
GenerateCode::getLicense () const {
    return license;
}


void
GenerateCode::setLicense (const char * lic) {
    license.assign (lic);
}


const char *
GenerateCode::getOutdir () const {
    return outdir.c_str ();
}


const std::string *
GenerateCode::getOutdirS () const {
    return &outdir;
}


void
GenerateCode::setOutdir (const char * dir) {
    outdir.assign (dir);
}


bool
GenerateCode::getOverwrite () const {
    return overwrite;
}


void
GenerateCode::setOverwrite (bool over) {
    overwrite = over;
}


bool
GenerateCode::getBuildTree () const {
    return buildtree;
}


void
GenerateCode::setBuildTree (bool build) {
    if (build) {
        oneClassOneHeader = true;
    }
    buildtree = build;

    return;
}


bool
GenerateCode::getOpenBraceOnNewline () const {
    return bOpenBraceOnNewline;
}


void
GenerateCode::setOpenBraceOnNewline (bool newline) {
    bOpenBraceOnNewline = newline;
}

bool
GenerateCode::getOneClass () const {
    return oneClassOneHeader;
}


void
GenerateCode::setOneClass (bool value) {
    oneClassOneHeader = value;
}

void
GenerateCode::setHandleIncludePackage (bool val) {
    handleIncludePackage = val;
}


#ifdef ENABLE_CORBA
bool
GenerateCode::getCorba () const {
    return isCorba;
}
#endif

void
GenerateCode::openOutfile (const std::string & filename, declaration & d) {
    static std::string outfilename;
    std::string tmpname;

    outfilename.assign (outdir);
    outfilename.append (1, SEPARATOR);
    outfilename.append (filename);
    outfilename.append (".");
    outfilename.append (getFileExt ());

    std::ifstream f (outfilename.c_str ());

    if ((f.good ()) && (!overwrite)) {
        f.close();
        throw std::string ("Overwrite " + outfilename + " is forbidden.\n");
    }
    else {
        f.close();
    }

    file.push_back (new std::ofstream ());
    file.back ()->open (outfilename.c_str ());
    if (!file.back ()->is_open ()) {
        file.back ()->close ();
        throw std::string ("Failed to create " + outfilename + ".\n");
    }

    tmpname = strtoupper (filename);
    assert (indentlevel == 0);
    writeStartHeader (tmpname);

    writeLicense ();

    getDia ().cleanIncludes ();
    getDia ().determineIncludes (d, !handleIncludePackage);
#ifdef ENABLE_CORBA
    if (getDia ().getUseCorba ()) {
        writeInclude ("p_orb.h");
        getFile () << "\n";
    }
#endif

    writeInclude (getDia ().getIncludes ());
    if (d.decl_kind == dk_class) {
        writeAfterInclude (d.u.this_class);
    }

    return;
}

void
GenerateCode::closeOutfile () {
    writeEndHeader ();
    assert (indentlevel == 0);
    file.back ()->close ();
    delete file.back ();
    file.pop_back ();
}

void
GenerateCode::generate_code () {
    std::list <declaration>::iterator it2;
    std::list <umlClassNode *> tmplist = getDia ().getUml ();
    
    for (umlClassNode * it : tmplist) {
        if (!it->isPushed ()) {
            getDia ().push (it);
        }
    }

    // Generate a file for each outer declaration.
    it2 = getDia ().getDeclBegin ();
    while (it2 != getDia ().getDeclEnd ()) {
        if (it2->decl_kind == dk_class) {
            if ((getDia ().getGenClasses ().empty ()) ||
                (isPresent (getDia ().getGenClasses (),
                            it2->u.this_class->getName ().c_str ()) ^
                                                  getDia ().getInvertSel ())) {
                genDecl (*it2, true);
            }
        }
        else {
            genDecl (*it2, true);
        }

        ++it2;
    }
}

const char *
GenerateCode::getFileExt () const {
    return file_ext.c_str ();
}


void
GenerateCode::setFileExt (const char * ext) {
    file_ext.assign (ext);
}


const char *
GenerateCode::getBodyFileExt () const {
    return body_file_ext.c_str ();
}


void
GenerateCode::setBodyFileExt (const char * ext) {
    body_file_ext.assign (ext);
}


std::ofstream &
GenerateCode::getFile () {
    return *file.back ();
}

char *
subst (char *str, const char search, char replace) {
    char *p;
    while ((p = strchr (str, search)) != NULL) {
        *p = replace;
    }
    return str;
}

char *
nospc (char *str) {
    return subst (str, ' ', '_');
}

bool
GenerateCode::passByReference (umlClass & cl) const {
    if (cl.isStereotypeTypedef ()) {
        umlClassNode *ref = findByName (dia.getUml (),
                                        cl.getName ().c_str ());
        if (ref == NULL) {
            return false;
        }
        return passByReference (*ref);
    }
    return (!cl.isStereotypeConst () &&
            !cl.isStereotypeEnum ());
}

#ifdef ENABLE_CORBA
static bool
isOoClass (umlClass &cl) {
    const char *st;
    st = cl.getStereotype ().c_str ();
    if (strlen (st) == 0) {
        return true;
    }
    return (!isConstStereo (st) &&
            !isTypedefStereo (st) &&
            !isEnumStereo (st) &&
            !isStructStereo (st) &&
            !eq (st, "CORBAUnion") &&
            !eq (st, "CORBAException"));
}
#endif

const char *
GenerateCode::cppName (std::string name) {
    static std::string buf;
#ifdef ENABLE_CORBA
    if (dia.getUseCorba ()) {
        if (name.compare ("boolean") == 0 || name.compare ("char") == 0 || name.compare ("octet") == 0 ||
            name.compare ("short") == 0 ||
            name.compare ("long") == 0 ||
            name.compare ("float") == 0 ||
            name.compare ("double") == 0 ||
            name.compare ("string") == 0 ||
            name.compare ("any") == 0) {
                buf.assign (strPackage ("CORBA"));
                buf.append (nospc (const_cast <char *> (
                                            strtoupperfirst (name).c_str ())));
        }
        else if (name.compare ("long long") == 0) {
            buf.assign (strPackage ("CORBA"));
            buf.append ("LongLong");
        }
        else if (name.compare ("unsigned short") == 0) {
            buf.assign (strPackage ("CORBA"));
            buf.append ("UShort");
        }
        else if (name.compare ("unsigned long") == 0) {
            buf.assign (strPackage ("CORBA"));
            buf.append ("ULong");
        }
        else if (name.compare ("unsigned long long") == 0) {
            buf.assign (strPackage ("CORBA"));
            buf.append ("ULongLong");
        }
        else {
            buf.assign (name);
        }
    } else
#endif
    {
        buf.assign (name);
    }
    return buf.c_str ();
}

const char *
GenerateCode::fqname (const umlClassNode &node, bool use_ref_type) const {
    static std::string buf;

    buf.clear ();
    if (node.getPackage () != NULL) {
        std::list <umlPackage *> pkglist;

        umlPackage::makePackageList (node.getPackage (), pkglist);
        for (const umlPackage * it : pkglist) {
            buf.append (strPackage (it->getName ().c_str ()));
        }
    }
    if (use_ref_type) {
        buf.append (strPointer (node.getName ()));
    }
    else {
        buf.append (node.getName ());
    }
    return buf.c_str ();
}

void
GenerateCode::genClass (const umlClassNode & node) {
#ifdef ENABLE_CORBA
    const char *name = node.getName ().c_str ();
#endif
    Visibility tmpv = Visibility::IMPLEMENTATION;

    if (node.getName ().empty ()) {
        std::cerr << "A class have an empty name.\n";
    }

#ifdef ENABLE_CORBA
    isCorba = node.isStereotypeCorba ();
#endif

    // Check that if class is abstract, at least one class are abstract.
    if (node.isAbstract ()) {
        bool absok = false;

        for (const umlOperation & umlo : node.getOperations ()) {
            if (umlo.getInheritance () == Inheritance::ABSTRACT) {
                absok = true;
            }
        }

        if ((!absok) && (!node.getOperations ().empty ())) {
            std::cerr << "Class " << fqname (node, false)
                      << " is abstract but no operation is defined as abstract.\n";
        }
    }

    writeClassComment (node.getComment ());
    writeClassStart (node);
    incIndentLevel ();

    if (!node.getAssociations ().empty ()) {
        writeComment ("Associations");
        for (const umlassoc & assoc : node.getAssociations ()) {
            writeAssociation (node, assoc, tmpv);
        }
    }

    if (!node.getAttributes ().empty ()) {
#ifdef ENABLE_CORBA
        if (isCorba) {
            writeComment ("Public state members");
            getFile () << spc () << "public:\n";
            incIndentLevel ();
            for (const umlAttribute & umla : node.getAttributes ()) {
                const char *member = umla.getName ().c_str ();
                umlClassNode *ref;
                if (umla.getVisibility () != '0') {
                    continue;
                }
                if (umla.isStatic ()) {
                    std::cerr << "CORBAValue " << name << "/" << member
                              << ": static not supported.\n",
                }
                ref = findByName (dia.getUml (), umla.getType ().c_str ());
                if (ref != NULL) {
                    getFile () << spc () << fqname (*ref, true);
                }
                else {
                    getFile () << spc () << cppName (umla.getType ());
                }
                if (!bOpenBraceOnNewline) {
                    getFile () << " " << member << " () { return _" << member
                         << "; }\n";
                }
                else {
                    getFile () << " " << member << " ()\n" << spc () << "{\n";
                    incIndentLevel ();
                    getFile () << spc () << "return _" << member << ";\n";
                    decIndentLevel ();
                    getFile () << spc () << "}\n";
                }
                getFile () << spc () << "void " << member << " (";
                if (ref != NULL) {
                    int by_ref = passByReference (*ref);
                    if (by_ref) {
                        getFile () << "const ";
                    }
                    getFile () << fqname (*ref, true);
                    if (by_ref) {
                        getFile () << "&";
                    }
                }
                else {
                    getFile () << cppName (umla.getType ());
                }
                if (!bOpenBraceOnNewline) {
                    getFile () << " value_) { _" << member << " = value_; }\n";
                }
                else {
                    getFile () << " value_)\n" << spc () << "{\n";
                    incIndentLevel ();
                    getFile () << spc () << "_" << member << " = value_;";
                    decIndentLevel ();
                    getFile () << spc () << "}\n";
                }
            }
            decIndentLevel ();
        }
        else
#endif
        {
            writeComment ("Attributes");
            for (const umlAttribute & umla : node.getAttributes ()) {
                if (umla.getName ().empty ()) {
                    std::cerr << "An attribute of the "
                              << fqname (node, false)
                              << " class have an empty name.\n";
                }
                if (umla.getType ().empty ()) {
                    std::cerr << "An attribute of the " << fqname (node, false)
                              << " class have an empty type.\n";
                }
                writeAttribute (node, umla, tmpv);
            }
        }
    }

    if (!node.getOperations ().empty ()) {
        writeComment ("Operations");
#ifdef ENABLE_CORBA
        if (isCorba) {
            getFile () << spc () << "public :\n";
        }
#endif
        for (const umlOperation & umlo : node.getOperations ()) {
            if (umlo.isStereotypeGetSet ()) {
                writeFunctionGetSet (node, umlo, tmpv);
            }
            else {
                writeFunction (node, umlo, tmpv);
            }
        }
    }

#ifdef ENABLE_CORBA
    if ((!node.getAttributes ().empty ()) && (isCorba)) {
        getFile () << "\n";
        decIndentLevel ();
        writeComment ("State member implementation");
        getFile () << spc () << "private :\n";
        incIndentLevel ();
        for (const umlAttribute & umla : node.getAttributes ()) {
            umlClassNode *ref = findByName (dia.getUml (),
                                            umla.getType ().c_str ());
            getFile () << spc ();
            if (ref != NULL) {
                getFile () << fqname (*ref, isOoClass (*ref));
                /*
                 * FIXME: Find a better way to decide whether to use
                 * a pointer.
                */
            }
            else {
                getFile () << cppName (umla.getType ());
            }
            getFile () << " _" << umla.getName () << ";\n";
        }
    }
#endif

    decIndentLevel ();
    writeClassEnd ();
}

const char *
dirName (umlPackage * pkg, char separator) {
    static std::string buf;
    std::list <umlPackage *>::const_iterator it;
    std::list <umlPackage *> pkglist;

    buf.clear ();
    umlPackage::makePackageList (pkg, pkglist);
    it = pkglist.begin ();
    while (it != pkglist.end ()) {
        buf.append ((*it)->getName ());
        ++it;
        if (it != pkglist.end ()) {
            buf.append (1, separator);
        }
    }
    return buf.c_str ();
}

void
GenerateCode::genDecl (declaration &d,
                       bool forceOpen) {
#ifdef ENABLE_CORBA
    const char *name;
    std::list <umlAttribute>::const_iterator umla;
#endif
    const umlClassNode *node;

    if ((d.decl_kind == dk_class) && (d.u.this_class->isStereotypeExtern ())) {
        return;
    }
    if ((d.decl_kind == dk_module) &&
        (d.u.this_module->pkg->isStereotypeExtern ())) {
        return;
    }

    if ((buildtree) && (d.decl_kind == dk_module)) {
        std::string folder;
        
        folder.assign (outdir);
        folder.append (1, SEPARATOR);
        folder.append (dirName (d.u.this_module->pkg, SEPARATOR));

        if (
#if defined(_WIN32) || defined(_WIN64)
        _mkdir (folder.c_str ()) != 0
#else
        mkdir (folder.c_str (), 0777) != 0
#endif
        ) {
            if (errno != EEXIST) {
                throw std::string (std::string ("Fail to create folder ") +
                                   folder + std::string (".\n"));
            }
        }
    }

    if (forceOpen && (!oneClassOneHeader || !d.decl_kind == dk_module)) {
        std::string name_;

        if (d.decl_kind == dk_module) {
            if (buildtree) {
                name_ = dirName (d.u.this_module->pkg, SEPARATOR);
            }
            else {
                name_ = dirName (d.u.this_module->pkg, '-');
            }
        } else {
            if (d.u.this_class->getPackage () != NULL) {
                if (buildtree) {
                    name_.assign (dirName (d.u.this_class->getPackage (),
                                           SEPARATOR));
                    name_.append (1, SEPARATOR);
                }
                else {
                    name_.assign (dirName (d.u.this_class->getPackage (),
                                           '-'));
                    name_.append (1, '-');
                }
            }
            name_.append (d.u.this_class->getName ());
        }

        openOutfile (name_, d);
    }

    if (d.decl_kind == dk_module) {
        for (declaration & it : d.u.this_module->contents) {
            genDecl (it, oneClassOneHeader);
        }
        if (forceOpen && !oneClassOneHeader) {
            closeOutfile ();
        }
        return;
    }

    node = d.u.this_class;

    writeNameSpaceStart (node);
#ifdef ENABLE_CORBA
    name = node->getName ().c_str ();
    umla = node->getAttributes ().begin ();
#endif

#ifdef ENABLE_CORBA
    if (eq (stype, "CORBANative")) {
        writeComment (std::string ("CORBANative: ") +
                      node->getName () +
                      std::string ("\n"));
    }
    else
#endif
    if (node->isStereotypeEnum ()) {
        if (!node->getOperations ().empty ()) {
            std::cerr << "Class \"" << node->getName ()
                      << "\" is enum. All operations are ignored.\n";
        }
        if (!node->getTemplates ().empty ()) {
            std::cerr << "Class \"" << node->getName ()
                      << "\" is enum. All templates are ignored.\n";
        }
        if (node->isAbstract ()) {
            std::cerr << "Class \"" << node->getName ()
                      << "\" is abstact. Ignored.\n";
        }
        writeEnum (*node);
    }
    else if (node->isStereotypeStruct ()) {
        writeStruct (*node);
    }
#ifdef ENABLE_CORBA
    else if (eq (stype, "CORBAException")) {
        std::cerr << name << ": CORBAException not yet implemented.\n";

    }
    else if (eq (stype, "CORBAUnion")) {
        if (umla == node->getAttributes ().end ()) {
            throw std::string ("Attributes not set at union " + name + "\n");
        }
        std::cerr << name << ": CORBAUnion not yet fully implemented.\n";
        if (bOpenBraceOnNewline) {
            writeComment ("CORBAUnion");
            getFile () << spc () << "class " << name << "\n";
            getFile () << spc () << "{\n";
        }
        else {
            writeComment ("CORBAUnion");
            getFile () << spc () << "class " << name << " {\n";
        }
        getFile () << spc () << "public :\n";
        incIndentLevel ();
        getFile () << spc () << (*umla).getType () << " _d();\n\n";
        ++umla;
        while (umla != node->getAttributes ().end ()) {
            (*umla).check (name);
            getFile () << spc () << cppName ((*umla).getType ()) << " "
                 << (*umla).getName () << " ();\n";
            getFile () << spc () << "void " << (*umla).getName () << " ("
                 << cppName ((*umla).getType ())
                 << " _value);\n\n";
            ++umla;
        }
        decIndentLevel ();
        getFile () << spc () << "};\n\n";

    }
#endif
    else if (node->isStereotypeTypedef ()) {
        writeTypedef (*node);
    }
    else {
        genClass (*node);
    }

    writeNameSpaceEnd (d.u.this_class);

    if (forceOpen) {
        closeOutfile ();
    }
}


std::string &
GenerateCode::spc () const {
    static std::string spcbuf;

    spcbuf.assign (indent * indentlevel, ' ');

    return spcbuf;
}

uint8_t
GenerateCode::getVersion () const {
    return version;
}

void
GenerateCode::setVersion (uint8_t version_) {
    version = version_;
}

uint32_t
GenerateCode::getIndent () const {
    return indent;
}


void
GenerateCode::setIndent (uint8_t spaces) {
    if ((spaces < 1) || (spaces > 8)) {
        return;
    }

    indent = spaces & 15;

    return;
}


void
GenerateCode::incIndentLevel () {
    indentlevel++;
}


void
GenerateCode::decIndentLevel () {
    assert (indentlevel != 0);
    indentlevel--;
}

void
GenerateCode::writeFile () {
#if defined(_WIN32) || defined(_WIN64)
    FILE * licensefile;
    if ((fopen_s (&licensefile, license.c_str (), "r") != 0) ||
        (licensefile == NULL)) {
        throw std::string ("Can't open the license file " + license + ".\n");
    }
#else
    FILE * licensefile = fopen (license.c_str (), "r");

    if (!licensefile) {
        throw std::string ("Can't open the license file " + license + ".\n");
    }
#endif

    int lc;
    rewind (licensefile);
    while ((lc = fgetc (licensefile)) != EOF) {
        getFile () << static_cast <char> (lc);
    }

    fclose (licensefile);
}


const char *
GenerateCode::comment (const std::string & comment_,
                       const std::string & startFirstLine,
                       const std::string & startOtherLines,
                       const char * endLastLine) {
    static std::string buf;
    size_t start = 0;
    size_t end;

    buf.clear ();

    end = comment_.find ("\n", start);
    while (end != std::string::npos)
    {
        if (start == 0) {
            buf.append (startFirstLine);
        }
        else {
            buf.append (startOtherLines);
        }
        buf.append (comment_.substr (start, end-start));
        buf.append ("\n");
        start = end + 1;
        end = comment_.find ("\n", start);
    }

    if (start == 0) {
        buf.append (startFirstLine);
    }
    else {
        buf.append (startOtherLines);
    }
    buf.append (comment_.substr (start, end-start));
    buf.append (endLastLine);

    return buf.c_str ();
}

void
GenerateCode::writeLicense1 (const char * start, const char * end) {
    if (getLicense ().empty ()) {
        return;
    }

    getFile () << start << "\n";
    writeFile ();
    getFile () << end << "\n\n";
}

const char *
GenerateCode::visibility1 (std::string desc,
                           const Visibility & vis) {
    switch (vis) {
        case Visibility::PUBLIC :
            return "public";
        case Visibility::PRIVATE :
            return "private";
        case Visibility::PROTECTED :
            return "protected";
        case Visibility::IMPLEMENTATION :
            std::cerr << desc + ": implementation not applicable. Default: public.\n";
            return "public";
        default :
            throw std::string ("Unknown visibility.\n");
    }
}

void
GenerateCode::writeFunctionGetSet1 (const umlClassNode & node,
                                    const umlOperation & ope,
                                    Visibility & curr_visibility) {
    std::string tmpname;

    if ((!ope.getType ().compare ("bool")) ||
        (!ope.getType ().compare ("boolean")) ||
        (!ope.getType ().compare ("Boolean"))) {
        tmpname.assign ("is");
    }
    else {
        tmpname.assign ("get");
    }
    tmpname.append (strtoupperfirst (ope.getName ()));
    umlOperation ope2 (tmpname,
                       ope.getType (),
                       "",
                       ope.getVisibility (),
                       ope.getInheritance (),
                       ope.isStatic (),
                       true,
                       false,
                       false,
                       false);
    writeFunction (node,ope2, curr_visibility);

    umlAttribute parameter ("value",
                            "",
                            ope.getType (),
                            "",
                            ope.getVisibility (),
                            ope.getInheritance (),
                            ope.isStatic (),
                            ope.isConstant (),
                            Kind::IN);
    tmpname.assign ("set");
    tmpname.append (strtoupperfirst (ope.getName ()));
    ope2 = umlOperation (tmpname,
                         "void",
                         "",
                         ope.getVisibility (),
                         ope.getInheritance (),
                         ope.isStatic (),
                         false,
                         false,
                         false,
                         false);
    ope2.addParameter (parameter);
    writeFunction (node, ope2, curr_visibility);
}

bool
GenerateCode::writeInclude1 (const std::list <std::pair <
                                                      std::list <umlPackage *>,
                                                const umlClassNode *> > & name,
                             const char * startIncludeSystem,
                             const char * endIncludeSystem,
                             const char * startIncludeFile,
                             const char * endIncludeFile,
                             bool forceExtExtern) {
    bool ret = false;
    // List of include then if (true) the class is system and should not be
    // generated.
    std::list <std::pair <std::string, bool> > incs;

    for (const std::pair <std::list <umlPackage *>,
                                           const umlClassNode *> & it : name) {
        std::string include;
        std::pair <std::string, bool> add;

        if (it.second == NULL) {
            continue;
        }

        ret = true;
    
        if (getBuildTree () || it.second->isStereotypeExtern ()) {
            if (!it.first.empty ()) {
                for (const umlPackage * pack : it.first) {
                    include.append (pack->getName ());
                    include.append (1, SEPARATOR);
                }
            }
            include.append (it.second->getName ());
        }
        else if (getOneClass ()) {
            if (!it.first.empty ()) {
                for (const umlPackage * pack : it.first) {
                    include.append (pack->getName ());
                    include.append ("-");
                }
            }
            include.append (it.second->getName ());
        }
        else {
            if (!it.first.empty ()) {
                include.append ((*it.first.begin ())->getName ());
            }
            else {
                include.append (it.second->getName ());
            }
        }
        if ((forceExtExtern) || (!it.second->isStereotypeExtern ())) {
            include.append (".");
            include.append (getFileExt ());
        }
        add.first = include;
        add.second = it.second->isStereotypeExtern ();
        if (std::find (incs.begin (),
                       incs.end (),
                       add) == incs.end ()) {
            if (add.second) {
                incs.push_front (add);
            }
            else {
                incs.push_back (add);
            }
        }
    }

    for (const std::pair <std::string, bool> & add : incs) {
        if (add.second) {
            getFile () << spc () << startIncludeSystem << add.first
                       << endIncludeSystem;
        }
        else {
            getFile () << spc () << startIncludeFile << add.first
                       << endIncludeFile;
        }
    }
    if (!incs.empty ()) {
        getFile () << "\n";
    }

    return ret;
}

void
GenerateCode::writeAfterInclude (umlClassNode *) {

}

GenerateCode::~GenerateCode () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
