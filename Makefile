CC = gcc
CFLAGS = -Wall -std=c99

main:
	$(CC) $(CFLAGS)  main.c -o main
debug:
	$(CC) $(CFLAGS) -g main.c -o main
clean:
	rm -f main
