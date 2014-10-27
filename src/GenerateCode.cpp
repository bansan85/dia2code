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
    file_ext (ext),
    file (NULL),
    indent (4),
    indentlevel (0) {
}


DiaGram &
GenerateCode::getDia () {
    return dia;
}


void
GenerateCode::emit (char *msg, ...)
{
    var_arg_to_str (msg);
    fputs (str, file);
}

void
GenerateCode::eboth (char *msg, ...)
{
    var_arg_to_str (msg);
    fputs (str, file);
    if (body != NULL)
        fputs (str, body);
}


void
GenerateCode::open_outfile (char *filename)
{
    static char outfilename[BIG_BUFFER];
    int tmpdirlgth, tmpfilelgth;

    tmpdirlgth = strlen (dia.getOutdir ());
    tmpfilelgth = strlen (filename);

    /* This prevents buffer overflows */
    if (tmpfilelgth + tmpdirlgth > sizeof(outfilename) - 2) {
        fprintf (stderr, "Sorry, name of file too long ...\n"
                    "Try a smaller dir name\n");
        exit (1);
    }

    sprintf (outfilename, "%s/%s", dia.getOutdir (), filename);
    file = fopen (outfilename, "r");
    if (file != NULL && !overwrite) {
        fclose (file);
        return;
    }
    file = fopen (outfilename, "w");
    if (file == NULL) {
        fprintf (stderr, "Can't open file %s for writing\n", outfilename);
        exit (1);
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
    if (getDia ().getLicense () != NULL) {
        licensefile = fopen (getDia ().getLicense (), "r");
        if (!licensefile) {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }

    while (tmplist != NULL) {
        if (! (is_present (getDia ().getGenClasses (), tmplist->key->name) ^ getDia ().getInvertSel ())) {
            getDia ().push (tmplist);
        }
        tmplist = tmplist->next;
    }

    /* Generate a file for each outer declaration.  */
    d = decls;
    while (d != NULL) {
        char *name, *tmpname;
        char filename[BIG_BUFFER];

        if (d->decl_kind == dk_module) {
            name = d->u.this_module->pkg->name;
        } else {         /* dk_class */
            name = d->u.this_class->key->name;
        }
        sprintf (filename, "%s.%s", name, getFileExt ());

        spec = getDia ().open_outfile (filename);
        if (spec == NULL) {
            d = d->next;
            continue;
        }

        tmpname = strtoupper(name);
        print("#ifndef %s__H\n", tmpname);
        print("#define %s__H\n\n", tmpname);

        /* add license to the header */
        if (getDia ().getLicense ()) {
            int lc;
            rewind (licensefile);
            while ((lc = fgetc (licensefile)) != EOF)
                print ("%c", (char) lc);
        }

        getDia ().cleanIncludes ();
        getDia ().determine_includes (d);
        if (getDia ().getUseCorba ())
            print ("#include <p_orb.h>\n\n");
        std::list <std::string> incfile = getDia ().getIncludes ();
        for (std::string namei : incfile) {
            if (namei.compare (name)) {
                print ("#include \"%s.%s\"\n", namei.c_str (), getFileExt ());
            }
        }
        print ("\n");

        gen_decl (d);

        print("#endif\n");
        fclose (spec);

        d = d->next;
    }
}

char *
GenerateCode::getFileExt () {
    return file_ext.c_str ();
}


void
GenerateCode::setFileExt (char * ext) {
    file_ext.assign (ext);

    return;
}


static void
check_umlattr (umlattribute *u, char *typename_)
{
    /* Check settings that don't make sense for C++ generation.  */
    if (u->visibility == '1')
        fprintf (stderr, "%s/%s: ignoring non-visibility\n", typename_, u->name);
    if (u->isstatic)
        fprintf (stderr, "%s/%s: ignoring staticness\n", typename_, u->name);
}

static char *
subst (char *str, char search, char replace)
{
    char *p;
    while ((p = strchr (str, search)) != NULL)
        *p = replace;
    return str;
}

static char *
nospc (char *str)
{
    return subst (str, ' ', '_');
}

int
GenerateCode::pass_by_reference (umlclass *cl)
{
    char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype;
    if (strlen (st) == 0)
        return 1;
    if (is_typedef_stereo (st)) {
        umlclassnode *ref = find_by_name (dia.getUml (), cl->name);
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
    char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype;
    if (strlen (st) == 0)
        return 1;
    return (!is_const_stereo (st) &&
            !is_typedef_stereo (st) &&
            !is_enum_stereo (st) &&
            !is_struct_stereo (st) &&
            !eq (st, "CORBAUnion") &&
            !eq (st, "CORBAException"));
}

static int
has_oo_class (declaration *d)
{
    while (d != NULL) {
        if (d->decl_kind == dk_module) {
            if (has_oo_class (d->u.this_module->contents))
                return 1;
        } else {         /* dk_class */
            if (is_oo_class (d->u.this_class->key))
                return 1;
        }
        d = d->next;
    }
    return 0;
}

char *
GenerateCode::cppname (char *name)
{
    static char buf[SMALL_BUFFER];
    if (dia.getUseCorba ()) {
        if (eq (name, "boolean") ||
            eq (name, "char") ||
            eq (name, "octet") ||
            eq (name, "short") ||
            eq (name, "long") ||
            eq (name, "float") ||
            eq (name, "double") ||
            eq (name, "string") ||
            eq (name, "any")) {
            sprintf (buf, "CORBA::%s", nospc (strtoupperfirst (name)));
        } else if (eq (name, "long long")) {
            sprintf (buf, "CORBA::LongLong");
        } else if (eq (name, "unsigned short")) {
            sprintf (buf, "CORBA::UShort");
        } else if (eq (name, "unsigned long")) {
            sprintf (buf, "CORBA::ULong");
        } else if (eq (name, "unsigned long long")) {
            sprintf (buf, "CORBA::ULongLong");
        } else {
            strcpy (buf, name);
        }
    } else {
        strcpy (buf, name);
    }
    return buf;
}


static char *
fqname (umlclassnode *node, int use_ref_type)
{
    static char buf[BIG_BUFFER];

    buf[0] = '\0';
    if (node == NULL)
        return buf;
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL) {
            strcat (buf, pkglist->key->name);
            strcat (buf, "::");
            pkglist = pkglist->next;
        }
    }
    strcat (buf, node->key->name);
    if (use_ref_type)
        strcat (buf, "*");
    return buf;
}

void
GenerateCode::check_visibility (int *curr_vis, int new_vis)
{
    if (*curr_vis == new_vis)
        return;
    indentlevel--;
    switch (new_vis) {
      case '0':
        print ("public:\n");
        break;
      case '1':
        print ("private:\n");
        break;
      case '2':
        print ("protected:\n");
        break;
    }
    *curr_vis = new_vis;
    indentlevel++;
}

void
GenerateCode::gen_class (umlclassnode *node)
{
    char *name = node->key->name;
    char *stype = node->key->stereotype;
    int is_valuetype = 0;

    if (strlen (stype) > 0) {
        print ("// %s\n", stype);
        is_valuetype = eq (stype, "CORBAValue");
    }

    print("/// class %s - %s\n", name, node->key->comment);

    if (node->key->templates != NULL) {
        umltemplatelist template_ = node->key->templates;
        if (is_valuetype) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else {
            print ("template <");
            while (template_ != NULL) {
                print ("%s %s", template_->key.type, template_->key.name);
                template_ = template_->next;
                if (template_ != NULL)
                    emit (", ");
            }
            emit (">\n");
        }
    }

    print ("class %s", name);
    if (node->parents != NULL) {
        umlclasslist parent = node->parents;
        emit (" : ");
        while (parent != NULL) {
            emit ("public %s", fqname (parent, 0));
            parent = parent->next;
            if (parent != NULL)
                emit (", ");
        }
    } else if (is_valuetype) {
        emit (" : CORBA::ValueBase");
    }
    emit (" {\n");
    indentlevel++;

    if (node->associations != NULL) {
        umlassoclist assoc = node->associations;
        print ("// Associations\n");
        /*
         * The associations are mapped as private members.
         * Is that really what we want?
         * (For example, other UML tools additionally generate
         * setters/getters.)  Ideas and comments welcome.
        */
        while (assoc != NULL) {
            umlclassnode *ref;
            if (assoc->name[0] != '\0')
            {
                ref = find_by_name (dia.getUml (), assoc->key->name);
                print ("");
                if (ref != NULL)
                    emit ("%s", fqname (ref, !assoc->composite));
                else
                    emit ("%s", cppname (assoc->key->name));
                emit (" %s;\n", assoc->name);
            }
            assoc = assoc->next;
        }
    }

    if (node->key->attributes != NULL) {
        umlattrlist umla = node->key->attributes;
        if (is_valuetype) {
            print ("// Public state members\n");
            indentlevel--;
            print ("public:\n");
            indentlevel++;
            while (umla != NULL) {
                char *member = umla->key.name;
                umlclassnode *ref;
                if (umla->key.visibility != '0') {
                    umla = umla->next;
                    continue;
                }
                print ("");
                if (umla->key.isstatic) {
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n",
                                     name, member);
                }
                ref = find_by_name (dia.getUml (), umla->key.type);
                if (ref != NULL)
                    eboth ("%s", fqname (ref, 1));
                else
                    eboth ("%s", cppname (umla->key.type));
                emit (" %s () { return _%s; }\n", member, member);
                print ("void %s (", member);
                if (ref != NULL) {
                    int by_ref = pass_by_reference (ref->key);
                    if (by_ref)
                        emit ("const ");
                    emit ("%s", fqname (ref, 1));
                    if (by_ref)
                        emit ("&");
                } else {
                    emit ("%s", cppname (umla->key.type));
                }
                emit (" value_) { _%s = value_; }\n");
                umla = umla->next;
            }
        } else {
            int tmpv = -1;
            print ("// Attributes\n");
            while (umla != NULL) {
                check_visibility (&tmpv, umla->key.visibility);
                if (strlen(umla->key.comment)) {
                    print("/// %s\n", umla->key.comment);
                }
                print ("");
                if (umla->key.isstatic) {
                    emit ("static ");
                }
                emit ("%s %s", umla->key.type, umla->key.name);
                emit (";\n");
                umla = umla->next;
            }
        }
    }

    if (node->key->operations != NULL) {
        umloplist umlo = node->key->operations;
        int tmpv = -1;
        print ("// Operations\n");
        if (is_valuetype) {
            indentlevel--;
            print ("public:\n");
            indentlevel++;
        }
        while (umlo != NULL) {
            umlattrlist tmpa;
            if (is_valuetype) {
                if (umlo->key.attr.visibility != '0')
                    fprintf (stderr, "CORBAValue %s/%s: must be public\n",
                                     name, umlo->key.attr.name);
            } else {
                check_visibility (&tmpv, umlo->key.attr.visibility);
            }

            /* print comments on operation */
            if (strlen(umlo->key.attr.comment)) {
                print("/// %s\n", umlo->key.attr.comment);
                tmpa = umlo->key.parameters;
                while (tmpa != NULL) {
                     print("/// @param %s\t\t(%s) %s\n",
                           tmpa->key.name,
                           kind_str(tmpa->key.kind),
                           tmpa->key.comment);
                           tmpa = tmpa->next;
                }
            }
            /* print operation */
            print ("");
            if (umlo->key.attr.isabstract || is_valuetype) {
                emit ("virtual ");
                umlo->key.attr.value[0] = '0';
            }
            if (umlo->key.attr.isstatic) {
                if (is_valuetype)
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n",
                                     name, umlo->key.attr.name);
                else
                    emit ("static ");
            }
            if (strlen (umlo->key.attr.type) > 0) {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s (", umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL) {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);
                if (tmpa->key.value[0] != 0) {
                    if (is_valuetype)
                        fprintf (stderr, "CORBAValue %s/%s: param default "
                                 "not supported\n", name, umlo->key.attr.name);
                    else
                       emit (" = %s", tmpa->key.value);
                }
                tmpa = tmpa->next;
                if (tmpa != NULL) {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant) {
                emit (" const");
            }
            if (umlo->key.attr.value[0]) {
                // virtual
                if ((umlo->key.attr.isabstract || is_valuetype) &&
                    umlo->key.attr.name[0] != '~')
                    emit (" = %s", umlo->key.attr.value);
            }
            emit (";\n");
            umlo = umlo->next;
        }
    }

    if (node->key->attributes != NULL && is_valuetype) {
        umlattrlist umla = node->key->attributes;
        emit ("\n");
        indentlevel--;
        print ("private:  // State member implementation\n");
        indentlevel++;
        while (umla != NULL) {
            umlclassnode *ref = find_by_name (dia.getUml (), umla->key.type);
            print ("");
            if (ref != NULL) {
                emit ("%s", fqname (ref, is_oo_class (ref->key)));
                /*
                 * FIXME: Find a better way to decide whether to use
                 * a pointer.
                */
            } else
                emit ("%s", cppname (umla->key.type));
            emit (" _%s;\n", umla->key.name);
            umla = umla->next;
        }
    }

    indentlevel--;
    print ("};\n\n");
}


void
GenerateCode::gen_decl (declaration *d)
{
    char *name;
    char *stype;
    umlclassnode *node;
    umlattrlist umla;

    if (d == NULL)
        return;

    if (d->decl_kind == dk_module) {
        name = d->u.this_module->pkg->name;
        print ("namespace %s {\n\n", name);
        indentlevel++;
        d = d->u.this_module->contents;
        while (d != NULL) {
            gen_decl (d);
            d = d->next;
        }
        indentlevel--;
        print ("};\n\n", name);
        return;
    }

    node = d->u.this_class;
    stype = node->key->stereotype;
    name = node->key->name;
    umla = node->key->attributes;

    if (strlen (stype) == 0) {
        gen_class (node);
        return;
    }

    if (eq (stype, "CORBANative")) {
        print ("// CORBANative: %s \n\n", name);

    } else if (is_const_stereo (stype)) {
        if (umla == NULL) {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        print ("const %s %s = %s;\n\n", cppname (umla->key.type), name,
                                                 umla->key.value);

    } else if (is_enum_stereo (stype)) {
        print ("enum %s {\n", name);
        indentlevel++;
        while (umla != NULL) {
            char *literal = umla->key.name;
            check_umlattr (&umla->key, name);
            if (strlen (umla->key.type) > 0)
                fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
            print ("%s", literal);
            if (strlen (umla->key.value) > 0)
                print (" = %s", umla->key.value);
            if (umla->next)
                emit (",");
            emit ("\n");
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    } else if (is_struct_stereo (stype)) {
        print ("struct %s {\n", name);
        indentlevel++;
        while (umla != NULL) {
            check_umlattr (&umla->key, name);
            print ("%s %s", cppname (umla->key.type), umla->key.name);
            if (strlen (umla->key.value) > 0)
                fprintf (stderr, "%s/%s: ignoring value\n",
                                 name, umla->key.name);
            emit (";\n");
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    } else if (eq (stype, "CORBAException")) {
        fprintf (stderr, "%s: CORBAException not yet implemented\n", name);

    } else if (eq (stype, "CORBAUnion")) {
        umlattrnode *sw = umla;
        if (sw == NULL) {
            fprintf (stderr, "Error: attributes not set at union %s\n", name);
            exit (1);
        }
        fprintf (stderr, "%s: CORBAUnion not yet fully implemented\n", name);
        print ("class %s {  // CORBAUnion\n", name);
        print ("public:\n", name);
        indentlevel++;
        print ("%s _d();  // body TBD\n\n", umla->key.type);
        umla = umla->next;
        while (umla != NULL) {
            check_umlattr (&umla->key, name);
            print ("%s %s ();  // body TBD\n",
                   cppname (umla->key.type), umla->key.name);
            print ("void %s (%s _value);  // body TBD\n\n", umla->key.name,
                   cppname (umla->key.type));
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    } else if (is_typedef_stereo (stype)) {
        /* Conventions for CORBATypedef:
           The first (and only) attribute contains the following:
           Name:   Empty - the name is taken from the class.
           Type:   Name of the original type which is typedefed.
           Value:  Optionally contains array dimension(s) of the typedef.
                   These dimensions are given in square brackets, e.g.
                   [3][10]
         */
        if (umla == NULL) {
            fprintf (stderr, "Error: first attribute (impl type) not set "
                             "at typedef %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)  {
            fprintf (stderr, "Warning: typedef %s: ignoring name field "
                        "in implementation type attribute\n", name);
        }
        print ("typedef %s %s%s;\n\n", cppname (umla->key.type), name,
                                                umla->key.value);
    } else {
        gen_class (node);
    }
}


char *
GenerateCode::spc()
{
   static char spcbuf[BIG_BUFFER];
   int n_spaces = indent * indentlevel;
   if (n_spaces >= sizeof(spcbuf)) {
       fprintf (stderr, "spc(): spaces buffer overflow\n");
       exit (1);
   }
   memset (spcbuf, ' ', n_spaces);
   spcbuf[n_spaces] = '\0';
   return spcbuf;
}


uint32_t
GenerateCode::getIndent () {
    return indent;
}


void
GenerateCode::print (char *msg, ...)
{
    var_arg_to_str (msg);
    fprintf (file, "%s%s", spc(), str);
}


void
GenerateCode::pbody (char *msg, ...)
{
    var_arg_to_str (msg);
    if (body != NULL)
        fprintf (body, "%s%s", spc(), str);
}

void
GenerateCode::pboth (char *msg, ...)
{
    var_arg_to_str (msg);
    fprintf (file, "%s%s", spc(), str);
    if (body != NULL)
        fprintf (body, "%s%s", spc(), str);
}


void
GenerateCode::setIndent (uint32_t spaces) {
    if ((spaces < 1) || (spaces > 8))
        return;
    
    indent = spaces;

    return;
}



GenerateCode::~GenerateCode () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
