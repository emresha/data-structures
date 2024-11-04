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
    {
        stack->data[++stack->top] = value;
    }
    else
    {
        printf("Стек переполнен!\n");
        printf("Удалите элемент для продолжения!\n");
    }
}

char pop_array(StackArray *stack)
{
    if (!is_empty_array(stack)) 
    {
        return stack->data[stack->top--];
    }
    return '\0';
}

void print_stack_array(StackArray *stack)
{
    if (is_empty_array(stack)) 
    {
        printf("Стек пуст\n");
        return;
    }
    printf("Текущий стек (массив)\n");
    for (int i = stack->top; i >= 0; i--) 
    {
        printf("| %c |\n", stack->data[i]);
    }
    printf("\n");
}

int is_palindrome_array(StackArray *stack) 
{
    char element;

    StackArray temp_fill_checking, temp_fill_reserve;

    init_stack_array(&temp_fill_checking);
    init_stack_array(&temp_fill_reserve);

    int size = stack->top + 1;

    while (!is_empty_array(stack))
    {
        element = pop_array(stack);
        push_array(&temp_fill_checking, element);
        push_array(&temp_fill_reserve, element);
    }

    while (!is_empty_array(&temp_fill_checking))
    {
        push_array(stack, pop_array(&temp_fill_checking));
    }

    for (size_t i = 0; i < (size / 2); i++)
    {
        push_array(&temp_fill_checking, pop_array(stack));
    }

    if (size % 2 != 0)
    {
        pop_array(stack);
    }

    while (!is_empty_array(&temp_fill_checking) || !is_empty_array(stack))
    {
        if (pop_array(&temp_fill_checking) != pop_array(stack))
        {
            while (!is_empty_array(stack))
            {
                pop_array(stack);
            }
            for (size_t i = 0; i < size; i++)
            {
                push_array(stack, pop_array(&temp_fill_reserve));
            }
            return 0;
        }
    }

    for (size_t i = 0; i < size; i++)
    {
        push_array(stack, pop_array(&temp_fill_reserve));
    }

    return 1;
}
