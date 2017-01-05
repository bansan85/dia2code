/***************************************************************************
          generate_code_python.c  -  Function that generates Python code
                             -------------------
    begin                : Sat Mar 24 2001
    copyright            : (C) 2001 by Cyrille Chepelov (based on code from
                                                               Javier O'Hara)
    email                : chepelov@calixo.net

    patched by Wolf Ó Spealáin to
    - recognise class and instance variables,
    - tidy Python output,
    - fix parameter listings,
    - include diagram comments as correctly formatted docstrings.

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dia2code.h"

void generate_code_python(batch *b) {
    umlclasslist tmplist, parents;
    umlattrlist umla, tmpa;
    umloplist umlo;
    char outfilename[BIG_BUFFER];
    FILE *licensefile = NULL;
    namelist used_classes, tmpnamelist;
    int intro_was_printed;
    int interface, abstract;

    tmplist = b->classlist;

    /* open license file */
    if ( b->license != NULL ) {
        licensefile = fopen(b->license, "r");
        if(!licensefile) {
            fprintf(stderr, "Can't open the license file.\n");
            exit(2);
        }
    }

    while ( tmplist != NULL ) {

        if ( ! ( is_present(b->classes, tmplist->key->name) ^ b->mask ) ) {

            char *tmpname = tmplist->key->name;

            sprintf(outfilename, "%s.py", tmplist->key->name);
            spec = open_outfile (outfilename, b);
            if (spec == NULL) {
                tmplist = tmplist->next;
                continue;
            }

            /* add license to the header */
            if (b->license != NULL) {
                int lc;
                rewind(licensefile);
                while ((lc = fgetc(licensefile)) != EOF) {
                    print("%c", (char) lc);
                }
            }

            print("# File: %s\n\n", outfilename);
            used_classes = find_classes(tmplist, b);
            tmpnamelist = used_classes;
            while (tmpnamelist != NULL) {
                print("from %s import %s\n",
                        tmpnamelist->name, tmpnamelist->name);
                tmpnamelist = tmpnamelist->next;
            }
            if (used_classes != NULL) {
                print("\n");
            }
            tmpname = strtolower(tmplist->key->stereotype);
            interface = eq("interface", tmpname);
            abstract = tmplist->key->isabstract;
            free(tmpname);

            print("class %s", tmplist->key->name);

            parents = tmplist->parents;
            if (parents != NULL) {
                print("(");
                while ( parents != NULL ) {
                    print("%s", parents->key->name);
                    parents = parents->next;
                    if (parents != NULL) print(", ");
                }
                print(")");
            }
            print(":");
            if (abstract) {
                print("    # Abstract class");
            } else if (interface) {
                print("    # Interface");
            }
            print("\n");
            if (tmplist->key->comment && *(tmplist->key->comment))
                print("    \"\"\"%s\"\"\"\n", tmplist->key->comment); /* include comments, wolf */
            intro_was_printed = 0;
            umla = tmplist->key->attributes;

            /* class scope */
            while (umla != NULL) {
                if (umla->key.isstatic) {
                    if (!intro_was_printed) {
                        print("\n# Attributes: Class\n\n");
                        intro_was_printed = 1;
                    }
                    switch (umla->key.visibility) {
                    case '0':
                        print("    ");
                        break;
                    case '1':
                        print("    __");
                        break;
                    case '2':
                        print("    _");
                        break;
                    }
                    print("%s", umla->key.name);
                    if ( umla->key.value[0] != 0 ) {
                        print(" = %s", umla->key.value);
                    } else {
                        print(" = None");
                    }
                    if (umla->key.type[0] != 0) {
                        print("  # %s", umla->key.type);
                    }
                    print("\n");
                }
                umla = umla->next;
            }
            /* instance attributes */
            intro_was_printed = 0;
            umla = tmplist->key->attributes;
            while ( umla != NULL) {
                if (!umla->key.isstatic) {
                    if (!intro_was_printed) {
                        print("\n# Attributes: Instance\n\n");
                        print("    def __init__(self):\n");
                        intro_was_printed = 1;
                    }
                    switch (umla->key.visibility) {
                    case '0':
                        print("        self.");
                        break;
                    case '1':
                        print("        self.__");
                        break;
                    case '2':
                        print("        self._");
                        break;
                    }
                    print("%s", umla->key.name);
                    if ( umla->key.value[0] != 0 ) {
                        print(" = %s", umla->key.value);
                    } else {
                        print(" = None");
                    }
                    if (umla->key.type[0] != 0) {
                        print("  # %s", umla->key.type);
                    }
                    print("\n");
                }
                umla = umla->next;
            }

            umlo = tmplist->key->operations;
            print("\n# Operations\n\n");
            while (umlo != NULL) {

                switch (umlo->key.attr.visibility) {
                case '0':
                    print("    def %s(self", umlo->key.attr.name);
                    break;
                case '1':
                    print("    def __%s(self", umlo->key.attr.name);
                    break;
                case '2':
                    print("    def _%s(self", umlo->key.attr.name);
                    break;
                }

                tmpa = umlo->key.parameters;
                while (tmpa != NULL) {
                    print(", %s", tmpa->key.name);
                    if ( tmpa->key.value[0] != 0 ) {
                        print(" = %s", tmpa->key.value);
                    }
                    tmpa = tmpa->next;
                }
                print("):\n");
                tmpa = umlo->key.parameters;
                if (tmpa)
                    print("        \n");
                while (tmpa != NULL) {
                    print("        %s: %s\n", tmpa->key.name, tmpa->key.type);
                    tmpa = tmpa->next;
                }
                if (abstract || interface) {
                    print("        raise NotImplementedError()\n    \n");
                } else {
                    print("        return");
                    if (umlo->key.attr.type[0]) 
                        print("    # %s", umlo->key.attr.type);
                    print("\n    \n");
                }
                umlo = umlo->next;
            }
            print("\n");

            fclose(spec);
        }
        tmplist = tmplist->next;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
