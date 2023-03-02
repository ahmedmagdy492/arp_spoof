CC=gcc

all:
	make libs exe

libs:
	$(CC) -c -O util.c arg_parser.c

exe:
	$(CC) -g -o main main.c util.o arg_parser.o

clean:
	rm -rf main
