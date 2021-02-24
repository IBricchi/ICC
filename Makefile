  
CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include

bin/c_compiler : bin/compiler src/wrapper.sh
	cp src/wrapper.sh bin/c_compiler
	chmod u+x bin/c_compiler

bin/compiler : src/compiler.cpp
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/compiler $^

parser: src/parser/parser.tab.cpp src/parser/parser.tab.hpp src/parser/lexer.yy.cpp

src/parser/parser.tab.cpp src/parser/parser.tab.hpp : src/parser/parser.y
	bison -v -d src/parser/parser.y -o src/parser/parser.tab.cpp

src/parser/lexer.yy.cpp : src/parser/lexer.l src/parser/parser.tab.hpp
	flex -o src/parser/lexer.yy.cpp src/parser/lexer.l

touch :
	find . -exec touch {} \;

clean :
	rm -f src/*.o
	rm -f bin/*
	rm -f src/parser/*.o
	rm -f src/parser/*.output
	rm -f src/parser/*.tab.cpp
	rm -f src/parser/*.tab.hpp
	rm -f src/parser/*.yy.cpp
