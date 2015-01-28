bin_PROGRAMS = dia2code
dia2code_CPPFLAGS = @XML_CFLAGS@
dia2code_CXXFLAGS = @XML_CFLAGS@ -std=c++11 -Wall -Wextra -Wpacked -Wmissing-format-attribute -fstrict-aliasing -Wstrict-aliasing=3 -Wuninitialized -Wfloat-equal -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Wpedantic -Wformat=2 -Wformat-nonliteral -Wformat-security -Winit-self -fstrict-overflow -Wstrict-overflow=5 -Wno-unused-parameter -Wconversion -Wold-style-cast -Wno-overloaded-virtual -Wwrite-strings -Wcast-qual -Wcast-align -Wredundant-decls -Wlogical-op -Wpointer-arith -fprofile-arcs -ftest-coverage -g3 -ggdb3 -O0 -pipe -fno-omit-frame-pointer -ftrapv -Wzero-as-null-pointer-constant
dia2code_LDADD    = @XML_LIBS@
dia2code_SOURCES  = DiaGram.cpp \
										GenerateCode.cpp \
										GenerateCodeCpp.cpp \
										GenerateCodeCpp11.cpp \
										GenerateCodeJava.cpp \
                    main.cpp \
										parse_diagram.cpp \
										scan_tree.cpp \
										string2.cpp \
										umlAttribute.cpp \
										umlClass.cpp \
										umlClassNode.cpp \
										umlOperation.cpp \
										umlPackage.cpp


#generate_code_c.cpp \
#generate_code_java.cpp generate_code_ada.cpp generate_code_sql.cpp \
#generate_code_python.cpp generate_code_php.cpp generate_code_shp.cpp generate_code_idl.cpp \
#generate_code_csharp.cpp generate_code_php_five.cpp generate_code_ruby.cpp generate_code_as3.cpp

EXTRA_DIST = decls.hpp \
						 DiaGram.hpp \
						 GenerateCode.hpp \
						 GenerateCodeCpp.hpp \
						 GenerateCodeCpp11.hpp \
						 GenerateCodeJava.hpp \
						 parse_diagram.hpp \
						 scan_tree.hpp \
             string2.hpp \
						 umlAssoc.hpp \
						 umlAttribute.hpp \
						 umlClass.hpp \
						 umlClassNode.hpp \
						 umlOperation.hpp \
						 umlPackage.hpp

CLEANFILES = *.gcda *.gcno
