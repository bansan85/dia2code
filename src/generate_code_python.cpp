/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2001-2014 Javier O'Hara, Cyrille Chepelov

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


#include "dia2code.hpp"

void generate_code_python(batch *b) {
    umlclasslist tmplist, parents;
    umlattrlist umla, tmpa;
    umloplist umlo;
    char *tmpname;
    char outfilename[BIG_BUFFER];
    FILE * outfile, *dummyfile, *licensefile = NULL;
    namelist used_classes, tmpnamelist;

    int tmpdirlgth, tmpfilelgth;
    int interface, abstract;

    if (b->outdir == NULL) {
        b->outdir = ".";
    }
    tmpdirlgth = strlen(b->outdir);

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

            tmpname = tmplist->key->name;

            /* This prevents buffer overflows */
            tmpfilelgth = strlen(tmpname);
            if (tmpfilelgth + tmpdirlgth > sizeof(*outfilename) - 2) {
                fprintf(stderr, "Sorry, name of file too long ...\nTry a smaller dir name\n");
                exit(4);
            }

            sprintf(outfilename, "%s/%s.py", b->outdir, tmplist->key->name);
            dummyfile = fopen(outfilename, "r");
            if ( b->clobber || ! dummyfile ) {

                outfile = fopen(outfilename, "w");
                if ( outfile == NULL ) {
                    fprintf(stderr, "Can't open file %s for writing\n", outfilename);
                    exit(3);
                }

                /* add license to the header */
                if (b->license != NULL) {
                    int lc;
                    rewind(licensefile);
                    while ((lc = fgetc(licensefile)) != EOF) {
                        fprintf(outfile,"%c", (char) lc);
                    }
                }

                used_classes = find_classes(tmplist, b);
                tmpnamelist = used_classes;
                while (tmpnamelist != NULL) {
                    fprintf(outfile, "from %s import %s\n",
                            tmpnamelist->name, tmpnamelist->name);
                    tmpnamelist = tmpnamelist->next;
                }

                fprintf(outfile, "\n");

                tmpname = strtolower(tmplist->key->stereotype);
                interface = eq("interface", tmpname);
                abstract = tmplist->key->isabstract;
                free(tmpname);

                fprintf(outfile, "class %s", tmplist->key->name);

                parents = tmplist->parents;
                if (parents != NULL) {
                    fprintf(outfile, "(");
                    while ( parents != NULL ) {
                        fprintf(outfile, "%s", parents->key->name);
                        parents = parents->next;
                        if (parents != NULL) fprintf(outfile, ", ");
                    }
                    fprintf(outfile, ")");
                }
                fprintf(outfile, ":\n");
                if (abstract) {
                    fprintf(outfile, "    \"\"\"Abstract class %s\n    \"\"\"\n",
                            tmplist->key->name);
                } else if (interface) {
                    fprintf(outfile, "    \"\"\"Interface %s\n    \"\"\"\n",
                            tmplist->key->name);
                } else {
                    fprintf(outfile, "    \"\"\"Class %s\n    \"\"\"\n",
                            tmplist->key->name);
                }

                fprintf(outfile, "    # Attributes:\n");
                umla = tmplist->key->attributes;
                while ( umla != NULL) {

                    switch (umla->key.visibility) {
                    case '0':
                        fprintf (outfile, "    ");
                        break;
                    case '1':
                        fprintf (outfile, "    __");
                        break;
                    case '2':
                        fprintf (outfile, "    _");
                        break;
                    }

                    fprintf(outfile, "%s", umla->key.name);
                    if ( umla->key.value[0] != 0 ) {
                        fprintf(outfile, " = %s", umla->key.value);
                    } else {
                        fprintf(outfile, " = None");
                    }
                    fprintf(outfile, "  # (%s) \n", umla->key.type);
                    umla = umla->next;
                }

                fprintf(outfile, "    \n");
                umlo = tmplist->key->operations;
                fprintf(outfile, "    # Operations\n");
                while ( umlo != NULL) {

                    switch (umlo->key.attr.visibility) {
                    case '0':
                        fprintf (outfile, "    def %s(self", umlo->key.attr.name);
                        break;
                    case '1':
                        fprintf (outfile, "    def __%s(self", umlo->key.attr.name);
                        break;
                    case '2':
                        fprintf (outfile, "    def _%s(self", umlo->key.attr.name);
                        break;
                    }

                    tmpa = umlo->key.parameters;
                    while (tmpa != NULL) {
                        fprintf(outfile, ", %s", tmpa->key.name);
                        if ( tmpa->key.value[0] != 0 ) {
                            fprintf(outfile, " = %s", tmpa->key.value);
                        }
                        tmpa = tmpa->next;
                    }
                    fprintf(outfile, "):\n");
                    fprintf(outfile, "        \"\"\"function %s\n", umlo->key.attr.name);
                    tmpa = umlo->key.parameters;
                    if (tmpa) fprintf(outfile, "        \n");
                    while (tmpa != NULL) {
                        fprintf(outfile, "        %s: %s\n", tmpa->key.name, tmpa->key.type);
                        tmpa = tmpa->next;
                    }
                    fprintf(outfile, "        \n");
                    fprintf(outfile, "        returns %s\n", umlo->key.attr.type);
                    fprintf(outfile, "        \"\"\"\n");

                    if (abstract || interface) {
                        fprintf(outfile, "        raise NotImplementedError()\n    \n");
                    } else {
                        fprintf(outfile, "        return None # should raise NotImplementedError()\n    \n");
                    }
                    umlo = umlo->next;
                }
                fprintf(outfile, "\n");

                fclose(outfile);
            }
        }
        tmplist = tmplist->next;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
