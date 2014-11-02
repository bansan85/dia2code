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

char * d2c_indentstring = "   ";
int d2c_indentposition = 0;
int generate_backup;

static int DBG_LEVEL = 4;

void debug_setlevel( int newlevel )
{
    DBG_LEVEL = newlevel;
}

std::string
format (const std::string fmt,
        va_list marker)
{
  size_t      size = 1024;
  bool        b = false;
  char *ttt;
  std::string s;
  
  while (!b)
  {
    int32_t n;
    
    ttt = malloc (sizeof (char)*size);
//    s.resize (size);
    n = vsnprintf (ttt,
                   size,
                   fmt.c_str (),
                   marker);
    if ((b = (static_cast <size_t> (n) < size)) == true)
    {
    }
    else
    {
      size = size * 2;
    }
  }
  s.assign (ttt);
  free (ttt);
  return s;
}


/* 
 * a dummy logger / debugger function
 */
void debug( int level, char *fmt, ... )
{
    static std::string debug_buffer;
    va_list argptr;
    if( level != DBG_LEVEL ) 
        return;
    va_start(argptr, fmt);
    debug_buffer = format(fmt, argptr);
    va_end(argptr);
    fprintf( stderr, "DBG %d: %s\n", level, debug_buffer.c_str ());
    fflush( stderr);
}

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
  * This function returns the lower case char* of the one taken on input
  * The char * received may be freed by the caller
