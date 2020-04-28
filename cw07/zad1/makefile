all: program worker1 worker2 worker3

program: program.c common.h
	gcc -Wall -g program.c -o program

worker1:worker1.c common.h
	gcc -Wall -g worker1.c -o worker1

worker2:worker2.c common.h
	gcc -Wall -g worker2.c -o worker2

worker3:worker3.c common.h
	gcc -Wall -g worker3.c -o worker3

clean:
	 rm -f *.o *.a *.so *.tmp main

test:
	./program