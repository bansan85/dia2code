bin_PROGRAMS = dia2code
dia2code_CPPFLAGS = @XML_CFLAGS@
dia2code_CXXFLAGS = @XML_CFLAGS@ -fpermissive -std=c++11
dia2code_LDADD    = @XML_LIBS@
dia2code_SOURCES  = dia2code.cpp main.cpp parse_diagram.cpp scan_tree.cpp decls.cpp source_parser.cpp DiaGram.cpp generate_code_cpp.cpp


#generate_code_c.cpp \
#generate_code_java.cpp generate_code_cpp.cpp generate_code_ada.cpp generate_code_sql.cpp \
#generate_code_python.cpp generate_code_php.cpp generate_code_shp.cpp generate_code_idl.cpp \
#generate_code_csharp.cpp generate_code_php_five.cpp generate_code_ruby.cpp generate_code_as3.cpp

EXTRA_DIST = code_generators.hpp decls.hpp dia2code.hpp parse_diagram.hpp source_parser.hpp DiaGram.hpp generate_code_cpp.hpp

