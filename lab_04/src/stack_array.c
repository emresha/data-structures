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
    return stack->top == MAX_SIZE - 1;
}

void push_array(StackArray *stack, char value)
{
    if (!is_full_array(stack))
        stack->str[++stack->top] = value;
    else
        printf("Переполнение стека.\n");
}

char pop_array(StackArray *stack)
{
    if (!is_empty_array(stack))
        return stack->str[stack->top--];

    return 0;
}

void print_stack_array(StackArray *stack)
{
    if (is_empty_array(stack))
        printf("Стек пустой.\n");
    else
    {
        printf("Текущий стек\n");
        for (int i = stack->top; i >= 0; i--)
            printf("\'%d\'\n", stack->str[i]);
    }

    printf("\n");
}

void sort_stacks(StackArray *stack1, StackArray *stack2, StackArray *sorted_stack)
{
    init_stack_array(sorted_stack);

    while (!is_empty_array(stack1))
    {
        push_array(sorted_stack, pop_array(stack1));
    }
    while (!is_empty_array(stack2))
    {
        push_array(sorted_stack, pop_array(stack2));
    }

    StackArray temp_stack;
    init_stack_array(&temp_stack);

    while (!is_empty_array(sorted_stack))
    {
        int temp = pop_array(sorted_stack);

        while (!is_empty_array(&temp_stack) && temp_stack.str[temp_stack.top] > temp)
        {
            push_array(sorted_stack, pop_array(&temp_stack));
        }

        push_array(&temp_stack, temp);
    }

    while (!is_empty_array(&temp_stack))
    {
        push_array(sorted_stack, pop_array(&temp_stack));
    }
}
