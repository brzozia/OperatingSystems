all: golibroda

golibroda: golibroda.c
	gcc -Wall -g golibroda.c -o golibroda -lpthread

test:
	./golibroda 4 15

clean: 
	rm -f golibroda