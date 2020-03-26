all: macierz ps_work pastef

macierz: macierz.c
	make clean
	gcc -Wall -g -o macierz macierz.c

pastef: pastef.c
	make clean
	gcc -Wall -g -o pastef pastef.c

clean:
	rm -f *.o *.a *.so *.tmp main
