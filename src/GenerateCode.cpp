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

#include "GenerateCode.hpp"
#include "scan_tree.hpp"
#include "string2.hpp"

#define SPEC_EXT "h"
#define BODY_EXT "cpp"

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
GenerateCode::open_outfile (const char *filename) {
    static std::string outfilename;

    outfilename.assign (outdir);
    outfilename.append ("/");
    outfilename.append (filename);
    file.open (outfilename.c_str ());
    if (file.is_open () && !overwrite) {
        fprintf (stderr, "Failed to overwrite %s.\n", outfilename.c_str ());
        file.close ();
        exit (1);
    }
    return;
}

void
GenerateCode::generate_code () {
    std::list <declaration>::iterator it2;
    std::list <umlClassNode> tmplist = getDia ().getUml ();
    
    for (umlClassNode & it : tmplist) {
        if (!(is_present (getDia ().getGenClasses (),
                         it.getName ().c_str ()) ^ getDia ().getInvertSel ())) {
            getDia ().push (it);
        }
    }

    /* Generate a file for each outer declaration.  */
    it2 = getDia ().getDeclBegin ();
    while (it2 != getDia ().getDeclEnd ()) {
        std::string name, tmpname;
        std::string filename;

        if ((*it2).decl_kind == dk_module) {
            name = (*it2).u.this_module->pkg.getName ();
        } else {         /* dk_class */
            name = (*it2).u.this_class->getName ();
        }
        filename.assign (name);
        filename.append (".");
        filename.append (getFileExt ());

        open_outfile (filename.c_str ());

        writeLicense ();

        tmpname = strtoupper (name);
        if (indentlevel != 0) {
            fprintf (stderr, "indent level (%d) should be 0.\n", indentlevel);
        }
        writeStartHeader (tmpname);

        getDia ().cleanIncludes ();
        getDia ().determine_includes (*it2);
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
            file << "\n";
        }
        for (std::string namei : incfile) {
            if (namei.compare (name)) {
                writeInclude (namei + "." + getFileExt ());
            }
        }

        gen_decl (*it2);

        writeEndHeader ();
        if (indentlevel != 0) {
            fprintf (stderr, "indent level (%d) should be 0.\n", indentlevel);
        }
        file.close ();

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
    return file;
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
is_enum_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "enum") ||
            !strcasecmp (stereo, "enumeration")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAEnum")
#endif
            );
}

int
is_struct_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "struct") ||
            !strcasecmp (stereo, "structure")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAStruct")
#endif
            );
}

int
is_typedef_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "typedef")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBATypedef")
#endif
            );
}

int
is_const_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "const") ||
            !strcasecmp (stereo, "constant")
#ifdef ENABLE_CORBA
            || !strcmp (stereo, "CORBAConstant")
#endif
            );
}

int
GenerateCode::pass_by_reference (umlClass &cl) {
    const char *st;
    st = cl.getStereotype ().c_str ();
    if (strlen (st) == 0) {
        return 1;
    }
    if (is_typedef_stereo (st)) {
        umlClassNode *ref = find_by_name (dia.getUml (),
                                          cl.getName ().c_str ());
        if (ref == NULL) {
            return 0;
        }
        return pass_by_reference (*ref);
    }
    return (!is_const_stereo (st) &&
            !is_enum_stereo (st));
}

#ifdef ENABLE_CORBA
static int
is_oo_class (umlClass &cl) {
    const char *st;
    st = cl.getStereotype ().c_str ();
    if (strlen (st) == 0) {
        return 1;
    }
    return (!is_const_stereo (st) &&
            !is_typedef_stereo (st) &&
            !is_enum_stereo (st) &&
            !is_struct_stereo (st) &&
            !eq (st, "CORBAUnion") &&
            !eq (st, "CORBAException"));
}
#endif

