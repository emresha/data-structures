#ifndef STACK_ARRAY_H__
#define STACK_ARRAY_H__

#define MAX_SIZE 10000

typedef struct
{
    int str[MAX_SIZE];
    int top;
}
StackArray;

void init_stack_array(StackArray *stack);

int is_empty_array(StackArray *stack);

int is_full_array(StackArray *stack);

void push_array(StackArray *stack, char value);

void print_stack_array(StackArray *stack);

char pop_array(StackArray *stack);

void sort_stacks(StackArray *stack1, StackArray *stack2, StackArray *sorted_stack);

#endif
