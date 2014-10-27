/*
Copyright (C) 2014-2014

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

#include "config.h"

#include "DiaGram.hpp"

DiaGram::DiaGram () :
    uml (),
    genClasses (),
    license (),
    outdir ("."),
    overwrite (true),
    buildtree (false),
    invertsel (false),
    usecorba (false),
    tmp_classes (),
    includes () {
}


umlclasslist
DiaGram::getUml () {
    return uml;
}


void
DiaGram::setUml (umlclasslist diagram) {
    uml = diagram;

    return;
}


void
DiaGram::addGenClasses (std::list <std::string> classes) {
    genClasses.merge (classes);

    return;
}


bool
DiaGram::genGenClasses (char * class_) {
    if (genClasses.empty ())
        return true;

    for (std::string nom : genClasses) {
        if (nom.compare (class_) == 0) {
            return true ^ invertsel;
        }
    }

    return false ^ invertsel;
}


std::list <std::string>
DiaGram::getGenClasses () {
    return genClasses;
}


char *
DiaGram::getLicense () {
    return license.c_str ();
}


void
DiaGram::setLicense (char * lic) {
    license.assign (lic);

    return;
}


char *
DiaGram::getOutdir () {
    return outdir.c_str ();
}


const std::string *
DiaGram::getOutdirS () {
    return &outdir;
}


void
DiaGram::setOutdir (char * dir) {
    outdir.assign (dir);

    return;
}


bool
DiaGram::getOverwrite () {
    return overwrite;
}


void
DiaGram::setOverwrite (bool over) {
    overwrite = over;

    return;
}


bool
DiaGram::getBuildTree () {
    return buildtree;
}


void
DiaGram::setBuildTree (bool build) {
    buildtree = build;

    return;
}


bool
DiaGram::getInvertSel () {
    return invertsel;
}


void
DiaGram::setInvertSel (bool invert) {
    invertsel = invert;

    return;
}


bool
DiaGram::getUseCorba () {
    return usecorba;
}


void
DiaGram::setUseCorba (bool corba) {
    usecorba = corba;

    return;
}


/* Returns a list with all the classes declared in the diagram */
std::list <std::string>
DiaGram::scan_tree_classes () {
    std::list <std::string> result;
    umlclasslist tmplist = uml;
    
    while ( tmplist != NULL ) {
        result.push_back (tmplist->key->name);
        tmplist = tmplist->next;
    }
    
    return result;
}


/* Returns a list with all the classes declared in the diagram that current
 * class references */
std::list <std::string>
DiaGram::find_classes (umlclasslist current_class) {
    umlclasslist parents, dependencies;
    umlassoclist associations;
    umlattrlist umla, tmpa;
    umloplist umlo;
    std::list <std::string> result;
    std::list <std::string> classes = scan_tree_classes();

    umla = current_class->key->attributes;
    while ( umla != NULL) {
        if (is_present(classes, umla->key.type)
                && !is_present(result, umla->key.type)) {
            result.push_back (umla->key.type);
        }
        umla = umla->next;
    }

    umlo = current_class->key->operations;
    while ( umlo != NULL) {
        if (is_present(classes, umlo->key.attr.type)
                && !is_present(result, umlo->key.attr.type)) {
            result.push_back (umlo->key.attr.type);
        }
        tmpa = umlo->key.parameters;
        while (tmpa != NULL) {
            if (is_present(classes, tmpa->key.type)
                    && !is_present(result, tmpa->key.type)) {
                result.push_back (tmpa->key.type);
            }
            tmpa = tmpa->next;
        }

        umlo = umlo->next;
    }

    parents = current_class->parents;
    while ( parents != NULL ) {
        if ( is_present(classes, parents->key->name)
                && ! is_present(result, parents->key->name) ) {
            result.push_back (parents->key->name);
        }
        parents = parents->next;
    }

    dependencies = current_class->dependencies;
    while (dependencies != NULL) {
        if ( is_present(classes, dependencies->key->name)
                && ! is_present(result, dependencies->key->name) ) {
            result.push_back (dependencies->key->name);
        }
        dependencies = dependencies->next;
    }

    associations = current_class->associations;
    while (associations != NULL) {
        if ( is_present(classes, associations->key->name)
                && ! is_present(result, associations->key->name ) ) {
            result.push_back (associations->key->name);
        }
        associations = associations->next;
    }

    return result;
}

/* Creates a new umlclassnode with class as the key, then appends it to
  the end of list */
