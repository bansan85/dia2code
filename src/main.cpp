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
#include "parse_diagram.hpp"
#include "GenerateCodeCpp.hpp"

#include "DiaGram.hpp"

int process_initialization_file(char *filename, int exit_if_not_found);

#ifdef DSO
static void *
find_dia2code_module(const char *lang) {
    char *homedir;
    char *modulepath;
    char *modulename;
    /*  char *generatorname;*/
    void *handle;
    void (*generator)();

    homedir = getenv("HOME");
    if ( homedir )
        homedir = strdup(homedir);
    else
        homedir = strdup(".");

    modulename = (char*)malloc(strlen(DSO_PREFIX) + strlen(lang) + 1);
    sprintf(modulename, "%s%s", DSO_PREFIX, lang);

    modulepath = (char*)malloc(strlen(homedir) + strlen(modulename)
                               + strlen(MODULE_DIR) + strlen(DSO_SUFFIX) + 3);
    sprintf(modulepath, "%s/%s/%s%s", homedir, MODULE_DIR, modulename, DSO_SUFFIX);

    handle = dlopen(modulepath, RTLD_NOW | RTLD_GLOBAL);
    if ( !handle ) {
        fprintf(stderr, "can't find the module: %s\n", dlerror());
        exit(2);
    }
    printf("module name : %s\n", modulename);
    generator = dlsym(handle, modulename);

    free(modulepath);
    free(modulename);
    free(homedir);

    return generator;
}
#endif /* DSO */

int bOpenBraceOnNewline = 1; /* This should also be a command-line parameter */

enum ParseType {
    PARSE_TYPE_FUNCTION = 0,
    PARSE_TYPE_INT = 1,
    PARSE_TYPE_STRCPY = 2,
    PARSE_TYPE_STRDUP = 3,
    PARSE_TYPE_YESNO = 4,
    PARSE_TYPE_TRUEFALSE = 5
};

typedef struct ini_parse_command {
    char *    name;
    ParseType type;
    void *    ref;
} ini_parse_command;

ini_parse_command *ini_parse_commands;

