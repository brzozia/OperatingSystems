all: producent consument main

producent: producent.c
	make clean
	gcc -Wall -g producent.c -o producent

consument: consument.c
	make clean
	gcc -Wall -g consument.c -o consument

main: main.c
	make clean
	gcc -Wall -g main.c -o main

clean:
	rm -f *.o *.a *.so

test:
	./main FIFO wy.txt 12 a1 5 a2 5 a3 5 a4 5 a5 5
