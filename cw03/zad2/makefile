all: macierz ps_work

macierz: macierz.c
	make clean
	gcc -Wall -g -o macierz macierz.c

ps_work: ps_work.c
	make clean
	gcc -Wall -g -o ps_work ps_work.c
clean:
	rm -f *.o *.a *.so *.tmp main