int main(int argc, char **argv) {
    DiaGram diagram;
    int i;
    char *infile = NULL;    /* The input file */
    int parameter = 0;
    /* put to 1 in the params loop if the generator accepts buildtree option */
    int generator_buildtree = 0;
    int iniParameterProcessed;
    char inifile[BIG_BUFFER];
    
    int     tab;
    char *  ext = NULL, *outdir = NULL, *license = NULL;
    bool    overwrite = true, buildtree = false;

    GenerateCodeCpp *generator;

    char * notice = "\
dia2code version " VERSION ", Copyright (C) 2000-2014 Javier O'Hara\n\
Dia2Code comes with ABSOLUTELY NO WARRANTY\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; read the COPYING file for details.\n";

    char *help = "[-h|--help] [-d <dir>] [-nc] [-cl <classlist>]\n\
       [-t (ada|c|cpp|csharp|idl|java|php|php5|python|ruby|shp|sql|as3)] [-v]\n\
       [-l <license file>] [-ini <initialization file>] <diagramfile>";

    char *bighelp = "\
    -h --help            Print this help and exit.\n\
    -t <target>          Selects the output language. <target> can be\n\
                         one of: ada,c,cpp,idl,java,php,php5,python,ruby,shp,sql,as3 or csharp. \n\
                         Default is C++.\n\
    -d <dir>             Output generated files to <dir>, default is \".\" \n\
    --buildtree          Convert package names to a directory tree. off by default.\n\
    -l <license>         License file to prepend to generated files.\n\
    -nc                  Do not overwrite files that already exist.\n\
    -cl <classlist>      Generate code only for the classes specified in\n\
                         the comma-separated <classlist>. \n\
                         E.g: Base,Derived.\n\
    -v                   Invert the class list selection.  When used \n\
                         without -cl prevents any file from being created.\n\
    --tab <number>       Set numbre of spaces for one indentation.\n\
                         Default: 4. Maximum: 8.\
    -ext <extension>     Use <extension> as the file extension.\n\
                         Here are the defaults:\n\
                         ada:\"ads\", c:\"h\", cpp:\"h\", idl:\"idl\",\n\
                         java:\"java\", php:\"php\", python:\"py\", csharp:\"cs\".\n\
                         Not applicable to shp, sql.\n\
    -bext <extension>    Use <extension> as the body (implementation) file\n\
                         extension. Currently only applies only to ada.\n\
                         Here are the defaults:\n\
                         ada:\"adb\"\n\
    -ini <file>          Can be used instead of command-line parameters.\n\
    --debug <level>      Show debugging messages of this level.\n\
    <diagramfile>        The Dia file that holds the diagram to be read.\n\n\
    Note: parameters can be specified in any order.";

    /* initialise stuff like global variables to their default values */
    dia2code_initializations();

    generator = NULL;


    if (argc < 2) {
        fprintf(stderr, "%s\nUsage: %s %s\n", notice, argv[0], help);
        exit(2);
    }

    iniParameterProcessed = 0;

    /* Argument parsing: rewritten from scratch */
    for (i = 1; i < argc; i++) {
        switch ( parameter ) {
        case 0:
            if ( !strcmp (argv[i], "-t") ) {
                parameter = 1;
            } else if ( !strcmp (argv[i], "-d") ) {
                parameter = 2;
            } else if ( !strcmp (argv[i], "-nc") ) {
                overwrite = false;
            } else if ( !strcmp (argv[i], "-cl") ) {
                parameter = 3;
            } else if ( !strcmp (argv[i], "-l") ) {
                parameter = 4;
            } else if ( !strcmp (argv[i], "-ext") ) {
                parameter = 5;
            } else if ( !strcmp (argv[i], "-bext") ) {
                parameter = 6;
            } else if ( !strcmp (argv[i], "-ini") ) {
                parameter = 7;
            } else if ( !strcmp (argv[i], "-v") ) {
                diagram.setInvertSel (!diagram.getInvertSel ());
            } else if ( !strcmp (argv[i], "--debug") ) {
                parameter = 8;
            } else if ( !strcmp (argv[i], "--tab") ) {
                parameter = 9;
            } else if ( !strcmp("-h", argv[i]) || !strcmp("--help", argv[i]) ) {
                printf("%s\nUsage: %s %s\n\n%s\n", notice, argv[0], help, bighelp);
                exit(0);
            } else if ( !strcmp (argv[i], "--buildtree") ) {
                buildtree = true;
            } else {
                infile = argv[i];
            }
            break;
        case 1:   /* Which code generator */
            parameter = 0;
            if ( !strcmp (argv[i], "cpp") ) {
                generator = new GenerateCodeCpp (diagram);
            } else if ( !strcmp (argv[i], "java") ) {
//                generator = generators[1];
                generator_buildtree = 1;
            } else if ( !strcmp (argv[i], "c") ) {
//                generator = generators[2];
            } else if ( !strcmp (argv[i], "sql") ) {
//                generator = generators[3];
            } else if ( !strcmp (argv[i], "ada") ) {
//                generator = generators[4];
            } else if ( !strcmp (argv[i], "python") ) {
//                generator = generators[5];
            } else if ( !strcmp (argv[i], "php") ) {
//                generator = generators[6];
                generator_buildtree = 1;
            } else if ( !strcmp (argv[i], "shp") ) {
//                generator = generators[7];
            } else if ( !strcmp (argv[i], "idl") ) {
//                generator = generators[8];
            } else if ( !strcmp (argv[i], "csharp") ) {
//                generator = generators[9];
            } else if ( !strcmp(argv[i], "php5") ) {
//                generator = generators[10];
            } else if ( !strcmp(argv[i], "ruby") ) {
//                generator = generators[11];
            } else if ( !strcmp(argv[i], "as3") ) {
//                generator = generators[12];
                generator_buildtree = 1;
            } else {
#ifdef DSO
/*                generator = find_dia2code_module(argv[i]);
                if ( ! generator ) {
                    fprintf(stderr, "can't find the generator: %s\n", dlerror());
                    parameter = -1;
                }*/
#else
parameter = -1;
#endif
            }
            break;
        case 2:   /* Which output directory */
            outdir = argv[i];
            parameter = 0;
            break;
        case 3:   /* Which classes to consider */
            diagram.addGenClasses (parse_class_names(argv[i]));
            parameter = 0;
            break;
        case 4:   /* Which license file */
            license = argv[i];
            parameter = 0;
            break;
        case 5:   /* Which file extension */
            ext = argv[i];
            parameter = 0;
            break;
        case 6:   /* Which implementation file extension */
            body_file_ext = argv[i];
            parameter = 0;
            break;
        case 7:   /* Use initialization file */
            process_initialization_file (argv[i], 1);
            iniParameterProcessed = 1;
            parameter = 0;
            break;
        case 8:   /* Debug level */
            debug_setlevel (atoi (argv[i]));
            parameter = 0;
            break;
        case 9: {  /* Number of spaces for one indentation */
            int num = atoi( argv[i] );
            if ((num < 1) || (num > 8)) {
                fprintf (stderr, "The number of spaces for one indentation must be between 1 and 8.\n");
            }
            else {
                tab = num;
            }
            parameter = 0;
            break;
        }
        }
    }
    /* parameter != 0 means the command line was invalid */

    if ( parameter != 0 || infile == NULL ) {
        printf ("%s\nUsage: %s %s\n\n%s\n", notice, argv[0], help, bighelp);
        exit (2);
    }

    if (iniParameterProcessed == 0)
    {
        if (!process_initialization_file("dia2code.ini", 0))
        {
#ifdef WIN32
            if (getenv("HOME") == NULL)
                strcpy(inifile, "c:");
            else
                strcpy(inifile, getenv("HOME"));
            strcat(inifile, "\\dia2code\\dia2code.ini");
#else
            strcpy(inifile, "~/.dia2code/dia2code.ini");
#endif
            process_initialization_file(inifile, 0);           
        }
    }

    if (generator_buildtree == 0 && buildtree) {
        buildtree = false;
        fprintf( stderr,"warning: this generator does not support building tree yet. disabled \n" );
    }

    LIBXML_TEST_VERSION;
    xmlKeepBlanksDefault(0);

    /* We build the class list from the dia file here */
    diagram.setUml (parse_diagram(infile));

    /* Code generation */
    if ( !generator ) {
        fprintf( stderr,"error : no generator specify.\n" );
        exit (1);
    }
    
    ini_parse_command ini_parse_commands[] = {
        {"file.outdir",
         PARSE_TYPE_STRDUP,
         generator->getOutdirS ()},
        {"indent.brace.newline",
         PARSE_TYPE_YESNO,
         &indent_open_brace_on_newline},
        {"indent.size",
         PARSE_TYPE_INT,
         &indent_count},
        {"generate.backup",
         PARSE_TYPE_YESNO,
         &generate_backup},
        {NULL,
         -1,
         NULL}
    };

    generator->setIndent (tab);
    generator->setOverwrite (overwrite);
    generator->setBuildTree (buildtree);
    if (ext != NULL)
        generator->setFileExt (ext);
    if (outdir != NULL)
        generator->setOutdir (outdir);
    if (license != NULL)
        generator->setLicense (argv[i]);
    generator->generate_code ();
    delete generator;

    param_list_destroy();
    return 0;
}

