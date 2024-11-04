#include "stack_array.h"
#include <stdio.h>

void init_stack_array(StackArray *stack) 
{
    stack->top = -1;
}

int is_empty_array(StackArray *stack) 
{
    return stack->top == -1;
}

int is_full_array(StackArray *stack) 
{
    return stack->top == STACK_SIZE - 1;
}

void push_array(StackArray *stack, char value)
{
    if (!is_full_array(stack)) 
        stack->data[++stack->top] = value;
    else
        printf("Переполнение стека.\n");
}

char pop_array(StackArray *stack)
{
    if (!is_empty_array(stack)) 
        return stack->data[stack->top--];
    
    return 0;
}

void print_stack_array(StackArray *stack)
{
    if (is_empty_array(stack)) 
        printf("Стек пустой.\n");
    else
    {
        printf("Текущий стек (массив)\n");
        for (int i = stack->top; i >= 0; i--) 
            printf("| %c |\n", stack->data[i]);
    }

    printf("\n");
}

int is_palindrome_array(StackArray *stack) 
{
    StackArray temp_stack;
    init_stack_array(&temp_stack);

    int size = stack->top + 1;
    int mid = size / 2;
    int is_palindrome = 1;

    for (int i = 0; i < mid; i++) 
    {
        push_array(&temp_stack, pop_array(stack));
    }

    if (size % 2 != 0) 
    {
        pop_array(stack);
    }

    while (!is_empty_array(&temp_stack)) 
    {
        if (pop_array(&temp_stack) != pop_array(stack)) 
        {
            is_palindrome = 0;
            break;
        }
    }

    for (int i = 0; i < mid; i++) 
    {
        push_array(stack, pop_array(&temp_stack));
    }

    return is_palindrome;
}
