all: main

main: main.c
	make clean
	gcc -Wall -g main.c -o main
clean:
	rm -f *.o *.a *.so
test: 
	./main tasks
