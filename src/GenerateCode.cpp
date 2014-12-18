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
    bOpenBraceOnNewline (false) {
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
        if (!is_present (getDia ().getGenClasses (),
                         it.getName().c_str ()) ^ getDia ().getInvertSel ()) {
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
            name = (*it2).u.this_class->getName();
        }
        filename.assign (name);
        filename.append (".");
        filename.append (getFileExt ());

        open_outfile (filename.c_str ());

        writeLicense ();

        tmpname = strtoupper (name);
        writeStartHeader (tmpname);

        getDia ().cleanIncludes ();
        getDia ().determine_includes (*it2);
        if (getDia ().getUseCorba ()) {
            writeInclude ("p_orb.h");
        }
        std::list <std::string> incfile = getDia ().getIncludes ();
        for (std::string namei : incfile) {
            if (namei.compare (name)) {
                writeInclude (namei + "." + getFileExt ());
            }
        }
        if (!incfile.empty ()) {
            file << "\n";
        }

        gen_decl (*it2);

        writeEndHeader ();
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
            !strcasecmp (stereo, "enumeration") ||
            !strcmp (stereo, "CORBAEnum"));
}

int
is_struct_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "struct") ||
            !strcasecmp (stereo, "structure") ||
            !strcmp (stereo, "CORBAStruct"));
}

int
is_typedef_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "typedef") ||
            !strcmp (stereo, "CORBATypedef"));
}

int
is_const_stereo (const char *stereo) {
    return (!strcasecmp (stereo, "const") ||
            !strcasecmp (stereo, "constant") ||
            !strcmp (stereo, "CORBAConstant"));
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
                                          cl.getName().c_str ());
        if (ref == NULL) {
            return 0;
        }
        return pass_by_reference (*ref);
    }
    return (!is_const_stereo (st) &&
            !is_enum_stereo (st));
}

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

const char *
GenerateCode::cppname (std::string name) const {
    static std::string buf;
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
    } else {
        buf.assign (name);
    }
    return buf.c_str ();
}


