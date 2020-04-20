all: client server

client: client.c
	gcc -Wall -g -o client client.c -lrt

server: server.c
	gcc -Wall -g -o server server.c -lrt

clean:
	rm -f *.o *.a *.so *.tmp main