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

#if defined(_WIN32) || defined(_WIN64)
#else
#include <sys/stat.h>
#endif

#include "GenerateCode.hpp"
#include "scan_tree.hpp"
#include "string2.hpp"

#define eq  !strcmp

GenerateCode::GenerateCode (DiaGram    & diagram,
                            const char * ext) :
    dia (diagram),
    license (),
    outdir ("."),
    file_ext (ext),
    file (),
    indent (4),
    indentlevel (0),
    overwrite (true),
    buildtree (false),
    bOpenBraceOnNewline (false),
    oneClassOneHeader (false)
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
GenerateCode::setLicense (char * lic) {
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
GenerateCode::setOutdir (char * dir) {
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
    file.push_back (new std::ofstream ());
    file.back ()->open (outfilename.c_str ());
    if (file.back ()->is_open () && !overwrite) {
        fprintf (stderr, "Failed to overwrite %s.\n", outfilename.c_str ());
        file.back ()->close ();
        exit (1);
    }

    writeLicense ();

    tmpname = strtoupper (filename);
    if (indentlevel != 0) {
        fprintf (stderr, "indent level (%d) should be 0.\n", indentlevel);
    }
    writeStartHeader (tmpname);

    getDia ().cleanIncludes ();
    getDia ().determineIncludes (d, oneClassOneHeader, buildtree);
#ifdef ENABLE_CORBA
    if (getDia ().getUseCorba ()) {
        writeInclude ("p_orb.h");
    }
#endif
    std::list <std::string> incfile = getDia ().getIncludes ();
    if ((!incfile.empty ())
#ifdef ENABLE_CORBA
         || (getDia ().getUseCorba ())
#endif
        ) {
        getFile () << "\n";
    }
    for (std::string namei : incfile) {
        if (namei.compare (filename)) {
            writeInclude (namei + "." + getFileExt ());
        }
    }

    return;
}

void
GenerateCode::closeOutfile () {
    writeEndHeader ();
    if (indentlevel != 0) {
        fprintf (stderr, "indent level (%d) should be 0.\n", indentlevel);
    }
    file.back ()->close ();
    delete file.back ();
    file.pop_back ();
}

void
GenerateCode::generate_code () {
    std::list <declaration>::iterator it2;
    std::list <umlClassNode> tmplist = getDia ().getUml ();
    
    for (umlClassNode & it : tmplist) {
        if (!(is_present (getDia ().getGenClasses (),
                          it.getName ().c_str ()) ^
                                                  getDia ().getInvertSel ())) {
            getDia ().push (it);
        }
    }

    // Generate a file for each outer declaration.
    it2 = getDia ().getDeclBegin ();
    while (it2 != getDia ().getDeclEnd ()) {
        genDecl (*it2, true);

        ++it2;
    }
}

const char *
GenerateCode::getFileExt () const {
    return file_ext.c_str ();
}


void
GenerateCode::setFileExt (char * ext) {
    file_ext.assign (ext);
}


const char *
GenerateCode::getBodyFileExt () const {
    return body_file_ext.c_str ();
}


void
GenerateCode::setBodyFileExt (char * ext) {
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

int
isEnumStereo (const char *stereo) {
    return (!strcasecmp (stereo, "enum") ||
            !strcasecmp (stereo, "enumeration")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAEnum")
#endif
            );
}

int
isStructStereo (const char *stereo) {
    return (!strcasecmp (stereo, "struct") ||
            !strcasecmp (stereo, "structure")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAStruct")
#endif
            );
}

int
isTypedefStereo (const char *stereo) {
    return (!strcasecmp (stereo, "typedef")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBATypedef")
#endif
            );
}

int
isConstStereo (const char *stereo) {
    return (!strcasecmp (stereo, "const") ||
            !strcasecmp (stereo, "constant")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAConstant")
#endif
            );
}

int
GenerateCode::passByReference (umlClass &cl) {
    const char *st;
    st = cl.getStereotype ().c_str ();
    if (strlen (st) == 0) {
        return 1;
    }
    if (isTypedefStereo (st)) {
        umlClassNode *ref = find_by_name (dia.getUml (),
                                          cl.getName ().c_str ());
        if (ref == NULL) {
            return 0;
        }
        return passByReference (*ref);
    }
    return (!isConstStereo (st) &&
            !isEnumStereo (st));
}

#ifdef ENABLE_CORBA
static int
isOoClass (umlClass &cl) {
    const char *st;
    st = cl.getStereotype ().c_str ();
    if (strlen (st) == 0) {
        return 1;
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
GenerateCode::cppName (std::string name) const {
    static std::string buf;
#ifdef ENABLE_CORBA
    if (dia.getUseCorba ()) {
        if (name.compare ("boolean") == 0 ||
            name.compare ("char") == 0 ||
            name.compare ("octet") == 0 ||
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

void
GenerateCode::genClass (const umlClassNode & node) {
#ifdef ENABLE_CORBA
    const char *name = node.getName ().c_str ();
#endif
    const char *stype = node.getStereotype ().c_str ();

    if (strlen (stype) > 0) {
        writeComment (std::string ("Stereotype : ") + stype);
#ifdef ENABLE_CORBA
        isCorba = eq (stype, "CORBAValue");
#endif
    }

    if (!node.getTemplates ().empty ()) {
#ifdef ENABLE_CORBA
        if (isCorba) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else
#endif
        {
            writeTemplates (node.getTemplates ());
        }
    }

    writeClassComment (node);
    writeClass (node);
    incIndentLevel ();

    if (!node.getAssociations ().empty ()) {
        writeComment ("Associations");
        /*
         * The associations are mapped as private members.
         * Is that really what we want?
         * (For example, other UML tools additionally generate
         * setters/getters.)  Ideas and comments welcome.
        */
        for (const umlassoc & assoc : node.getAssociations ()) {
            writeAssociation (assoc);
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
                    fprintf (stderr,
                             "CORBAValue %s/%s: static not supported\n",
                             name,
                             member);
                }
                ref = find_by_name (dia.getUml (), umla.getType ().c_str ());
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
            int tmpv = -1;
            writeComment ("Attributes");
            incIndentLevel ();
            for (const umlAttribute & umla : node.getAttributes ()) {
                writeAttribute (umla, &tmpv);
            }
            decIndentLevel ();
        }
    }

    if (!node.getOperations ().empty ()) {
        int tmpv = -1;
        writeComment ("Operations");
#ifdef ENABLE_CORBA
        if (isCorba) {
            getFile () << spc () << "public :\n";
        }
#endif
        incIndentLevel ();
        for (const umlOperation & umlo : node.getOperations ()) {
            writeFunction (umlo, &tmpv);
        }
        decIndentLevel ();
    }

#ifdef ENABLE_CORBA
    if ((!node.getAttributes ().empty ()) && (isCorba)) {
        getFile () << "\n";
        decIndentLevel ();
        writeComment ("State member implementation");
        getFile () << spc () << "private :\n";
        incIndentLevel ();
        for (const umlAttribute & umla : node.getAttributes ()) {
            umlClassNode *ref = find_by_name (dia.getUml (),
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
    getFile () << spc () << "};\n";
}

const char *
dirName (const umlPackage &pkg) {
    static std::string buf;
    std::list <umlPackage>::const_iterator it;

    buf.clear ();
    std::list <umlPackage> pkglist;
    umlPackage::make_package_list (&pkg, pkglist);
    it = pkglist.begin ();
    while (it != pkglist.end ()) {
        buf.append ((*it).getName ());
        ++it;
        if (it != pkglist.end ()) {
            buf.append (1, SEPARATOR);
        }
    }
    return buf.c_str ();
}

void
GenerateCode::genDecl (declaration &d,
                       bool forceOpen) {
#ifdef ENABLE_CORBA
    const char *name;
#endif
    const char *stype;
    const umlClassNode *node;
    std::list <umlAttribute>::const_iterator umla;

    if ((buildtree) && (d.decl_kind == dk_module)) {
        std::string folder;
        
        folder.assign (outdir);
        folder.append (1, SEPARATOR);
        folder.append (dirName (d.u.this_module->pkg));

        mkdir (folder.c_str (), 0777);
    }

    if (forceOpen && (!oneClassOneHeader || !d.decl_kind == dk_module)) {
        std::string name_;

        if (d.decl_kind == dk_module) {
            if (buildtree) {
                name_ = dirName (d.u.this_module->pkg);
            }
            else {
                name_ = d.u.this_module->pkg.getName ();
            }
        } else {
            if ((buildtree) && (d.u.this_class->getPackage () != NULL)) {
                name_.assign (dirName (*d.u.this_class->getPackage ()));
                name_.append (1, SEPARATOR);
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

    getFile () << "\n";
    writeNameSpaceStart (d.u.this_class);

    node = d.u.this_class;
    stype = node->getStereotype ().c_str ();
#ifdef ENABLE_CORBA
    name = node->getName ().c_str ();
#endif
    umla = node->getAttributes ().begin ();

#ifdef ENABLE_CORBA
    if (eq (stype, "CORBANative")) {
        writeComment (std::string ("CORBANative: ") +
                      node->getName () +
                      std::string ("\n"));
    }
    else
#endif
    if (isConstStereo (stype)) {
        writeConst (*node);
    }
    else if (isEnumStereo (stype)) {
        writeEnum (*node);
    }
    else if (isStructStereo (stype)) {
        writeStruct (*node);
    }
#ifdef ENABLE_CORBA
    else if (eq (stype, "CORBAException")) {
        fprintf (stderr, "%s: CORBAException not yet implemented\n", name);

    }
    else if (eq (stype, "CORBAUnion")) {
        if (umla == node->getAttributes ().end ()) {
            fprintf (stderr, "Error: attributes not set at union %s\n", name);
            exit (1);
        }
        fprintf (stderr, "%s: CORBAUnion not yet fully implemented\n", name);
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
    else if (isTypedefStereo (stype)) {
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


std::string
GenerateCode::spc () const {
    std::string spcbuf ("");
    int n_spaces = indent * indentlevel, i;

    for (i = 0; i < n_spaces; i++) {
        spcbuf.append (" ");
    }

    return spcbuf;
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
    if (indentlevel != 0) {
        indentlevel--;
    }
    else {
        fprintf (stderr, "Failed to decIndentLevel.\n");
    }
}

void
GenerateCode::writeLicenseAll () {
    FILE * licensefile = fopen (license.c_str (), "r");
    if (!licensefile) {
        fprintf (stderr, "Can't open the license file.\n");
        exit (1);
    }

    int lc;
    rewind (licensefile);
    while ((lc = fgetc (licensefile)) != EOF) {
        getFile () << static_cast <char> (lc);
    }

    fclose (licensefile);
}


GenerateCode::~GenerateCode () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
