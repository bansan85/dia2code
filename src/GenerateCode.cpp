/*
Copyright (C) 2000-2014 Javier O'Hara - Oliver Kellogg

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

/* NB: If you use CORBA stereotypes, you will need the file p_orb.h
   found in the runtime/cpp directory.  */

#include "dia2code.hpp"
#include "decls.hpp"
#include "GenerateCode.hpp"

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


const char *
GenerateCode::getLicense () {
    return license.c_str ();
}


void
GenerateCode::setLicense (char * lic) {
    license.assign (lic);

    return;
}


const char *
GenerateCode::getOutdir () {
    return outdir.c_str ();
}


const std::string *
GenerateCode::getOutdirS () {
    return &outdir;
}


void
GenerateCode::setOutdir (char * dir) {
    outdir.assign (dir);

    return;
}


bool
GenerateCode::getOverwrite () {
    return overwrite;
}


void
GenerateCode::setOverwrite (bool over) {
    overwrite = over;

    return;
}


bool
GenerateCode::getBuildTree () {
    return buildtree;
}


void
GenerateCode::setBuildTree (bool build) {
    buildtree = build;

    return;
}


bool
GenerateCode::getOpenBraceOnNewline () {
    return bOpenBraceOnNewline;
}


void
GenerateCode::setOpenBraceOnNewline (bool newline) {
    bOpenBraceOnNewline = newline;

    return;
}


void
GenerateCode::open_outfile (const char *filename)
{
    static std::string outfilename;

    outfilename.assign (outdir);
    outfilename.append ("/");
    outfilename.append (filename);
    file.open (outfilename.c_str ());
    if (file.is_open () && !overwrite) {
        fprintf (stderr, "Failed to overwrite %s.\n", outfilename.c_str ());
        file.close ();
        exit (1);
        return;
    }
    return;
}

void
GenerateCode::generate_code ()
{
    declaration *d;
    umlclasslist tmplist = getDia ().getUml ();
    FILE *licensefile = NULL;

    /* open license file */
    if (!license.empty ()) {
        licensefile = fopen (license.c_str (), "r");
        if (!licensefile) {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }

    while (tmplist != NULL) {
        if (! (is_present (getDia ().getGenClasses (), tmplist->key->name.c_str ()) ^ getDia ().getInvertSel ())) {
            getDia ().push (tmplist);
        }
        tmplist = tmplist->next;
    }

    /* Generate a file for each outer declaration.  */
    d = decls;
    while (d != NULL) {
        std::string name, tmpname;
        std::string filename;

        if (d->decl_kind == dk_module) {
            name = d->u.this_module->pkg->name;
        } else {         /* dk_class */
            name = d->u.this_class->key->name;
        }
        filename.assign (name);
        filename.append (".");
        filename.append (getFileExt ());

        open_outfile (filename.c_str ());

        tmpname = strtoupper(name);
        file << spc () << "#ifndef " << tmpname.c_str () << "__HPP\n";
        file << spc () << "#define " << tmpname.c_str () << "__HPP\n\n";

        /* add license to the header */
        if (!license.empty ()) {
            int lc;
            rewind (licensefile);
            while ((lc = fgetc (licensefile)) != EOF)
                file << static_cast <char> (lc);
        }

        getDia ().cleanIncludes ();
        getDia ().determine_includes (d);
        if (getDia ().getUseCorba ())
            file << "#include <p_orb.h>\n\n";
        std::list <std::string> incfile = getDia ().getIncludes ();
        for (std::string namei : incfile) {
            if (namei.compare (name)) {
                file << "#include \"" << namei.c_str () << "." << getFileExt () << "\"\n";
            }
        }
        file << "\n";

        gen_decl (d);

        file << "#endif\n";
        file.close ();

        d = d->next;
    }
    if (licensefile != NULL)
        fclose (licensefile);
}

const char *
GenerateCode::getFileExt () {
    return file_ext.c_str ();
}


void
GenerateCode::setFileExt (char * ext) {
    file_ext.assign (ext);

    return;
}


const char *
GenerateCode::getBodyFileExt () {
    return body_file_ext.c_str ();
}


void
GenerateCode::setBodyFileExt (char * ext) {
    body_file_ext.assign (ext);

    return;
}


static void
check_umlattr (umlattribute &u, const char *typename_)
{
    /* Check settings that don't make sense for C++ generation.  */
    if (u.visibility == '1')
        fprintf (stderr, "%s/%s: ignoring non-visibility\n", typename_, u.name.c_str ());
    if (u.isstatic)
        fprintf (stderr, "%s/%s: ignoring staticness\n", typename_, u.name.c_str ());
}

char *
subst (char *str, const char search, char replace)
{
    char *p;
    while ((p = strchr (str, search)) != NULL)
        *p = replace;
    return str;
}

char *
nospc (char *str)
{
    return subst (str, ' ', '_');
}

int
GenerateCode::pass_by_reference (umlclass *cl)
{
    const char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype.c_str ();
    if (strlen (st) == 0)
        return 1;
    if (is_typedef_stereo (st)) {
        umlclassnode *ref = find_by_name (dia.getUml (), cl->name.c_str ());
        if (ref == NULL)
            return 0;
        return pass_by_reference (ref->key);
    }
    return (!is_const_stereo (st) &&
            !is_enum_stereo (st));
}

static int
is_oo_class (umlclass *cl)
{
    const char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype.c_str ();
    if (strlen (st) == 0)
        return 1;
    return (!is_const_stereo (st) &&
            !is_typedef_stereo (st) &&
            !is_enum_stereo (st) &&
            !is_struct_stereo (st) &&
            !eq (st, "CORBAUnion") &&
            !eq (st, "CORBAException"));
}

const char *
GenerateCode::cppname (std::string name)
{
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
                buf.assign ("CORBA::");
                buf.append (nospc (const_cast <char *> (strtoupperfirst (name).c_str ())));
        } else if (name.compare ("long long") == 0) {
            buf.assign ("CORBA::LongLong");
        } else if (name.compare ("unsigned short") == 0) {
            buf.assign ("CORBA::UShort");
        } else if (name.compare ("unsigned long") == 0) {
            buf.assign ("CORBA::ULong");
        } else if (name.compare ("unsigned long long") == 0) {
            buf.assign ("CORBA::ULongLong");
        } else {
            buf.assign (name);
        }
    } else {
        buf.assign (name);
    }
    return buf.c_str ();
}


