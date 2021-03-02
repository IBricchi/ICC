  
CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include

AST_BIN += include/bin/ast.o include/bin/util.o include/bin/expression.o
AST_BIN += include/bin/primitive.o include/bin/statement.o include/bin/structure.o

# moves wrapper to form c_compiler
bin/c_compiler : bin/compiler src/wrapper.sh
	cp src/wrapper.sh bin/c_compiler
	chmod u+x bin/c_compiler

# compile ast into objects
include/bin/ast.o: include/ast_src/ast.cpp include/ast_src/ast.hpp
include/bin/util.o: include/ast_src/util.cpp include/ast_src/util.hpp
include/bin/expression.o: include/ast_src/expressions.cpp include/ast_src/expression.hpp
include/bin/primitive.o: include/ast_src/primitive.cpp include/ast_src/primitive.hpp
include/bin/statement.o: include/ast_src/statement.cpp include/ast_src/statement.hpp
include/bin/structure.o: include/ast_src/structure.cpp include/ast_src/structure.hpp

$(AST_BIN):
	g++ $(CPPFLAGS) -o $@ -c $<


# compile compiler
bin/compiler : src/compiler.cpp $(AST_BIN) include/ast parser src/parser/parser.tab.o src/parser/lexer.yy.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/compiler src/compiler.cpp $(AST_BIN) src/parser/parser.tab.o src/parser/lexer.yy.o

# compiles yacc and lexx files
parser: src/parser/parser.tab.cpp src/parser/parser.tab.hpp src/parser/lexer.yy.cpp

src/parser/parser.tab.cpp src/parser/parser.tab.hpp : src/parser/parser.y
	bison -v -d src/parser/parser.y -o src/parser/parser.tab.cpp
	g++ $(CPPFLAGS) -o src/parser/parser.tab.o -c src/parser/parser.tab.cpp

src/parser/lexer.yy.cpp : src/parser/lexer.l src/parser/parser.tab.hpp
	flex -o src/parser/lexer.yy.cpp src/parser/lexer.l
	g++ $(CPPFLAGS) -o src/parser/lexer.yy.o -c src/parser/lexer.yy.cpp

touch :
	find . -exec touch {} \;

clean :
	rm -f src/*.o
	rm -f bin/*
	rm -f include/bin/*
	rm -f src/parser/*.o
	rm -f src/parser/*.output
	rm -f src/parser/*.tab.cpp
	rm -f src/parser/*.tab.hpp
	rm -f src/parser/*.yy.cpp
