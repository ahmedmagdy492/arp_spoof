CC=gcc

lib:
	$(CC) -c -O util.c

exe:
	$(CC) -g -o main main.c util.o

clean:
	rm -rf main
