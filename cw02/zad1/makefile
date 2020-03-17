all: program

program: program.c
	gcc -Wall -g -o program program.c

clean:
	rm -f *.o *.a *.so *.tmp main

test:
	./program generate dane1 90000000 1
	./program copy dane1 dane2 90000000 1 sys
	./program copy dane2 dane3 90000000 1 lib
	./program sort dane1 90000 1 sys
	./program sort dane2 90000 1 lib
	rm dane1 dane2 dane3

	./program generate dane1 10000000 1
	./program copy dane1 dane2 10000000 1 sys
	./program copy dane1 dane3 10000000 1 lib
	./program sort dane1 9000 1 sys
	./program sort dane2 9000 1 lib
	rm dane1 dane2 dane3
	./program generate dane1 90000000 4
	./program copy dane1 dane2 90000000 4 sys
	./program copy dane1 dane3 90000000 4 lib
	./program sort dane1 100000 4 sys
	./program sort dane2 100000 4 lib
	rm dane1 dane2 dane3

	./program generate dane1 10000000 4
	./program copy dane1 dane2 10000000 4 sys
	./program copy dane1 dane3 10000000 4 lib
	./program sort dane1 90000 4 sys
	./program sort dane2 90000 4 lib
	rm dane1 dane2 dane3

	./program generate dane1 9000000 512
	./program copy dane1 dane2 900000 512 sys
	rm dane2
	./program copy dane1 dane3 900000 512 lib
	./program sort dane1 90000 512 sys
	./program sort dane3 90000 512 lib
	rm dane1 dane3

	./program generate dane1 500000 512
	./program copy dane1 dane2 500000 512 sys
	rm dane2
	./program copy dane1 dane3 500000 512 lib
	./program sort dane1 10000 512 sys
	./program sort dane3 10000 512 lib
	rm dane1 dane3

	./program generate dane1 90000 1024
	./program copy dane1 dane2 90000 1024 sys
	rm dane2
	./program copy dane1 dane3 90000 1024 lib
	./program sort dane1 90000 1024 sys
	./program sort dane3 90000 1024 lib
	rm dane1 dane3

	./program generate dane1 50000 1024
	./program copy dane1 dane2 50000 1024 sys
	rm dane2
	./program copy dane1 dane3 50000 1024 lib
	./program sort dane1 10000 1024 sys
	./program sort dane3 10000 1024 lib
	rm dane1 dane3

	./program generate dane1 90000 4096
	./program copy dane1 dane2 90000 4096 sys
	rm dane2
	./program copy dane1 dane3 90000 4096 lib
	./program sort dane1 9000 4096 sys
	./program sort dane3 9000 4096 lib
	rm dane1 dane3

	./program generate dane1 8000 4096
	./program copy dane1 dane2 8000 4096 sys
	rm dane2
	./program copy dane1 dane3 8000 4096 lib
	./program sort dane1 50000 4096 sys
	./program sort dane3 50000 4096 lib
	rm dane1 dane3

	./program generate dane1 90000 8192
	./program copy dane1 dane2 90000 8192 sys
	rm dane2
	./program copy dane1 dane3 9000 8192 lib
	./program sort dane1 90000 8192 sys
	./program sort dane3 90000 8192 lib
	rm dane1 dane3

	./program generate dane1 50000 8192
	./program copy dane1 dane2 50000 8192 sys
	rm dane2
	./program copy dane1 dane3 50000 8192 lib
	./program sort dane1 9000 8192 sys
	./program sort dane3 9000 8192 lib
	rm dane1 dane3
