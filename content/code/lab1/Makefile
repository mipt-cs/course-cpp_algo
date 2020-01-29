	.PHONY: all clean install uninstall
		
	all: program
		
	clean:
		rm -rf mylib *.o	
	program.o: program.cpp mylib.hpp
		gcc -c -o program.o program.cpp
	mylib.o: mylib.cpp mylib.hpp
		gcc -c -o mylib.o mylib.cpp
	program: program.o mylib.o
		gcc -o mylib program.o mylib.o
	install:
		install ./program /usr/local/bin
	uninstall:
		rm -rf /usr/local/bin/program
