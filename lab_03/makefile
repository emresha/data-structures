CC := gcc
CFLAGS := -Wall -Werror -Wpedantic -std=c99 -g -O0

app.exe : main.o
	$(CC) $(CFLAGS) -o $@ $^

main.o : src/main.c
	$(CC) -c $<

