all: macierz helper pastef ps_work

macierz: macierz.c
	make clean
	gcc -Wall -g -o pastef pastef.c
	gcc -Wall -g -o macierz macierz.c

ps_work:
	gcc -Wall -g -o macierz macierz.c

helper: helper.c
	make clean
	gcc -Wall -g -o helper helper.c

pastef: pastef.c
	gcc -Wall -g -o pastef pastef.c

clean:
	rm -f *.o *.a *.so *.tmp *0 *1 *2 *3 *4 *5 *6 *7 *8 *9 main
	
test:
	make clean
	./helper 16 1 9 lista2
	./macierz lista2 4 1 1 1 1 
	./helper test lista2

	make clean
	./helper 23 1 13 lista3
	./macierz lista3 7 2 2 1 1
	./helper test lista3

	make clean
	./helper 10 1 15 lista4
	./macierz lista4 10 2 1 2 2
	./helper test lista4

	make clean
	./helper 13 1 7 lista5
	./macierz lista5 10 3 2 1 1
	./helper test lista5

	make clean
	./helper 5 1 15 lista4
	./macierz lista4 1 1 1 1 1
	./helper test lista4

	make clean
	./helper 50 1 13 lista5
	./macierz lista5 10 5 2 5 10
	./helper test lista5
	make clean

