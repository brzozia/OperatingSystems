all: siginherit_test exec_test

siginherit_test: siginherit_test.c
	make clean
	gcc -Wall -g -o exec_test exec_test.c
	gcc -Wall -g -o siginherit_test siginherit_test.c

exec_test: exec_test.c
	make clean
	gcc -Wall -g -o exec_test exec_test.c

clean:
	rm -f *.o *.a *.so


ignore:
	./siginherit_test ignore
handler:
	./siginherit_test handler
mask:
	./siginherit_test mask
pending:
	./siginherit_test pending

test:
	make ignore > raport2.txt
	make handler >> raport2.txt
	make mask >> raport2.txt
	make pending >> raport2.txt