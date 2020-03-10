static:
	make clean
	gcc -Wall -c -g diffoperations.c
	ar crs libdiffoperations.a diffoperations.o
	gcc -Wall main.c -g -o main -L. -ldiffoperations
shared:
	make clean
	gcc -Wall -fPIC -c -g diffoperations.c
	gcc -Wall -shared -fPIC -o libdiffoperations.so diffoperations.o
	gcc main.c -Wall -o main -L. -ldiffoperations -Wl,-rpath=`pwd`
clean:
	rm -f *.o *.a *.so *.tmp main

