CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c99 -pedantic -g -O0

app.exe : main.o
	$(CC) $(CFLAGS) -o $@ $^

main.o : main.c
	$(CC) $(CFLAGS) -c $<