*/
char *strtolower(char *s) {
    char *tmp = strdup(s);
    int i, n;
    if (tmp == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    n = strlen(tmp);
    for (i = 0; i < n; i++) {
        tmp[i] = tolower(tmp[i]);
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
    int i, n;
    n = tmp.length ();
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
        char* mask;
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

umlattrlist copy_attributes(umlattrlist src)
{
    umlattrlist cpy = NULL, start = NULL;

    while (src != NULL)
    {
        umlattrlist tmp = new umlattrnode;
        tmp->key = src->key;
        if (cpy == NULL) {
            cpy = tmp;
            start = tmp;
        } else {
            cpy->next = tmp;
            cpy = tmp;
        }
        src = src->next;
    }
    if (cpy != NULL)
        cpy->next = NULL;

    return start;
}


FILE *body = NULL;

void ebody (char *msg, ...)
{
    var_arg_to_str (msg);
    if (body != NULL)
        fputs (str.c_str (), body);
}

char *body_file_ext = NULL;

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

/* Added by RK 2003-02-20
   This should become part of the uml_class object. */

struct endless_string_buf
{
    char *buf;
    struct endless_string_buf *next;
};
typedef struct endless_string_buf endless_string_buf;

struct endless_string
{
    endless_string_buf *start;
    endless_string_buf *end;
};
typedef struct endless_string endless_string;

struct d2c_impl{
    std::string name;
    endless_string *impl;
    int impl_len;
    int in_source;
    int in_class;
    struct d2c_impl *next;
};

typedef struct d2c_impl d2c_impl;
d2c_impl *d2c_impl_list = NULL;

/* Todo on auto-indentation:
   1. Define meta-characters that are converted to braces
*/
int indent_count = 4;
int indent_open_brace_on_newline = 1;


typedef struct
{
    FILE *f;
    int indentation;
} D2C_INDENT_STRUCT;

D2C_INDENT_STRUCT d2c_files[32];
int d2c_num_files = 0;

int d2c_indent_offset(FILE *f)
{
    int i;
    for (i = 0; i < d2c_num_files; i++)
    {
        if (d2c_files[i].f == f)
        {
            return i;
        }
    }
    d2c_files[i].f = f;
    d2c_files[i].indentation = 0;
    d2c_num_files++;
    return i;
}

void d2c_indent(FILE *f)
{
    d2c_files[d2c_indent_offset(f)].indentation++;
}

void d2c_outdent(FILE *f)
{
    d2c_files[d2c_indent_offset(f)].indentation--;
}

int d2c_fprint_indent(FILE *f)
{
    int i;
    int indentation;

    indentation = d2c_files[d2c_indent_offset(f)].indentation;

    for (i = 0; i < indentation * indent_count; i++)
        fputc(' ', f);
    return i;
}

char d2c_io_lchar = 0;

int _d2c_fputc(int c, FILE *f)
{
    int indent_cnt = 0;
    int rc;

    if (d2c_io_lchar == '\n' && c != '\n')
        indent_cnt = d2c_fprint_indent(f);
    d2c_io_lchar = c;
    rc = fputc(c, f);
    if (rc == EOF)
        return rc;
    else
        return indent_cnt + 1;
}

int _d2c_fputs(const char *s, FILE *f)
{
    const char *buf = s;
    /* int len = 0; */
    while (*buf != '\0')
    {
        if (fputc(*buf, f) == EOF)
            return EOF;
        /* len++; */
        buf++;
    }
    return 1;
}

std::string d2c_fprintf_buf;

int _d2c_fprintf(FILE *f, char *fmt, ...)
{
    va_list argptr;
    int cnt;
    int extern_cnt;
    int i;

    va_start(argptr, fmt);
    d2c_fprintf_buf = format (fmt, argptr);
    va_end(argptr);

    extern_cnt = cnt;
    if (cnt != EOF)
    {
        for (i = 0; i < cnt; i++)
        {
            extern_cnt += _d2c_fputc(d2c_fprintf_buf[i], f);
        }
    }
    return extern_cnt;
}

void d2c_indentate( FILE *f)
{
    int i;
    for ( i = 0; i < d2c_indentposition; i++) {
        fputs( d2c_indentstring, f);
    }
}


int d2c_directprintf(FILE *f, char *fmt, ...)
{
    va_list argptr;

    d2c_indentate(f);
    va_start(argptr, fmt);
    vfprintf(f, fmt, argptr);
    va_end(argptr);
    return 0;
}

void d2c_open_brace(FILE *f, char *suffix)
{
    if (indent_open_brace_on_newline)
    {
        d2c_fputc('\n', f);
    }
    else
    {
        d2c_fputc(' ', f);
    }
    d2c_fprintf(f, "{%s\n", suffix);
    d2c_indent(f);
}

void d2c_close_brace(FILE *f, char *suffix)
{
    d2c_outdent(f);
    d2c_fprintf(f, "}%s\n", suffix);
}

param_list *d2c_parameters = NULL;

void param_list_destroy()
{
    param_list *p = d2c_parameters;
    while (p != NULL)
    {
        d2c_parameters = p;
        p = p->next;
        delete d2c_parameters;
    }
    d2c_parameters = NULL;
}

param_list * d2c_parameter_add(char *name, char *value)
{
    param_list *entry = new param_list;
    if (name != NULL)
        entry->name.assign (name);
    if (value != NULL)
        entry->value.assign (value);
    entry->next = d2c_parameters;
    d2c_parameters = entry;

    return entry;
}

param_list * d2c_parameter_set(char *name, char *value)
{
    param_list *entry = d2c_parameter_find(name);
    if (entry == NULL)
        entry=d2c_parameter_add(name, value);
    else
    {
        entry->value.assign (value);
    }

    return entry;
}

param_list *d2c_parameter_find(char *name)
{
    param_list *p = d2c_parameters;
    while (p != NULL)
    {
        if (p->name.compare (name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}


/*
 * increment the tab position 
 * tab position is used in d2c_fprintf and alike
 */
void d2c_shift_code()
{
    d2c_indentposition ++;   
}


/*
 * increment the tab position 
 * tab position is used in d2c_fprintf and alike
 */
void d2c_unshift_code()
{
    if( d2c_indentposition > 0 )
        d2c_indentposition --;
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
    debug( DBG_CORE, "find_diaoid()" );
    if( buf == NULL ) {
        return NULL;
    }
    cp = strstr( buf, oidtag );
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
