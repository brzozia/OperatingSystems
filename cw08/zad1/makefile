all: program

program: program.c
	gcc -Wall -g program.c -o program -lpthread

test:
	echo "Number of threads: 1" > Times.txt
	echo "Option: sign" >> Times.txt
	./program 1 sign casablanca.ascii.pgm casa_sign.pgm >> Times.txt
	echo "Option: block" >> Times.txt
	./program 1 block casablanca.ascii.pgm casa_block.pgm >>Times.txt
	echo "Option: interleaved" >> Times.txt
	./program 1 interleaved casablanca.ascii.pgm casa_inter.pgm >>Times.txt

	echo "" >>Times.txt
	echo "Number of threads: 2" >> Times.txt
	echo "Option: sign" >> Times.txt
	./program 2 sign casablanca.ascii.pgm casa_sign.pgm >> Times.txt
	echo "Option: block" >> Times.txt
	./program 2 block casablanca.ascii.pgm casa_block.pgm >>Times.txt
	echo "Option: interleaved" >> Times.txt
	./program 2 interleaved casablanca.ascii.pgm casa_inter.pgm >>Times.txt

	echo "" >>Times.txt
	echo "Number of threads: 4" >> Times.txt
	echo "Option: sign" >> Times.txt
	./program 4 sign casablanca.ascii.pgm casa_sign.pgm >> Times.txt
	echo "Option: block" >> Times.txt
	./program 4 block casablanca.ascii.pgm casa_block.pgm >>Times.txt
	echo "Option: interleaved" >> Times.txt
	./program 4 interleaved casablanca.ascii.pgm casa_inter.pgm >>Times.txt

	echo "" >>Times.txt
	echo "Number of threads: 8" >> Times.txt
	echo "Option: sign" >> Times.txt
	./program 8 sign casablanca.ascii.pgm casa_sign.pgm >> Times.txt
	echo "Option: block" >> Times.txt
	./program 8 block casablanca.ascii.pgm casa_block.pgm >>Times.txt
	echo "Option: interleaved" >> Times.txt
	./program 8 interleaved casablanca.ascii.pgm casa_inter.pgm >>Times.txt


clean: 
	rm -f program