all: program

program: program.c
	gcc -Wall -g program.c -o program -lpthread -lrt

test:
	# ./program 8 interleaved balloons.ascii.pgm ball_inter.pgm
	# ./program 4 sign balloons.ascii.pgm ball_sign.pgm
	# ./program 2 block balloons.ascii.pgm ball_block.pgm
	./program 8 interleaved casablanca.ascii.pgm casa_inter.pgm
	./program 4 sign casablanca.ascii.pgm casa_sign.pgm
	./program 2 block casablanca.ascii.pgm casa_block.pgm
	# ./program 8 interleaved mountain.ascii.pgm mount_inter.pgm
	# ./program 4 sign mountain.ascii.pgm mount_sign.pgm
	# ./program 2 block mountain.ascii.pgm mount_block.pgm

clean: 
	rm -f program