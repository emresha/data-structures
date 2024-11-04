#ifndef STACK_ARRAY_H__
#define STACK_ARRAY_H__

#define STACK_SIZE 1000

typedef struct
{
    char data[STACK_SIZE];
    int top;
} StackArray;

void init_stack_array(StackArray *stack);

int is_empty_array(StackArray *stack);

int is_full_array(StackArray *stack);

void push_array(StackArray *stack, char value);

char pop_array(StackArray *stack);

void print_stack_array(StackArray *stack);

int is_palindrome_array(StackArray *stack);

#endif
