all: program

program: program.c
	gcc -Wall -g program.c -o program -lpthread -lrt

test:
	./program 6 interleaved mountain.ascii.pgm wyj.txt

clean: 
	rm -f program