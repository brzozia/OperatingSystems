all: find

find: find.c
	make clean
	gcc -Wall -g -o find find.c

clean:
	rm -f *.o *.a *.so *.tmp find

test:
	mkdir -p k1/k2
	mkdir -p k1/k3
	mkdir -p k4/k5/k6
	touch f.txt
	cd k1/k3
	touch f2.txt
	touch f3
	cd ..
	touch f4
	cd ..
	cd k4/k5
	touch f6
	ln -s k4/k5/k6 symlink1
	./find . -mtime 0 -maxdepth 6
	rm -r k1
	rm -r k4
	rm symlink1
	rm f.txt
	rm f3
	rm f4
	rm f6
	rm f2.txt



