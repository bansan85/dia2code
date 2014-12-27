/*
This file is part of dia2code. It generates code from an UML Dia Diagram.
Copyright (C) 2000-2014 Javier O'Hara

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

#include "config.h"

#include <libxml/tree.h>
#include <iostream>

#include "string2.hpp"
#include "GenerateCodeCpp.hpp"
//#include "GenerateCodeJava.hpp"

int main (int argc, char **argv) {
    DiaGram diagram;
    int i;
    char *infile = NULL;    /* The input file */
    int parameter = 0;
    
    uint8_t tab = 4;
    char   *ext = NULL, *bext = NULL, *outdir = NULL, *license = NULL;
    bool    overwrite = true, buildtree = false, newline = false,
            oneclass = false;

    GenerateCodeCpp *generator;

    const char * notice = "\
dia2code version " VERSION ", Copyright (C) 2000-2014 Javier O'Hara\n\
Dia2Code comes with ABSOLUTELY NO WARRANTY\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; read the COPYING file for details.\n";

    const char *help = "[-h|--help] [-d <dir>] [--buildtree] [-l <license file>\n\
       [-nc] [-cl <classlist>] [-v] [--version] [--tab <number>]\n\
       [-ext <extension>] [-bext <extension>] [-nl] [-1] \n\
       <-t (ada|as3|c|cpp|csharp|idl|java|php|php5|python|ruby|shp|sql)>\n\
       <diagramfile>";

    const char *bighelp = "\
    -h --help            Print this help and exit.\n\
    -d <dir>             Output generated files to <dir>, default is \".\" \n\
    --buildtree          Convert package names to a directory tree.\n\
                         Off by default.\n\
    -l <license>         License file to prepend to generated files.\n\
    -nc                  Do not overwrite files that already exist.\n\
    -cl <classlist>      Generate code only for the classes specified in\n\
                         the comma-separated <classlist>. \n\
                         E.g: Base,Derived.\n\
    -v                   Invert the class list selection.  When used \n\
                         without -cl prevents any file from being created.\n\
    --version            Show version x.y.z.\n\
    --tab <number>       Set numbre of spaces for one indentation.\n\
                         Default: 4. Maximum: 8.\n\
    -ext <extension>     Use <extension> as the file extension.\n\
                         Here are the defaults:\n\
                         ada:\"ads\", c:\"h\", cpp:\"hpp\", idl:\"idl\",\n\
                         java:\"java\", php:\"php\", python:\"py\",\n\
                         csharp:\"cs\".\n\
                         Not applicable to shp, sql.\n\
    -bext <extension>    Use <extension> as the body (implementation) file\n\
                         extension. Only applies only to ada and c.\n\
                         Here are the defaults:\n\
                         ada:\"adb\", c:\"c\"\n\
    -nl                  Create new line on new brace. Off by default.\n\
    -1                   One header contains only one header.\n\
                         Off by default but java.\n\
    -t <target>          Selects the output language. <target> can be one of:\n\
                         ada,as3,c,cpp,csharp,idl,java,php,php5,python,ruby,\n\
                         shp,sql. \n\
    <diagramfile>        The Dia file that holds the diagram to be read.\n\n\
    Note: parameters can be specified in any order.";

    generator = NULL;


    if (argc < 2) {
        fprintf (stderr, "%s\nUsage: %s %s\n", notice, argv[0], help);
        exit (2);
    }

    /* Argument parsing: rewritten from scratch */
    for (i = 1; i < argc; i++) {
        switch (parameter) {
        case 0:
            if (!strcmp ("-h", argv[i]) ||
                !strcmp ("--help", argv[i])) {
                printf ("%s\nUsage: %s %s\n\n%s\n",
                        notice,
                        argv[0],
                        help,
                        bighelp);
                exit (0);
            } else if (!strcmp (argv[i], "-d")) {
                parameter = 2;
            } else if (!strcmp (argv[i], "--buildtree")) {
                buildtree = true;
            } else if (!strcmp (argv[i], "-l")) {
                parameter = 4;
            } else if (!strcmp (argv[i], "-nc")) {
                overwrite = false;
            } else if (!strcmp (argv[i], "-cl")) {
                parameter = 3;
            } else if (!strcmp (argv[i], "-v")) {
                diagram.setInvertSel (!diagram.getInvertSel ());
            } else if (!strcmp (argv[i], "--version")) {
                std::cout << PACKAGE_VERSION << std::endl;
                exit (0);
            } else if (!strcmp (argv[i], "--tab")) {
                parameter = 8;
            } else if (!strcmp (argv[i], "-ext")) {
                parameter = 5;
            } else if (!strcmp (argv[i], "-bext")) {
                parameter = 6;
            } else if (!strcmp (argv[i], "-nl")) {
                newline = true;
            } else if (!strcmp (argv[i], "-1")) {
                oneclass = true;
            } else if (!strcmp (argv[i], "-t")) {
                parameter = 1;
            } else {
                infile = argv[i];
            }
            break;
        case 1:   /* Which code generator */
            parameter = 0;
            if (!strcmp (argv[i], "cpp")) {
                generator = new GenerateCodeCpp (diagram);
            } else if (!strcmp (argv[i], "java")) {
//                generator = new GenerateCodeJava (diagram);
            } else if (!strcmp (argv[i], "c")) {
//                generator = generators[2];
            } else if (!strcmp (argv[i], "sql")) {
//                generator = generators[3];
            } else if (!strcmp (argv[i], "ada")) {
//                generator = generators[4];
            } else if (!strcmp (argv[i], "python")) {
//                generator = generators[5];
            } else if (!strcmp (argv[i], "php")) {
//                generator = generators[6];
            } else if (!strcmp (argv[i], "shp")) {
//                generator = generators[7];
            } else if (!strcmp (argv[i], "idl")) {
//                generator = generators[8];
            } else if (!strcmp (argv[i], "csharp")) {
//                generator = generators[9];
            } else if (!strcmp(argv[i], "php5")) {
//                generator = generators[10];
            } else if (!strcmp(argv[i], "ruby")) {
//                generator = generators[11];
            } else if (!strcmp(argv[i], "as3")) {
//                generator = generators[12];
            } else {
                parameter = -1;
            }
            break;
        case 2:   /* Which output directory */
            outdir = argv[i];
            parameter = 0;
            break;
        case 3:   /* Which classes to consider */
            diagram.addGenClasses (parse_class_names (argv[i]));
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
        case 6:   /* Which file extension for body file */
            bext = argv[i];
            parameter = 0;
            break;
        case 8: {  /* Number of spaces for one indentation */
            int num = atoi (argv[i]);
            if ((num < 1) || (num > 8)) {
                fprintf (stderr,
        "The number of spaces for one indentation must be between 1 and 8.\n");
            }
            else {
                tab = static_cast <uint8_t> (num);
            }
            parameter = 0;
            break;
        }
        default : {
            fprintf (stderr, "Unknown parameter : %s.\n", argv[i]);
        }
        }
    }
    /* parameter != 0 means the command line was invalid */

    if (parameter != 0 || infile == NULL) {
        printf ("%s\nUsage: %s %s\n\n%s\n", notice, argv[0], help, bighelp);
        exit (2);
    }

    LIBXML_TEST_VERSION;
    xmlKeepBlanksDefault (0);

    // We build the class list from the dia file here
    umlClass::parse_diagram (infile, diagram.getUml ());

    // Code generation
    if (!generator) {
        fprintf (stderr, "error : no generator specify.\n");
        exit (1);
    }
    
    generator->setIndent (tab);
    generator->setOverwrite (overwrite);
    generator->setBuildTree (buildtree);
    generator->setOpenBraceOnNewline (newline);
    generator->setOneClass (oneclass);
    if (ext != NULL) {
        generator->setFileExt (ext);
    }
    if (bext != NULL) {
        generator->setBodyFileExt (bext);
    }
    if (outdir != NULL) {
        generator->setOutdir (outdir);
    }
    if (license != NULL) {
        generator->setLicense (license);
    }
    generator->generate_code ();
    delete generator;

    xmlCleanupParser ();

    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
