/*
Copyright (C) 2000-2014

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

#ifndef SOURCEPARSER_H
#define SOURCEPARSER_H

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>

/*
 defines a section of source code between starting and ending positions
 as for now, two additional informations are attached to each block : an unique oid and a type 
*/
struct sourceblock {
    char * spos; /* starting position - file buffer offset */
    size_t len; /* length in char */
    int type;
    char *oid;
};

typedef struct sourceblock sourceblock;

/* 
 to link block togethers 
*/
struct _sourceblocknode {
    sourceblock *blk;
    struct _sourceblocknode *next;
};

typedef struct _sourceblocknode sourceblocknode;

/*  
  macro structure used for code preservation feature 
  buffer contains the complete source of the file
  blocks is a list of identified blocks (methods implementations for now) in this source
*/
struct _sourcecode
{
    sourceblocknode *blocks;
    char *buffer;
};
typedef struct _sourcecode sourcecode;


void source_preserve( batch *b, umlclass *class_, const char *filename, sourcecode *source );

//sourceblock *block_find( sourceblocknode list, const char *oid );
sourceblock *sourceblock_find( sourceblocknode *list, const char *oid );

sourceblocknode *source_parse( const char *sourcebuffer );

sourceblock *sourceblock_new( char *spos, size_t len, char *oid, int type );

char *source_loadfromfile(const char *filename );

int find_more_str( char *buf, char *tofind[], sourceblock pblocks[] );

sourceblock *find_matching_char( const char *buf, char *oid, char openc, char closec, int maxcars );


#endif
