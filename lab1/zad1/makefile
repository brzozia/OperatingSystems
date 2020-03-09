static:
	make clean
	gcc -Wall -c -g diffoperations.c
	ar crs libdiffoperations.a diffoperations.o
	gcc -Wall main.c -g -o main -L. -ldiffoperations
dynamic:
	make clean
	gcc -Wall -c -g diffoperations.c
	gcc -Wall -shared -fPIC -o libdiffoperations.so diffoperations.o
	gcc main.c -Wall -o main -ldiffoperations
clean:
	rm -f *.o *.a *.so main
