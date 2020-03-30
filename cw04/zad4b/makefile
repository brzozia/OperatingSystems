all: sender catcher

sender: sender.c
	make clean
	gcc -Wall -g catcher.c -o catcher
	gcc -Wall -g sender.c -o sender

catcher: catcher.c
	make clean
	gcc -Wall -g catcher.c -o catcher

clean:
	rm -f *.o *.tmp