const char *
fqname (umlclassnode *node, bool use_ref_type)
{
    static std::string buf;

    buf.clear ();
    if (node == NULL)
        return "";
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL) {
            buf.append (pkglist->key->name);
            buf.append ("::");
            pkglist = pkglist->next;
        }
    }
    buf.append (node->key->name);
    if (use_ref_type)
        buf.append ("*");
    return buf.c_str ();
}

void
GenerateCode::check_visibility (int *curr_vis, int new_vis)
{
    if (*curr_vis == new_vis)
        return;
    indentlevel--;
    switch (new_vis) {
        case '0':
            file << spc () << "public:\n";
            break;
        case '1':
            file << spc () << "private:\n";
            break;
        case '2':
            file << spc () << "protected:\n";
            break;
        default :
            fprintf (stderr, "Unknown visibility %d\n", new_vis);
            break;
    }
    *curr_vis = new_vis;
    indentlevel++;
}

void
GenerateCode::gen_class (umlclassnode *node)
{
    const char *name = node->key->name.c_str ();
    const char *stype = node->key->stereotype.c_str ();
    int is_valuetype = 0;

    if (strlen (stype) > 0) {
        file << spc () << "// " << stype << "\n";
        is_valuetype = eq (stype, "CORBAValue");
    }

    file << spc () << "/// class " << name << " - " << node->key->comment.c_str () << "\n";

    if (!node->key->templates.empty ()) {
        std::list <umltemplate>::iterator template_ = node->key->templates.begin ();
        if (is_valuetype) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else {
            file << spc () << "template <";
            while (template_ != node->key->templates.end ()) {
                file << (*template_).type.c_str () << " " << (*template_).name.c_str ();
                ++template_;
                if (template_ != node->key->templates.end ())
                    file << ", ";
            }
            file << ">\n";
        }
    }

    file << spc () << "class " << name;
    if (node->parents != NULL) {
        umlclasslist parent = node->parents;
        file << " : ";
        while (parent != NULL) {
            file << "public " << fqname (parent, false);
            parent = parent->next;
            if (parent != NULL)
                file << ", ";
        }
    } else if (is_valuetype) {
        file << " : CORBA::ValueBase";
    }
    if (bOpenBraceOnNewline) {
        file << "\n{\n";
    }
    else {
        file << " {\n";
    }
    indentlevel++;

    if (node->associations != NULL) {
        umlassoclist assoc = node->associations;
        file << spc () << "// Associations\n";
        /*
         * The associations are mapped as private members.
         * Is that really what we want?
         * (For example, other UML tools additionally generate
         * setters/getters.)  Ideas and comments welcome.
        */
        while (assoc != NULL) {
            umlclassnode *ref;
            if (!assoc->name.empty ())
            {
                ref = find_by_name (dia.getUml (), assoc->key->name.c_str ());
                if (ref != NULL)
                    file << spc () << fqname (ref, !assoc->composite);
                else
                    file << spc () << cppname (assoc->key->name);
                file << " " << assoc->name.c_str () << ";\n";
            }
            assoc = assoc->next;
        }
    }

    if (!node->key->attributes.empty ()) {
        std::list <umlattribute>::iterator umla = node->key->attributes.begin ();
        if (is_valuetype) {
            file << spc () << "// Public state members\n";
            indentlevel--;
            file << spc () << "public:\n";
            indentlevel++;
            while (umla != node->key->attributes.end ()) {
                const char *member = (*umla).name.c_str ();
                umlclassnode *ref;
                if ((*umla).visibility != '0') {
                    ++umla;
                    continue;
                }
                if ((*umla).isstatic) {
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n",
                                     name, member);
                }
                ref = find_by_name (dia.getUml (), (*umla).type.c_str ());
                if (ref != NULL)
                    file << spc () << fqname (ref, true);
                else
                    file << spc () << cppname ((*umla).type);
                if (!bOpenBraceOnNewline) {
                    file << " " << member << " () { return _" << member << "; }\n";
                }
                else {
                    file << " " << member << " ()\n";
                    file << spc () << "{\n";
                    indentlevel++;
                    file << spc () << "return _" << member << ";\n";
                    indentlevel--;
                    file << spc () << "}\n";
                }
                file << spc () << "void " << member << " (";
                if (ref != NULL) {
                    int by_ref = pass_by_reference (ref->key);
                    if (by_ref)
                        file << "const ";
                    file << fqname (ref, true);
                    if (by_ref)
                        file << "&";
                } else {
                    file << cppname ((*umla).type);
                }
                if (!bOpenBraceOnNewline) {
                    file << " value_) { _" << member << " = value_; }\n";
                }
                else {
                    file << " value_)\n";
                    file << spc () << "{\n";
                    indentlevel++;
                    file << spc () << "_" << member << " = value_;";
                    indentlevel--;
                    file << spc () << "}\n";
                }
                ++umla;
            }
        } else {
            int tmpv = -1;
            file << spc () << "// Attributes\n";
            while (umla != node->key->attributes.end ()) {
                check_visibility (&tmpv, (*umla).visibility);
                if (!(*umla).comment.empty ()) {
                    file << spc () << "/// " << (*umla).comment.c_str () << "\n";
                }
                file << spc ();
                if ((*umla).isstatic) {
                    file << spc () << "static " << (*umla).type.c_str () << " " << (*umla).name.c_str ();
                }
                else
                {
                    file << spc () << (*umla).type.c_str () << " " << (*umla).name.c_str () << "";
                }
                file << ";\n";
                ++umla;
            }
        }
    }

    if (!node->key->operations.empty ()) {
        std::list <umloperation>::iterator umlo = node->key->operations.begin ();
        int tmpv = -1;
        file << spc () << "// Operations\n";
        if (is_valuetype) {
            indentlevel--;
            file << spc () << "public:\n";
            indentlevel++;
        }
        while (umlo != node->key->operations.end ()) {
            std::list <umlattribute>::iterator tmpa;
            if (is_valuetype) {
                if ((*umlo).attr.visibility != '0')
                    fprintf (stderr, "CORBAValue %s/%s: must be public\n",
                                     name, (*umlo).attr.name.c_str ());
            } else {
                check_visibility (&tmpv, (*umlo).attr.visibility);
            }

            /* print comments on operation */
            if (!(*umlo).attr.comment.empty ()) {
                file << spc () << "/// " << (*umlo).attr.comment.c_str () << "\n";
                tmpa = (*umlo).parameters.begin ();
                while (tmpa != (*umlo).parameters.end ()) {
                     file << spc () << "/// @param " << (*tmpa).name.c_str () << "\t(" << kind_str((*tmpa).kind) << ") " << (*tmpa).comment.c_str () << "\n";
                     ++tmpa;
                }
            }
            /* print operation */
            file << spc ();
            if ((*umlo).attr.isabstract || is_valuetype) {
                file << "virtual ";
                (*umlo).attr.value.clear ();
            }
            if ((*umlo).attr.isstatic) {
                if (is_valuetype)
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n",
                                     name, (*umlo).attr.name.c_str ());
                else
                    file << "static ";
            }
            if (!(*umlo).attr.type.empty ()) {
                file << cppname ((*umlo).attr.type.c_str ()) << " ";
            }
            file << (*umlo).attr.name.c_str () << " (";
            tmpa = (*umlo).parameters.begin ();
            while (tmpa != (*umlo).parameters.end ()) {
                file << (*tmpa).type.c_str () << " " << (*tmpa).name.c_str ();
                if (!(*tmpa).value.empty ()) {
                    if (is_valuetype)
                        fprintf (stderr, "CORBAValue %s/%s: param default "
                                 "not supported\n", name, (*umlo).attr.name.c_str ());
                    else
                       file << " = " << (*tmpa).value.c_str ();
                }
                ++tmpa;
                if (tmpa != (*umlo).parameters.end ()) {
                    file << ", ";
                }
            }
            file << ")";
            if ((*umlo).attr.isconstant) {
                file << " const";
            }
            if ((*umlo).attr.value.empty ()) {
                // virtual
                if (((*umlo).attr.isabstract || is_valuetype) &&
                    (*umlo).attr.name[0] != '~')
                    file << " = 0";
            }
            file << ";\n";
            ++umlo;
        }
    }

    if ((!node->key->attributes.empty ()) && (is_valuetype)) {
        std::list <umlattribute>::iterator umla = node->key->attributes.begin ();
        file << "\n";
        indentlevel--;
        file << spc () << "private:  // State member implementation\n";
        indentlevel++;
        while (umla != node->key->attributes.end ()) {
            umlclassnode *ref = find_by_name (dia.getUml (), (*umla).type.c_str ());
            file << spc ();
            if (ref != NULL) {
                file << fqname (ref, is_oo_class (ref->key));
                /*
                 * FIXME: Find a better way to decide whether to use
                 * a pointer.
                */
            } else
                file << cppname ((*umla).type);
            file << " _" << (*umla).name.c_str () << ";\n";
            ++umla;
        }
    }

    indentlevel--;
    file << spc () << "};\n\n";
}


