all: klient server

server: server.c
	gcc -Wall -g server.c -o server -lpthread

klient: klient.c
	gcc -Wall -g klient.c -o klient

clean:
	rm -f klient server
