all: client server

client: client.c
	gcc -Wall -g -o client client.c

server: server.c
	gcc -Wall -g -o server server.c

clean:
	rm -f *.o *.a *.so *.tmp main