void
GenerateCode::gen_decl (declaration *d)
{
    const char *name;
    const char *stype;
    umlclassnode *node;
    std::list <umlattribute>::iterator umla;

    if (d == NULL)
        return;

    if (d->decl_kind == dk_module) {
        name = d->u.this_module->pkg->name.c_str ();
        if (bOpenBraceOnNewline) {
            file << spc () << "namespace " << name << "\n";
            file << spc () << "{\n\n";
        }
        else
        {
            file << spc () << "namespace " << name << " {\n\n";
        }
        indentlevel++;
        d = d->u.this_module->contents;
        while (d != NULL) {
            gen_decl (d);
            d = d->next;
        }
        indentlevel--;
        file << spc () << "};\n\n";
        return;
    }

    node = d->u.this_class;
    stype = node->key->stereotype.c_str ();
    name = node->key->name.c_str ();
    umla = node->key->attributes.begin ();

    if (strlen (stype) == 0) {
        gen_class (node);
        return;
    }

    if (eq (stype, "CORBANative")) {
        file << spc () << "// CORBANative: " << name << " \n\n";

    } else if (is_const_stereo (stype)) {
        if (umla == node->key->attributes.end ()) {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (!(*umla).name.empty ())
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        file << spc () << "const " << cppname ((*umla).type) << " " << name << " = " << (*umla).value.c_str () << ";\n\n";

    } else if (is_enum_stereo (stype)) {
        if (bOpenBraceOnNewline) {
            file << spc () << "enum " << name << "\n";
            file << spc () << "{\n";
        }
        else
        {
            file << spc () << "enum " << name << " {\n";
        }
        indentlevel++;
        while (umla != node->key->attributes.end ()) {
            const char *literal = (*umla).name.c_str ();
            check_umlattr (*umla, name);
            if (!(*umla).type.empty ())
                fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
            file << spc () << literal;
            if (!(*umla).value.empty ())
                file << " = " << (*umla).value.c_str ();
            ++umla;
            if (umla != node->key->attributes.end ())
                file << ",";
            file << "\n";
        }
        indentlevel--;
        file << spc () << "};\n\n";

    } else if (is_struct_stereo (stype)) {
        if (bOpenBraceOnNewline) {
            file << spc () << "struct " << name << "\n";
            file << spc () << "{\n";
        }
        else
        {
            file << spc () << "struct " << name << " {\n";
        }
        indentlevel++;
        while (umla != node->key->attributes.end ()) {
            check_umlattr (*umla, name);
            file << spc () << cppname ((*umla).type) << " " << (*umla).name.c_str ();
            if (!(*umla).value.empty ())
                fprintf (stderr, "%s/%s: ignoring value\n",
                                 name, (*umla).name.c_str ());
            file << ";\n";
            ++umla;
        }
        indentlevel--;
        file << spc () << "};\n\n";

    } else if (eq (stype, "CORBAException")) {
        fprintf (stderr, "%s: CORBAException not yet implemented\n", name);

    } else if (eq (stype, "CORBAUnion")) {
        if (umla == node->key->attributes.end ()) {
            fprintf (stderr, "Error: attributes not set at union %s\n", name);
            exit (1);
        }
        fprintf (stderr, "%s: CORBAUnion not yet fully implemented\n", name);
        if (bOpenBraceOnNewline) {
            file << spc () << "class " << name << "\n";
            file << spc () << "{ // CORBAUnion\n";
        }
        else
        {
            file << spc () << "class " << name << " { // CORBAUnion\n";
        }
        file << spc () << "public:\n";
        indentlevel++;
        file << spc () << (*umla).type.c_str () << " _d();  // body TBD\n\n";
        ++umla;
        while (umla != node->key->attributes.end ()) {
            check_umlattr (*umla, name);
            file << spc () << cppname ((*umla).type) << " " << (*umla).name.c_str () << " ();  // body TBD\n";
            file << spc () << "void " << (*umla).name.c_str () << " (" << cppname ((*umla).type) << " _value);  // body TBD\n\n";
            ++umla;
        }
        indentlevel--;
        file << spc () << "};\n\n";

    } else if (is_typedef_stereo (stype)) {
        /* Conventions for CORBATypedef:
           The first (and only) attribute contains the following:
           Name:   Empty - the name is taken from the class.
           Type:   Name of the original type which is typedefed.
           Value:  Optionally contains array dimension(s) of the typedef.
                   These dimensions are given in square brackets, e.g.
                   [3][10]
         */
        if (umla == node->key->attributes.end ()) {
            fprintf (stderr, "Error: first attribute (impl type) not set "
                             "at typedef %s\n", name);
            exit (1);
        }
        if (!(*umla).name.empty ())  {
            fprintf (stderr, "Warning: typedef %s: ignoring name field "
                        "in implementation type attribute\n", name);
        }
        file << spc () << "typedef " << cppname ((*umla).type) << " " << name << (*umla).value.c_str () << ";\n\n";
    } else {
        gen_class (node);
    }
}


std::string
GenerateCode::spc()
{
    std::string spcbuf ("");
    int n_spaces = indent * indentlevel, i;
    
    
    for (i=0; i<n_spaces; i++)
        spcbuf.append (" ");
    
    return spcbuf;
}


uint32_t
GenerateCode::getIndent () {
    return indent;
}


void
GenerateCode::setIndent (uint8_t spaces) {
    if ((spaces < 1) || (spaces > 8))
        return;
    
    indent = spaces & 15;

    return;
}


GenerateCode::~GenerateCode () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
