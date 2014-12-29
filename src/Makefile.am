bin_PROGRAMS = dia2code
dia2code_CPPFLAGS = @XML_CFLAGS@
dia2code_CXXFLAGS = @XML_CFLAGS@ -std=c++11 -Wall -Wextra -Winline -Wpacked -Wmissing-format-attribute -Wstrict-aliasing=2 -Wuninitialized -Wfloat-equal -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Wformat=2 -Wformat-nonliteral -Wformat-security -Wswitch-enum -Winit-self -fstrict-overflow -Wstrict-overflow=1 -Wno-unused-parameter -Wno-error=deprecated-declarations -Wconversion -Wold-style-cast -Wno-overloaded-virtual -Wwrite-strings -Wcast-qual -Wcast-align -Wredundant-decls -Wlogical-op -Wpointer-arith -fprofile-arcs -ftest-coverage
dia2code_LDADD    = @XML_LIBS@
dia2code_SOURCES  = string2.cpp main.cpp parse_diagram.cpp scan_tree.cpp DiaGram.cpp GenerateCodeCpp.cpp GenerateCode.cpp umlAttribute.cpp umlOperation.cpp umlPackage.cpp umlClass.cpp umlClassNode.cpp #GenerateCodeJava.cpp


#generate_code_c.cpp \
#generate_code_java.cpp generate_code_ada.cpp generate_code_sql.cpp \
#generate_code_python.cpp generate_code_php.cpp generate_code_shp.cpp generate_code_idl.cpp \
#generate_code_csharp.cpp generate_code_php_five.cpp generate_code_ruby.cpp generate_code_as3.cpp

EXTRA_DIST = decls.hpp string2.hpp parse_diagram.hpp DiaGram.hpp GenerateCodeCpp.hpp GenerateCode.hpp umlAttribute.hpp umlOperation.hpp umlPackage.hpp umlClass.hpp umlClassNode.hpp umlAssoc.hpp scan_tree.hpp #GenerateCodeJava.hpp

CLEANFILES = *.gcda *.gcno