const char *
GenerateCode::cppname (std::string name) const {
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
GenerateCode::gen_class (const umlClassNode & node) {
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
            file << spc () << "public:\n";
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
                    file << spc () << fqname (*ref, true);
                }
                else {
                    file << spc () << cppname (umla.getType ());
                }
                if (!bOpenBraceOnNewline) {
                    file << " " << member << " () { return _" << member
                         << "; }\n";
                }
                else {
                    file << " " << member << " ()\n" << spc () << "{\n";
                    incIndentLevel ();
                    file << spc () << "return _" << member << ";\n";
                    decIndentLevel ();
                    file << spc () << "}\n";
                }
                file << spc () << "void " << member << " (";
                if (ref != NULL) {
                    int by_ref = pass_by_reference (*ref);
                    if (by_ref) {
                        file << "const ";
                    }
                    file << fqname (*ref, true);
                    if (by_ref) {
                        file << "&";
                    }
                }
                else {
                    file << cppname (umla.getType ());
                }
                if (!bOpenBraceOnNewline) {
                    file << " value_) { _" << member << " = value_; }\n";
                }
                else {
                    file << " value_)\n" << spc () << "{\n";
                    incIndentLevel ();
                    file << spc () << "_" << member << " = value_;";
                    decIndentLevel ();
                    file << spc () << "}\n";
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
            file << spc () << "public :\n";
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
        file << "\n";
        decIndentLevel ();
        writeComment ("State member implementation");
        file << spc () << "private :\n";
        incIndentLevel ();
        for (const umlAttribute & umla : node.getAttributes ()) {
            umlClassNode *ref = find_by_name (dia.getUml (),
                                              umla.getType ().c_str ());
            file << spc ();
            if (ref != NULL) {
                file << fqname (*ref, is_oo_class (*ref));
                /*
                 * FIXME: Find a better way to decide whether to use
                 * a pointer.
                */
            }
            else {
                file << cppname (umla.getType ());
            }
            file << " _" << umla.getName () << ";\n";
        }
    }
#endif

    decIndentLevel ();
    file << spc () << "};\n";
}


void
GenerateCode::gen_decl (declaration &d) {
#ifdef ENABLE_CORBA
    const char *name;
#endif
    const char *stype;
    const umlClassNode *node;
    std::list <umlAttribute>::const_iterator umla;

    if (d.decl_kind == dk_module) {
        for (declaration & it : d.u.this_module->contents) {
            gen_decl (it);
        }
        return;
    }

    file << "\n";
    writeNameSpaceStart (d.u.this_class);

    node = d.u.this_class;
    stype = node->getStereotype ().c_str ();
#ifdef ENABLE_CORBA
    name = node->getName ().c_str ();
#endif
    umla = node->getAttributes ().begin ();

    if (strlen (stype) == 0) {
        gen_class (*node);
        writeNameSpaceEnd (d.u.this_class);
        return;
    }

#ifdef ENABLE_CORBA
    if (eq (stype, "CORBANative")) {
        writeComment (std::string ("CORBANative: ") +
                      node->getName () +
                      std::string ("\n"));
    }
    else
#endif
    if (is_const_stereo (stype)) {
        writeConst (*node);
    }
    else if (is_enum_stereo (stype)) {
        writeEnum (*node);
    }
    else if (is_struct_stereo (stype)) {
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
            file << spc () << "class " << name << "\n";
            file << spc () << "{\n";
        }
        else {
            writeComment ("CORBAUnion");
            file << spc () << "class " << name << " {\n";
        }
        file << spc () << "public :\n";
        incIndentLevel ();
        file << spc () << (*umla).getType () << " _d();\n\n";
        ++umla;
        while (umla != node->getAttributes ().end ()) {
            (*umla).check (name);
            file << spc () << cppname ((*umla).getType ()) << " "
                 << (*umla).getName () << " ();\n";
            file << spc () << "void " << (*umla).getName () << " ("
                 << cppname ((*umla).getType ())
                 << " _value);\n\n";
            ++umla;
        }
        decIndentLevel ();
        file << spc () << "};\n\n";

    }
#endif
    else if (is_typedef_stereo (stype)) {
        writeTypedef (*node);
    }
    else {
        gen_class (*node);
    }

    writeNameSpaceEnd (d.u.this_class);
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
