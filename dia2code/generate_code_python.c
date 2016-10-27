/***************************************************************************
          generate_code_python.c  -  Function that generates Python code
                             -------------------
    begin                : Sat Mar 24 2001
    copyright            : (C) 2001 by Cyrille Chepelov (based on code from
                                                               Javier O'Hara)
    email                : chepelov@calixo.net

    patched by Wolf Ó Spealáin to recognise class and instance variables
    and tidy Python output. 9th July 2016;
    and updated 23rd October 2016 to fix parameter listings,
    and include diagram comments as correctly formatted docstrings.

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
                if(b->license != NULL){
                    char lc;
                    rewind(licensefile);
                    while((lc = fgetc(licensefile)) != EOF){
                        fprintf(outfile,"%c",lc);
                    }
                }
                
                used_classes = find_classes(tmplist, b);
                tmpnamelist = used_classes;
                while (tmpnamelist != NULL) {
                    fprintf(outfile, "from %s import %s\n",
                            tmpnamelist->name, tmpnamelist->name);
                    tmpnamelist = tmpnamelist->next;
                }
                if (used_classes != NULL) {
                    fprintf(outfile, "\n");
                }
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
                    fprintf(outfile, ")\n");
                }
                fprintf(outfile, ":\n    \"\"\"%s\"\"\"\n", tmplist->key->comment); /* include comments, wolf */

                fprintf(outfile, "\n# Attributes: Class\n");
                umla = tmplist->key->attributes;

                /* __init__ patch by Wolf */
                /* class scope */
                while ( umla != NULL) {
                    if (umla->key.isstatic){
                        switch (umla->key.visibility) {
                            case '0':
                                fprintf (outfile, "\n    ");
                            break;
                            case '1':
                                fprintf (outfile, "\n    __");
                                break;
                            case '2':
                                fprintf (outfile, "\n    _");
                            break;
                        }
                        fprintf(outfile, "%s", umla->key.name);
                        if ( umla->key.value[0] != 0 ) {
                            fprintf(outfile, " = %s", umla->key.value);
                        } else {
                            fprintf(outfile, " = None");
                        }
                        if (umla->key.type[0]!=0){
                            fprintf(outfile, "  # %s", umla->key.type);
                        }
                    }
                    umla = umla->next;
                }
                /* instance attributes */
                umla = tmplist->key->attributes;
                fprintf(outfile, "\n\n# Attributes: Instance\n\n");
                fprintf(outfile, "    def __init__(self");
                while (umla != NULL) {
                    if (!umla->key.isstatic) {
                        fprintf(outfile, ", %s", umla->key.name);
                        if (umla->key.value[0] != 0) {
                            fprintf(outfile, "=%s", umla->key.value);
                        }
                    }
                    umla = umla->next;
                }
                fprintf(outfile, "):");
                umla = tmplist->key->attributes;
                while ( umla != NULL) {
                    if (!umla->key.isstatic){
                        switch (umla->key.visibility) {
                        case '0':
                            fprintf (outfile, "\n        self.");
                            break;
                        case '1':
                            fprintf (outfile, "\n        self.__");
                            break;
                        case '2':
                            fprintf (outfile, "\n        self._");
                            break;
                        }
                        fprintf(outfile, "%s = %s", umla->key.name, umla->key.name);
                        if (umla->key.type[0]!=0) {
                            fprintf(outfile, "  # %s", umla->key.type);
                        }
                    }
                    umla = umla->next;
                }
                /* patch end */

                umlo = tmplist->key->operations;
                fprintf(outfile, "\n\n# Operations\n\n");
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
                    while (tmpa) {
                        if (tmpa->key.name[0] != 0) { /* skip empty paramters, wolf */
                            fprintf(outfile, ", %s", tmpa->key.name);
                        }
                        if (tmpa->key.value[0] != 0) {
                            fprintf(outfile, "=%s", tmpa->key.value); /* no spaces around = */
                        }
                        tmpa = tmpa->next;
                    }
                    fprintf(outfile, "):\n");
                    fprintf(outfile, "        \"\"\"");
                    if (umlo->key.attr.comment[0] != 0) {
                        fprintf(outfile, "%s\n", umlo->key.attr.comment);  /* include comments, wolf */
                    }
                    tmpa = umlo->key.parameters;
                    while (tmpa) {
                        if ( tmpa->key.name[0] != 0 ) {  /* skip empty paramters, wolf */
                            fprintf(outfile, "\n        %s: %s", tmpa->key.name, tmpa->key.type);
                        }
                        tmpa = tmpa->next;
                    }
                    fprintf(outfile, "\n        returns %s\"\"\"\n", umlo->key.attr.type);

                    if (abstract || interface) {
                        fprintf(outfile, "        raise NotImplementedError()\n    \n");
                    } else {
                        fprintf(outfile, "\n        return None\n\n");
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