const char *
GenerateCode::fqname (const umlClassNode &node, bool use_ref_type) {
    static std::string buf;

    buf.clear ();
    if (node.getPackage () != NULL) {
        std::list <umlPackage> pkglist;
        umlPackage::make_package_list (node.getPackage (), pkglist);
        for (umlPackage & it : pkglist) {
            buf.append (strPackage (it.getName ().c_str ()));
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
GenerateCode::check_visibility (int *curr_vis, int new_vis) {
    if (*curr_vis == new_vis) {
        return;
    }
    indentlevel--;
    switch (new_vis) {
        case '0':
            file << spc () << "public :\n";
            break;
        case '1':
            file << spc () << "private :\n";
            break;
        case '2':
            file << spc () << "protected :\n";
            break;
        case '3':
            file << spc () << "implemetation :\n";
            break;
        default :
            fprintf (stderr, "Unknown visibility %d\n", new_vis);
            exit (1);
            break;
    }
    *curr_vis = new_vis;
    indentlevel++;
}

void
GenerateCode::gen_class (umlClassNode *node) {
    const char *name = node->getName ().c_str ();
    const char *stype = node->getStereotype ().c_str ();
    int is_valuetype = 0;

    if (strlen (stype) > 0) {
        file << spc () << "// " << stype << "\n";
        is_valuetype = eq (stype, "CORBAValue");
    }

    file << spc () << "/// class " << name << " - " << node->getComment ()
         << "\n";

    if (!node->getTemplates ().empty ()) {
        if (is_valuetype) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else {
            std::list <std::pair <std::string, std::string> >::const_iterator
                                    template_ = node->getTemplates ().begin ();
            file << spc () << "template <";
            while (template_ != node->getTemplates ().end ()) {
                file << (*template_).second << " " << (*template_).first;
                ++template_;
                if (template_ != node->getTemplates ().end ()) {
                    file << ", ";
                }
            }
            file << ">\n";
        }
    }

    file << spc () << "class " << name;
    if (!node->getParents ().empty ()) {
        std::list <umlClassNode>::const_iterator parent;
        parent = node->getParents ().begin ();
        file << " : ";
        while (parent != node->getParents ().end ()) {
            file << "public " << fqname (*parent, false);
            ++parent;
            if (parent != node->getParents ().end ()) {
                file << ", ";
            }
        }
    } else if (is_valuetype) {
        file << " : " << strPackage ("CORBA") << "ValueBase";
    }
    if (bOpenBraceOnNewline) {
        file << "\n{\n";
    }
    else {
        file << " {\n";
    }
    indentlevel++;

    if (!node->getAssociations ().empty ()) {
        file << spc () << "// Associations\n";
        /*
         * The associations are mapped as private members.
         * Is that really what we want?
         * (For example, other UML tools additionally generate
         * setters/getters.)  Ideas and comments welcome.
        */
        for (const umlassoc & assoc : node->getAssociations ()) {
            if (!assoc.name.empty ()) {
                umlClassNode *ref;
                ref = find_by_name (dia.getUml (),
                                    assoc.key.getName ().c_str ());
                if (ref != NULL) {
                    file << spc () << fqname (*ref, !assoc.composite);
                }
                else {
                    file << spc () << cppname (assoc.key.getName ());
                }
                file << " " << assoc.name << ";\n";
            }
        }
    }

    if (!node->getAttributes ().empty ()) {
        if (is_valuetype) {
            file << spc () << "// Public state members\n";
            file << spc () << "public:\n";
            indentlevel++;
            for (const umlAttribute & umla : node->getAttributes ()) {
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
                    indentlevel++;
                    file << spc () << "return _" << member << ";\n";
                    indentlevel--;
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
                    indentlevel++;
                    file << spc () << "_" << member << " = value_;";
                    indentlevel--;
                    file << spc () << "}\n";
                }
            }
            indentlevel--;
        }
        else {
            int tmpv = -1;
            file << spc () << "// Attributes\n";
            indentlevel++;
            for (const umlAttribute & umla : node->getAttributes ()) {
                check_visibility (&tmpv, umla.getVisibility ());
                if (!umla.getComment ().empty ()) {
                    file << spc () << "/// " << umla.getComment () << "\n";
                }
                if (umla.isStatic ()) {
                    file << spc () << "static " << umla.getType () << " "
                         << umla.getName ();
                }
                else {
                    file << spc () << umla.getType () << " "
                         << umla.getName () << "";
                }
                file << ";\n";
            }
            indentlevel--;
        }
    }

    if (!node->getOperations ().empty ()) {
        int tmpv = -1;
        file << spc () << "// Operations\n";
        if (is_valuetype) {
            file << spc () << "public:\n";
        }
        indentlevel++;
        for (const umlOperation & umlo : node->getOperations ()) {
            std::list <umlAttribute>::const_iterator tmpa;
            if (is_valuetype) {
                if (umlo.getVisibility () != '0') {
                    fprintf (stderr, "CORBAValue %s/%s: must be public\n",
                                     name, umlo.getName ().c_str ());
                }
            }
            else {
                check_visibility (&tmpv, umlo.getVisibility ());
            }

            /* print comments on operation */
            if (!umlo.getComment ().empty ()) {
                file << spc () << "/// " << umlo.getComment () << "\n";
                for (const umlAttribute & tmpa2 : umlo.getParameters ()) {
                     file << spc () << "/// @param " << tmpa2.getName ()
                          << "\t(" << kind_str(tmpa2.getKind ()) << ") "
                          << tmpa2.getComment () << "\n";
                }
            }
            /* print operation */
            file << spc ();
            if (umlo.isAbstract () || is_valuetype) {
                file << "virtual ";
            }
            if (umlo.isStatic ()) {
                if (is_valuetype) {
                    fprintf (stderr,
                             "CORBAValue %s/%s: static not supported\n",
                             name,
                             umlo.getName ().c_str ());
                }
                else {
                    file << "static ";
                }
            }
            if (!umlo.getType ().empty ()) {
                file << cppname (umlo.getType ()) << " ";
            }
            file << umlo.getName () << " (";
            tmpa = umlo.getParameters ().begin ();
            while (tmpa != umlo.getParameters ().end ()) {
                file << (*tmpa).getType () << " " << (*tmpa).getName ();
                if (!(*tmpa).getValue ().empty ()) {
                    if (is_valuetype) {
                        fprintf (stderr,
                             "CORBAValue %s/%s: param default not supported\n",
                                 name,
                                 umlo.getName ().c_str ());
                    }
                    else {
                       file << " = " << (*tmpa).getValue ();
                    }
                }
                ++tmpa;
                if (tmpa != umlo.getParameters ().end ()) {
                    file << ", ";
                }
            }
            file << ")";
            if (umlo.isConstant ()) {
                file << " const";
            }
            // virtual
            if ((umlo.isAbstract () || is_valuetype) &&
                umlo.getName ()[0] != '~') {
                file << " = 0";
            }
            file << ";\n";
        }
        indentlevel--;
    }

    if ((!node->getAttributes ().empty ()) && (is_valuetype)) {
        file << "\n";
        indentlevel--;
        file << spc () << "private:  // State member implementation\n";
        indentlevel++;
        for (const umlAttribute & umla : node->getAttributes ()) {
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

    indentlevel--;
    file << spc () << "};\n\n";
}


void
GenerateCode::gen_decl (declaration &d) {
    const char *name;
    const char *stype;
    umlClassNode *node;
    std::list <umlAttribute>::const_iterator umla;

    if (d.decl_kind == dk_module) {
        name = d.u.this_module->pkg.getName ().c_str ();
        if (bOpenBraceOnNewline) {
            file << spc () << "namespace " << name << "\n";
            file << spc () << "{\n\n";
        }
        else {
            file << spc () << "namespace " << name << " {\n\n";
        }
        indentlevel++;
        for (declaration & it : d.u.this_module->contents) {
            gen_decl (it);
        }
        indentlevel--;
        file << spc () << "};\n\n";
        return;
    }

    node = d.u.this_class;
    stype = node->getStereotype ().c_str ();
    name = node->getName ().c_str ();
    umla = node->getAttributes ().begin ();

    if (strlen (stype) == 0) {
        gen_class (node);
        return;
    }

    if (eq (stype, "CORBANative")) {
        file << spc () << "// CORBANative: " << name << " \n\n";
    }
    else if (is_const_stereo (stype)) {
        if (umla == node->getAttributes ().end ()) {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (!(*umla).getName ().empty ()) {
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);
        }

        file << spc () << "const " << cppname ((*umla).getType ()) << " "
             << name << " = " << (*umla).getValue () << ";\n\n";

    }
    else if (is_enum_stereo (stype)) {
        if (bOpenBraceOnNewline) {
            file << spc () << "enum " << name << "\n";
            file << spc () << "{\n";
        }
        else {
            file << spc () << "enum " << name << " {\n";
        }
        indentlevel++;
        while (umla != node->getAttributes ().end ()) {
            const char *literal = (*umla).getName ().c_str ();
            (*umla).check (name);
            if (!(*umla).getType ().empty ()) {
                fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
            }
            file << spc () << literal;
            if (!(*umla).getValue ().empty ()) {
                file << " = " << (*umla).getValue ();
            }
            ++umla;
            if (umla != node->getAttributes ().end ()) {
                file << ",";
            }
            file << "\n";
        }
        indentlevel--;
        file << spc () << "};\n\n";

    }
    else if (is_struct_stereo (stype)) {
        if (bOpenBraceOnNewline) {
            file << spc () << "struct " << name << "\n";
            file << spc () << "{\n";
        }
        else {
            file << spc () << "struct " << name << " {\n";
        }
        indentlevel++;
        while (umla != node->getAttributes ().end ()) {
            (*umla).check (name);
            file << spc () << cppname ((*umla).getType ()) << " "
                 << (*umla).getName ();
            if (!(*umla).getValue ().empty ()) {
                fprintf (stderr,
                         "%s/%s: ignoring getValue ()\n",
                         name,
                         (*umla).getName ().c_str ());
            }
            file << ";\n";
            ++umla;
        }
        indentlevel--;
        file << spc () << "};\n\n";

    }
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
            file << spc () << "class " << name << "\n";
            file << spc () << "{ // CORBAUnion\n";
        }
        else {
            file << spc () << "class " << name << " { // CORBAUnion\n";
        }
        file << spc () << "public:\n";
        indentlevel++;
        file << spc () << (*umla).getType () << " _d();  // body TBD\n\n";
        ++umla;
        while (umla != node->getAttributes ().end ()) {
            (*umla).check (name);
            file << spc () << cppname ((*umla).getType ()) << " "
                 << (*umla).getName () << " ();  // body TBD\n";
            file << spc () << "void " << (*umla).getName () << " ("
                 << cppname ((*umla).getType ())
                 << " _value);  // body TBD\n\n";
            ++umla;
        }
        indentlevel--;
        file << spc () << "};\n\n";

    }
    else if (is_typedef_stereo (stype)) {
        /* Conventions for CORBATypedef:
           The first (and only) attribute contains the following:
           Name:   Empty - the name is taken from the class.
           Type:   Name of the original type which is typedefed.
           Value:  Optionally contains array dimension(s) of the typedef.
                   These dimensions are given in square brackets, e.g.
                   [3][10]
         */
        if (umla == node->getAttributes ().end ()) {
            fprintf (stderr,
                  "Error: first attribute (impl type) not set at typedef %s\n",
                     name);
            exit (1);
        }
        if (!(*umla).getName ().empty ())  {
            fprintf (stderr,
 "Warning: typedef %s: ignoring name field in implementation type attribute\n",
                     name);
        }
        file << spc () << "typedef " << cppname ((*umla).getType ()) << " "
             << name << (*umla).getValue () << ";\n\n";
    }
    else {
        gen_class (node);
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
