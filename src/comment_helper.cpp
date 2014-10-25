/*
Copyright (C) 2004-2014 Leo West

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

#include "dia2code.hpp"


/**
 * generate a comment block for an operation
 *
 */
void generate_operation_comment( FILE *outfile, batch *b, umloperation *ope )
{
    umlattrlist  tmpa;
    d2c_fprintf(outfile, "/**\n");
    d2c_fprintf(outfile, " * Operation %s\n", ope->attr.name );
    if( ope->attr.comment[0] != 0 ) {
        d2c_fprintf(outfile, " * %s\n", ope->attr.comment );
    }
    d2c_fprintf(outfile, " *\n");
     tmpa = ope->parameters;
    while (tmpa != NULL) {
        d2c_fprintf(outfile, " * @param %s - %s\n", tmpa->key.name, tmpa->key.comment );
        tmpa = tmpa->next;
    }
    if(strcmp(ope->attr.type, "void")) {
        d2c_fprintf(outfile, " * @return %s\n", ope->attr.type);
    }
    d2c_fprintf(outfile, " */\n");
}


/**
 * generate a comment block for an UML attribute
 *
 */
void generate_attribute_comment( FILE *outfile, batch *b, umlattribute *attr )
{
    d2c_fprintf(outfile, "/**\n");
    d2c_fprintf(outfile, " * %s\n", attr->comment );
    d2c_fprintf(outfile, " */\n");
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
