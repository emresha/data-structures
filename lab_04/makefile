CC := gcc
CFLAGS := -std=c99 -Wall -Werror -pedantic -g -O0 -Iinc

app.exe: out out/main.o out/stack_array.o out/stack_list.o out/time_measure.o
	$(CC) $(CFLAGS) -o $@ out/main.o out/stack_array.o out/stack_list.o out/time_measure.o

out/main.o: src/main.c inc/stack_array.h inc/stack_list.h
	$(CC) $(CFLAGS) -c $< -o $@

out/stack_array.o: src/stack_array.c inc/stack_array.h
	$(CC) $(CFLAGS) -c $< -o $@

out/stack_list.o: src/stack_list.c inc/stack_list.h
	$(CC) $(CFLAGS) -c $< -o $@

out/time_measure.o: src/time_measure.c inc/stack_array.h inc/stack_list.h
	$(CC) $(CFLAGS) -c $< -o $@

out:
	mkdir -p out

.PHONY: clean

clean:
	rm -rf out *.exe