void parse_command(char *name, char *value)
{
    int i = 0;
    void (*method)(char *, char *);

    while (1)
    {
        ini_parse_command *cmd = &ini_parse_commands[i];
        if(cmd->name == NULL)
            break;
        if (strcmp(cmd->name, name) != 0)
        {
            i++;
            continue;
        }
        switch(cmd->type)
        {
        case PARSE_TYPE_FUNCTION:
            method = cmd->ref;
            (*method)(name, value);
            break;

        case PARSE_TYPE_INT:
            *(int *)(cmd->ref) = atoi(value);
            break;

        case PARSE_TYPE_STRCPY:
            strcpy(value, (char *)cmd->ref);
            break;

        case PARSE_TYPE_STRDUP:
        {
            std::string *css = (std::string *)cmd->ref;
            css->assign (value);
            break;
        }
        
        case PARSE_TYPE_YESNO:
            switch(tolower(value[0]))
            {
            case 'y': *(int *)(cmd->ref) = 1; break;
            case 'n': *(int *)(cmd->ref) = 0; break;
            default:
                fprintf(stderr, "Invalid yes/no value for %s(%s)\n", name, value);
            }
            break;

        case PARSE_TYPE_TRUEFALSE:
            switch(tolower(value[0]))
            {
            case 't': *(int *)(cmd->ref) = 1; break;
            case 'f': *(int *)(cmd->ref) = 0; break;
            default:
                fprintf(stderr, "Invalid true/false value for %s(%s)\n", name, value);
            }
            break;


        default:
            break;
        }
        return;
    }
}

int process_initialization_file(char *filename, int exit_if_not_found)
{
    FILE *f = fopen(filename, "r");
    int line = 0;
    int slen;
    char s[LARGE_BUFFER];
    
    if (f == NULL)
    if (exit_if_not_found)
    {
        fprintf(stderr, "Could not open initialization file %s\n", filename);
        exit(-1);
    }
    else
        return 0;

    while (fgets(s, LARGE_BUFFER - 1, f) != NULL)
    {
        char *name = s;
        char *param = strchr(s, '=');

        line++;
        if (s[0] == '#')
            continue;

        if (param == NULL)
        {
            fprintf(stderr, "Invalid parameter entry in %s:%d\n", filename, line);
        }
        else
        {
            *(param++) = '\0';
            slen = strlen(param) - 1;
            while (param[slen] == '\n' || param[slen] == '\r')
            {
                param[slen--] = '\0';
            }
        }
        d2c_parameter_set(name, param);
        parse_command(name, param);
    }
    fclose(f);
    
    return 1;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
