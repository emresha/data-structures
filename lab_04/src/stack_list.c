
#include "stack_list.h"
#include <stdio.h>
#include <stdlib.h>

void free_stack_list(StackList *stack)
{
    Node *current = stack->top;

    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}

void init_stack_list(StackList *stack)
{
    stack->top = NULL;
}

void init_free_list(FreeList *free_list)
{
    free_list->count = 0;
}

int is_empty_list(StackList *stack)
{
    return stack->top == NULL;
}

void push_list(StackList *stack, char value)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node)
    {
        new_node->data = value;
        new_node->next = stack->top;
        stack->top = new_node;
    }
}

char pop_list(StackList *stack, FreeList *free_list)
{
    if (is_empty_list(stack))
        return '\0';
    
    Node *temp = stack->top;
    char value = temp->data;
    stack->top = stack->top->next;

    if (free_list->count < STACK_SIZE)
    {
        free_list->addresses[free_list->count++] = temp;
    }
    free(temp);
    return value;
}

void print_stack_list(StackList *stack)
{
    if (is_empty_list(stack))
    {
        printf("Стек пуст\n");
        return;
    }
    printf("Текущий стек (список)\n");
    Node *current = stack->top;
    while (current)
    {
        printf("| %c | %p |\n", current->data, (void *)current);
        current = current->next;
    }
    printf("\n");
}

void print_free_list(FreeList *free_list)
{
    if (free_list->count == 0)
    {
        printf("Список свободных областей пуст\n");
        return;
    }
    printf("Список свободных областей: ");
    for (int i = 0; i < free_list->count; i++)
    {
        printf("|%p|\n", (void *)free_list->addresses[i]);
    }
}

int is_palindrome_list(StackList *stack, FreeList *free_list)
{
    int size = 0;
    char element;
    StackList temp_fill_checking, temp_fill_reserve;
    FreeList temp_free_checking, temp_free_reserve;

    init_stack_list(&temp_fill_checking);
    init_stack_list(&temp_fill_reserve);
    init_free_list(&temp_free_checking);
    init_free_list(&temp_free_reserve);

    while (!is_empty_list(stack))
    {
        size++;
        push_list(&temp_fill_checking, pop_list(stack, free_list));
    }

    while (!is_empty_list(&temp_fill_checking))
    {
        push_list(stack, pop_list(&temp_fill_checking, &temp_free_checking));
    }

    while (!is_empty_list(stack))
    {
        element = pop_list(stack, free_list);
        push_list(&temp_fill_checking, element);
        push_list(&temp_fill_reserve, element);
    }

    while (!is_empty_list(&temp_fill_checking))
    {
        push_list(stack, pop_list(&temp_fill_checking, &temp_free_checking));
    }

    for (size_t i = 0; i < (size / 2); i++)
    {
        push_list(&temp_fill_checking, pop_list(stack, free_list));
    }

    if (size % 2 != 0)
    {
        pop_list(stack, free_list);
    }

    while (!is_empty_list(&temp_fill_checking) || !is_empty_list(stack))
    {
        if (pop_list(&temp_fill_checking, &temp_free_checking) != pop_list(stack, free_list))
        {
            while (!is_empty_list(stack))
            {
                pop_list(stack, free_list);
            }
            for (size_t i = 0; i < size; i++)
            {
                push_list(stack, pop_list(&temp_fill_reserve, &temp_free_reserve));
            }
            free_stack_list(&temp_fill_checking);
            free_stack_list(&temp_fill_reserve);
            return 0;
        }
    }

    for (size_t i = 0; i < size; i++)
    {
        push_list(stack, pop_list(&temp_fill_reserve, &temp_free_reserve));
    }
    free_stack_list(&temp_fill_checking);
    free_stack_list(&temp_fill_reserve);

    return 1;
}
