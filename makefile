link: compile
	gcc -o shell shell.o
	rm -rf shell.o

compile:
	gcc -g -c -o shell.o -Wall -pedantic-errors shell.c

clean: 
	rm -rf shell shell.o

run: link
	./shell
