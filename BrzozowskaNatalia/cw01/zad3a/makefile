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
dynamic:
	make clean
	gcc -Wall -fPIC -c diffoperations.c -o diffoperations.o
	gcc -Wall -Wl,-rpath=. -fPIC -shared -o libdiffoperations.so diffoperations.o
	gcc -Wall -g -fPIC -ldl -L. -o maindynamic maindynamic.c -D DYNAMIC
clean:
	rm -f *.o *.a *.so *.tmp main
tests:
	./main 5 compare_pairs lalka.txt:3muszkiet.txt biblia.txt:3muszkiet.txt lalka.txt:t345.txt t345.txt:t678.txt >> results3a.txt;
	./main 30 compare_pairs lalka.txt:3muszkiet.txt iliada.txt:3muszkiet.txt lalka.txt:t345.txt t345.txt:t678.txt iliada.txt:odyseja.txt iliada.txt:lalka.txt odyseja.txt:lalka.txt a.txt:short.txt b.txt:short.txt makbet.txt:short.txt t1.txt:short.txt t2.txt:short.txt t1.txt:t2.txt makbet.txt:lalka.txt makbet.txt:iliada.txt makbet.txt:odyseja.txt makbet.txt:3muszkiet.txt makbet.txt:biblia.txt makbet.txt:short.txt makbet.txt:b.txt makbet.txt:t345.txt t678.txt:short.txt a.txt:t1.txt a.txt:t2.txt a.txt:b.txt b.txt:t2.txt b.txt:t1.txt t345.txt:t678.txt t345.txt:lalka.txt remove_block 0 remove_block 1 remove_block 3 remove_block 6 remove_block 8 remove_block 10 remove_block 14 remove_block 18 remove_block 22 remove_block 27 compare_pairs lalka.txt:t345.txt odyseja.txt:lalka.txt iliada.txt:lalka.txt short.txt:a.txt 3muszkiet.txt:makbet.txt remove_block 0 remove_block 1 remove_block 2 remove_block 3 compare_pairs lalka.txt:3muszkiet.txt lalka.txt:odyseja.txt t345.txt:t678.txt a.txt:short.txt bible.txt:lalka.txt remove_block 5 remove_block 7 >> results3a.txt;
	./main 30 compare_pairs lalka.txt:3muszkiet.txt iliada.txt:3muszkiet.txt lalka.txt:t345.txt t345.txt:t678.txt iliada.txt:odyseja.txt iliada.txt:lalka.txt odyseja.txt:lalka.txt a.txt:short.txt b.txt:short.txt makbet.txt:short.txt t1.txt:short.txt t2.txt:short.txt t1.txt:t2.txt makbet.txt:lalka.txt makbet.txt:iliada.txt makbet.txt:odyseja.txt makbet.txt:3muszkiet.txt makbet.txt:biblia.txt makbet.txt:short.txt makbet.txt:b.txt makbet.txt:t345.txt t678.txt:short.txt a.txt:t1.txt a.txt:t2.txt a.txt:b.txt b.txt:t2.txt b.txt:t1.txt t345.txt:t678.txt t345.txt:lalka.txt remove_block 0 remove_block 1 remove_block 3 remove_block 6 remove_block 8 remove_block 10 remove_block 14 remove_block 18 remove_block 22 remove_block 27 compare_pairs lalka.txt:t345.txt odyseja.txt:lalka.txt iliada.txt:lalka.txt short.txt:a.txt 3muszkiet.txt:makbet.txt remove_block 0 remove_block 1 remove_block 2 remove_block 3 compare_pairs lalka.txt:3muszkiet.txt lalka.txt:odyseja.txt t345.txt:t678.txt a.txt:short.txt bible.txt:lalka.txt remove_block 5 remove_block 7  remove_operation 8 0 remove_operation 6 0 >> results3a.txt;
	./main 3 create_array 5 compare_pairs a.txt:b.txt lalka.txt:3muszkiet.txt biblia.txt:3muszkiet.txt >> results3a.txt;
	./main 2 compare_pairs a.txt:b.txt lalka.txt:3muszkiet.txt remove_block 7 >> results3a.txt;
	./main 2 compare_pairs a.txt:b.txt lalka.txt:3muszkiet.txt remove_operation 4 6 >> results3a.txt;