umlclasslist append ( umlclasslist list, umlclassnode * class_ ) {
    umlclasslist tmplist = list;
    umlclassnode *tmpnode = NULL;

    tmpnode = (umlclassnode*) my_malloc ( sizeof(umlclassnode) );
    tmpnode->key = class_->key;
    tmpnode->parents = class_->parents;
    tmpnode->associations = class_->associations;
    tmpnode->dependencies = class_->dependencies;
    tmpnode->next = NULL;

    if ( tmplist != NULL ) {
        while ( tmplist->next != NULL ) {
            tmplist = tmplist->next;
        }
        tmplist->next = tmpnode;
        return list;
    } else {
        return tmpnode;
    }
}

/* Returns a freshly constructed list of the classes that are used
   by the given class AND are themselves in the classlist of the
   given batch */
umlclasslist
DiaGram::list_classes(umlclasslist current_class) {
    umlclasslist parents, dependencies;
    umlassoclist associations;
    umlattrlist umla, tmpa;
    umloplist umlo;
    umlclasslist result = NULL;
    umlclasslist classes = uml;
    umlclasslist tmpnode = NULL;

    umla = current_class->key->attributes;
    while ( umla != NULL) {
        if ( strlen(umla->key.type) > 0 ) {
            tmpnode = find_by_name(classes, umla->key.type);
            if ( tmpnode && ! find_by_name(result, umla->key.type)) {
                result = append(result, tmpnode);
            }
        }
        umla = umla->next;
    }

    umlo = current_class->key->operations;
    while ( umlo != NULL) {
        tmpnode = find_by_name(classes, umlo->key.attr.type);
        if ( tmpnode && ! find_by_name(result, umlo->key.attr.type)) {
            result = append(result, tmpnode);
        }
        tmpa = umlo->key.parameters;
        while (tmpa != NULL) {
            tmpnode = find_by_name(classes, tmpa->key.type);
            if ( tmpnode && ! find_by_name(result, tmpa->key.type)) {
                result = append(result, tmpnode);
            }
            tmpa = tmpa->next;
        }
        umlo = umlo->next;
    }

    parents = current_class->parents;
    while ( parents != NULL ) {
        tmpnode = find_by_name(classes, parents->key->name);
        if ( tmpnode && ! find_by_name(result, parents->key->name) ) {
            result = append(result, tmpnode);
        }
        parents = parents->next;
    }

    dependencies = current_class->dependencies;
    while (dependencies != NULL) {
        tmpnode = find_by_name(classes, dependencies->key->name);
        if ( tmpnode && ! find_by_name(result, dependencies->key->name) ) {
            result = append(result, tmpnode);
        }
        dependencies = dependencies->next;
    }

    associations = current_class->associations;
    while (associations != NULL) {
        tmpnode = find_by_name(classes, associations->key->name);
        if ( tmpnode && ! find_by_name(result, associations->key->name ) ) {
            result = append(result, tmpnode);
        }
        associations = associations->next;
    }

    return result;

}

/**
 * create a directory hierarchy for the package name 
 * batch.outdir is taken as root directory 
 * works with java-like package naming convention 
 * the directory path is stored in pkg->directory
 * eg. org.foo.bar will create directory tree org/foo/bar
 * @param the current batch
 * @param the package pointer
 * @return the full directory path eg. "<outdir>/org/foo/bar"
 * 
 */
char *
DiaGram::create_package_dir(umlpackage *pkg )
{
    char *fulldirname, *dirname, fulldirnamedup[BIG_BUFFER];
    /* created directories permissions */
    mode_t dir_mask = S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP | S_IXGRP;
    if (pkg == NULL) {
        return NULL;
    }
    if (buildtree == 0 || pkg->name == NULL) {
        pkg->directory = outdir.c_str ();
    } else {
        fulldirname = (char*)my_malloc(BIG_BUFFER);
        sprintf(fulldirname, "%s", outdir.c_str ());
        dirname = strdup(pkg->name);
        dirname = strtok( dirname, "." );
        while (dirname != NULL) {
            sprintf( fulldirnamedup, "%s/%s", fulldirname, dirname );
            sprintf( fulldirname, "%s", fulldirnamedup );
            /* TODO : should create only if not existent */
            mkdir( fulldirname, dir_mask );
            dirname = strtok( NULL, "." );
        }
        /* set the package directory used later for source file creation */
        pkg->directory = fulldirname;
    }
    return pkg->directory;
}

