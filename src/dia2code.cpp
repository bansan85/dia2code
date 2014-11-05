/*
Copyright (C) 2000-2014 Javier O'Hara

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
#include <errno.h>

int generate_backup;


/**
 * This function returns the upper case char* of the one taken on input
 * The char * received may be freed by the caller
*/
std::string strtoupper(std::string s) {
    std::string tmp (s);
    int i, n;
    n = tmp.length ();
    for (i = 0; i < n; i++) {
        tmp[i] = toupper(tmp[i]);
    }
    return tmp;
}

/**
  * This function returns the a char* that has the first
  * character in upper case and the rest unchanged.
  * The char * received may be freed by the caller
*/
std::string strtoupperfirst(std::string s) {
    std::string tmp (s);
    int i;
    if (s.empty ())
        return s;
    tmp[0] = toupper(tmp[0]);
    return tmp;
}


std::list <std::string> parse_class_names (char *s) {
    char *token;
    const char *delim = ",";
    std::list <std::string> list;

    token = strtok (s, delim);
    while ( token != NULL ) {
        list.push_back (token);
        token = strtok (NULL, delim);
    }
    return list;
}


int is_present(std::list <std::string> list, const char *name) {
    for (std::string str : list) {
        const char *namei = str.c_str ();
        int len;
        const char* mask;
        if ( ! strcmp(namei, name) ) {
            return 1;
        }
        len = strlen(namei);
        if (len >= 2 && len <= strlen(name)
                && (mask = strchr(namei, '*')) != NULL
                && mask == strrchr(namei, '*') ) {
            len--;
            if ( mask == namei && ! strcmp(namei+1, name+strlen(name)-len) ) {
                return 1;
            }
            if ( mask == namei+len && ! strncmp(namei, name, len) ) {
                return 1;
            }
        }
    }
    return 0;
}

/*
    Builds a package list from the hierarchy of parents of package.
    The topmost package will be the first on the list and the initial
    package will be the last.
*/
umlpackagelist make_package_list(umlpackage * package){
    umlpackagelist dummylist, tmplist=NULL;

    while ( package != NULL ){
        dummylist = new umlpackagenode;
        dummylist->next = tmplist;
        tmplist = dummylist;
        tmplist->key = package;
        package = package->parent;
    }
    return tmplist;
}

int
is_enum_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "enum") ||
            !strcasecmp (stereo, "enumeration") ||
            !strcmp (stereo, "CORBAEnum"));
}

int
is_struct_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "struct") ||
            !strcasecmp (stereo, "structure") ||
            !strcmp (stereo, "CORBAStruct"));
}

int
is_typedef_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "typedef") ||
            !strcmp (stereo, "CORBATypedef"));
}

int
is_const_stereo (const char *stereo)
{
    return (!strcasecmp(stereo, "const") ||
            !strcasecmp (stereo, "constant") ||
            !strcmp (stereo, "CORBAConstant"));
}

/* 
* find a diaoid token in a string
* the diaoid must be formatted as @diaoid <oid> where oid is a string without space
* @param the NULL terminated string buffer to look in 
* @param (out) a pointer located on the first oid charater - NULL is allowed if you dont need this pointer
* @return the diaoid found or NULL if none is found 
*/
char *find_diaoid( const char *buf, char **newpos  )
{
    const char *oidtag = "@diaoid";
    char *cp, *ep; // current pos, diaoid ending position
    char *oidp=NULL;
    if( buf == NULL ) {
        return NULL;
    }
    cp = const_cast <char *> (strstr( buf, oidtag ));
    if( cp == NULL )
        return NULL;
    cp += strlen(oidtag)+1;
    /* get the oid */
    ep = strpbrk( cp, " \t\n\r" );
    if( ep == NULL ) {
        oidp = strdup(cp);
    } else {
        oidp= (char*) strndup( cp, (size_t) ( ep-cp));
    }
    /* caller want the new position : we set it */
    if( newpos != NULL ) {
        (*newpos) = cp;
    }
    return oidp;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
