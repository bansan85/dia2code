/***************************************************************************
                          generate_code_sql.c  -  description
                             -------------------
    begin                : Sat Jun 2 2001
    email                : sirnewton_01@yahoo.ca
    contrib              : 2012-2021
    email                : alejandro.imass@gmail.com
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
#include "parse_diagram.h"
#include "code_generators.h"

void inherit_attributes(umlclasslist parents, umlattrlist umla) {
    umlattrlist pumla;

    while (parents != NULL) {
        pumla = parents->key->attributes;
        insert_attribute(copy_attributes(pumla), umla);
        parents = parents->next;
    }
}

void generate_code_sql(batch *b) {
    umlclasslist tablelist;
    umlattrlist umla;
    char *tmpname;
    char outfilename[BIG_BUFFER];
    unsigned int counter = 0;
    FILE * outfilesql, *dummyfile;

    int tmpdirlgth, tmpfilelgth;

    if (b->outdir == NULL) {
        b->outdir = ".";
    }

    tmpdirlgth = strlen(b->outdir);

    tablelist = b->classlist;

    if (tablelist == NULL) {
        fprintf(stderr, "Sorry, no class found in your file.\n");
        exit(4);
    }

    sprintf(outfilename, "%s/DEFINITION.SQL", b->outdir);
    dummyfile = fopen(outfilename, "r");
    if ( b->clobber || ! dummyfile ) {
        outfilesql = fopen(outfilename, "w"); /* Moved this one to be able to OVERWRITE old file, changed from "a" to "w"... */
        if ( outfilesql == NULL ) {
            fprintf(stderr, "Can't open file %s for writing\n", outfilename);
            exit(3);
        }

        /* This prevents buffer overflows */
        if (tmpdirlgth > sizeof(*outfilename) - 2) {
            fprintf(stderr, "Sorry, name of file too long ...\nTry a smaller dir name\n");
            exit(4);
        }
    }

    while ( tablelist != NULL ) {

        if ( ! ( is_present(b->classes, tablelist->key->name) ^ b->mask ) ) {
            char seenFirst = 0;

            if (tablelist->key->isabstract) {
                tablelist = tablelist->next;
                continue;
            }

            /* Class (table) */
            fprintf(outfilesql, "CREATE TABLE %s(\n", tablelist->key->name);

            /* Attributes (columns) */
            fprintf(outfilesql, "-- Attributes --\n");
            umla = tablelist->key->attributes;
            inherit_attributes (tablelist->parents, umla);
            while ( umla != NULL) {
                fprintf(outfilesql, "  %s %s", umla->key.name, umla->key.type);
                if (umla->next != NULL) {
                    fprintf(outfilesql, ",\n");
                }
                umla = umla->next;
            }

            /* IsStatic attribute (Primary Key) */
            umla = tablelist->key->attributes;
            while ( umla != NULL) {
                if( umla->key.isstatic ) {
                    if( !seenFirst ) {
                            seenFirst = 1;
                            fprintf(outfilesql, ",\n  PRIMARY KEY (\n");
                    }
                    fprintf(outfilesql, "    %s", umla->key.name);
                    if (umla->next != NULL && umla->next->key.isstatic) {
                        fprintf(outfilesql, ",\n");
                    }
                }
                umla = umla->next;
            }
            if (seenFirst) fprintf(outfilesql, ")\n");

            fprintf(outfilesql, ");\n\n");
        }

        tablelist = tablelist->next;
    }

    /*
        Adding associations LAST since we want to have all tables around FIRST
        Rules and Caveats:
            Side a is always the parent table, side b is always the child table
            Association direction is ignored so regardless of the direction it always assumes A to B
        The FK is generated from side b to side a like this:
            ALTER TABLE [side b] ADD
            CONSTRAINT  FK_[side b]_[side a]  FOREIGN KEY([assoc name]) REFERENCES [side a] ([field]);
            field = "id" if assoc_name ends in _id else assoc_name
    */
    tablelist = b->classlist;
    while( tablelist != NULL ) {
        umlassocnode* assoc = tablelist->associations;
        // a class may have multiple assocs (FKs)
        counter = 1;
        while( assoc != NULL ) {
            /*
              AI: use "surrogate key" trailing _id convention of most popular ORMs
              http://en.wikipedia.org/wiki/Surrogate_key
            */
            char tail[4];
            const char *fk_col =  assoc->name;
            strncpy(tail, fk_col + strlen(fk_col) - 3, 3);
            if (!strcmp("_id", tail)) {
                fk_col = "id";
            } else {
                printf("warning: association name %s does not end in _id but rather %s,\n",
                       assoc->name, tail);
                printf("make sure FK cols have the same name on both tables\n");
                fk_col = assoc->name;
            }
            // assumes the default A to B direction (ignores direction setting in drawing)
            fprintf( outfilesql, "\n\nALTER TABLE %s ADD\n", assoc->key->name ); //side b, child
            fprintf( outfilesql, "    CONSTRAINT  FK_%s_%s_%i  FOREIGN KEY(%s) REFERENCES %s (%s);\n",
                     assoc->key->name, //side b, child
                     tablelist->key->name, //side a, parent
                     counter,
                     assoc->name,
                     tablelist->key->name, // side a, parent
                     fk_col ); // parent field, depends on surrogate model or not
            /* AI: add indexes to FOREIGN KEY */
            if(is_present(b->sqlopts, "fkidx")){
                /* AI: many-to-many connectors have fk that are also pk... */
                int pk = 0;
                umla = assoc->key->attributes;
                while ( umla != NULL) {
                    if( umla->key.isstatic && !strcmp(umla->key.name,assoc->name)) {
                        pk = 1;
                    }
                    umla = umla->next;
                }
                /* ... and these already have indexes */
                if(!pk){
                fprintf( outfilesql, "\n\nCREATE INDEX  FKIDX_%s_%s_%i  ON %s (%s);\n",
                    assoc->key->name,
                    tablelist->key->name,
                    counter,
                    assoc->key->name,
                    assoc->name );
                }
            }
            assoc = assoc->next;
            counter++;
        }
        tablelist = tablelist->next;
    }
    fprintf(stderr, "Finished!\n");
    fclose(outfilesql);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
