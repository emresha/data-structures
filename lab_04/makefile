COMPILER_NAME := gcc
FLAGS := -std=c99 -Wall -Werror -pedantic -g -O0 -Iinc

app.exe: out out/main.o out/stack_array.o out/stack_list.o out/timing.o
	$(COMPILER_NAME) $(FLAGS_LINK) -o $@ out/main.o out/stack_array.o out/stack_list.o out/timing.o

out/main.o: src/main.c inc/stack_array.h inc/stack_list.h inc/constants.h
	$(COMPILER_NAME) $(FLAGS) -c $< -o $@

out/stack_array.o: src/stack_array.c inc/stack_array.h inc/constants.h
	$(COMPILER_NAME) $(FLAGS) -c $< -o $@

out/stack_list.o: src/stack_list.c inc/stack_list.h inc/constants.h
	$(COMPILER_NAME) $(FLAGS) -c $< -o $@

out/timing.o: src/timing.c inc/stack_array.h
	$(COMPILER_NAME) $(FLAGS) -c $< -o $@

# .PHONY: out
out:
	mkdir -p out

.PHONY: clean
clean:
	rm -rf out
	rm -f *.exe
	rm -f *.gcov