void
DiaGram::push (umlclassnode *node)
{
    umlclasslist used_classes, tmpnode;
    module *m;
    declaration *d;

    if (node == NULL || find_class (node) != NULL) {
        return;
    }

    tmp_classes.push_back (node->key->name);
    
    used_classes = list_classes (node);
    /* Make sure all classes that this one depends on are already pushed. */
    tmpnode = used_classes;
    while (tmpnode != NULL) {
        /* don't push this class !*/
        if (! !strcmp (node->key->name, tmpnode->key->name) &&
            ! (is_present (tmp_classes, tmpnode->key->name) ^ invertsel)) {
            push (tmpnode);
        }
        tmpnode = tmpnode->next;
    }

    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        m = find_or_add_module (&decls, pkglist);
        if (m->contents == NULL) {
            m->contents = NEW (declaration);
            d = m->contents;
            d->prev = NULL;
        } else {
            /* We can simply append because all classes that we depend on
               are already pushed. */
            d = append_decl (m->contents);
        }
    } else {
        if (decls == NULL) {
            decls = NEW (declaration);
            d = decls;
            d->prev = NULL;
        } else {
            d = append_decl (decls);
            /* We can simply append because all classes that we depend on
               are already pushed. */
        }
    }
    d->decl_kind = dk_class;
    d->next = NULL;
    d->u.this_class = NEW (umlclassnode);
    memcpy (d->u.this_class, node, sizeof(umlclassnode));
    if (strncmp (node->key->stereotype, "CORBA", 5) == 0)
        usecorba = true;
}


int
DiaGram::have_include (char *name)
{
    for (std::string inc : includes) {
        if (!inc.compare (name)) {
            return 1;
        }
    }
    return 0;
}

void
DiaGram::add_include (char *name)
{
    if (have_include (name))
        return;
    
    includes.push_back (name);
}

void
DiaGram::push_include (umlclassnode *node)
{
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        add_include (pkglist->key->name);
    } else {
        add_include (node->key->name);
    }
}

std::list <std::string>
DiaGram::getIncludes () {
    return includes;
}


void
DiaGram::cleanIncludes () {
    includes.clear ();
}

void
DiaGram::determine_includes (declaration *d)
{
    if (d->decl_kind == dk_module) {
        declaration *inner = d->u.this_module->contents;
        while (inner != NULL) {
            determine_includes (inner);
            inner = inner->next;
        }
    } else {
        umlclasslist cl = list_classes (d->u.this_class);
        while (cl != NULL) {
            push_include (cl);
            cl = cl->next;
        }
    }
}

/*
 * test a file existence and extract the source and the blocks
 * source is allocated and initialised with the source code buffer and the blocks markers
 */
void
DiaGram::source_preserve(umlclass *class_, const char *filename, sourcecode *source )
{
    char *diaoid = NULL;
    umloplist umlo;
    sourceblock *srcblock = NULL;
    debug( 4, "preserve_source(filename=%s)", filename);
    source = (sourcecode*) my_malloc( sizeof(sourcecode));
    source->buffer = NULL;
    source->blocks = NULL;
    
    //open the file in read only 
    FILE * rofile = fopen(filename, "r");
    if( ! rofile ) {
        debug( DBG_SOURCE, "no existing file %s for class %s", filename, class_->name );
        return;
    }
    /* from here, the file exists, we transfer the content in a buffer and parse the source */
    source->buffer = source_loadfromfile( filename );
    if( source->buffer == NULL ) {
        debug( 4, "warning: NULL sourcebuffer from file" );
    } else {
        source->blocks  = source_parse( source->buffer );
        /* copy source blocks found to method->implementation */
        umlo = class_->operations;
        while( umlo != NULL ) {
            /* is there a diaoid hidden in the operation comment ? */
            if( (diaoid=find_diaoid(umlo->key.attr.comment,NULL)) != NULL) {
                debug( DBG_SOURCE,"diaoid:%s found in comment for method %s", diaoid, umlo->key.attr.name );
                /* now try to find the implementation block in the sourcebuffer */
                srcblock = sourceblock_find( source->blocks, diaoid );
                // srcblock->spos poitns the implementation of lengtjh srcblock->len
                if( srcblock != NULL ) {
                    umlo->key.implementation = (char*) strndup( srcblock->spos, srcblock->len );
                }
            } else {
                debug( DBG_SOURCE, "diaoid %s not found in source", diaoid );
            }
            umlo = umlo->next;
        } // while
    }
    fclose(rofile);
}

/**
 * generate a comment block for an operation
 *
 */
void
DiaGram::generate_operation_comment( FILE *outfile, umloperation *ope )
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
void
DiaGram::generate_attribute_comment( FILE *outfile, umlattribute *attr )
{
    d2c_fprintf(outfile, "/**\n");
    d2c_fprintf(outfile, " * %s\n", attr->comment );
    d2c_fprintf(outfile, " */\n");
}


DiaGram::~DiaGram () {
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
