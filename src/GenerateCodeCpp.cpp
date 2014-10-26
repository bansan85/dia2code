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
#include "GenerateCodeCpp.hpp"

#define SPEC_EXT "h"
#define BODY_EXT "cpp"

#define eq  !strcmp

GenerateCodeCpp::GenerateCodeCpp (DiaGram & diagram) :
    GenerateCode (diagram, "hpp") {
}

void
GenerateCodeCpp::generate_code ()
{
    declaration *d;
    umlclasslist tmplist = getDia ().getUml ();
    FILE *licensefile = NULL;

    if (getFileExt () == NULL)
        setFileExt ("hpp");
    /*
    if (body_file_ext == NULL)
        body_file_ext = "cpp";
     */

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

GenerateCodeCpp::~GenerateCodeCpp () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
