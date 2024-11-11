#include "stack_list.h"
#include <stdio.h>
#include <stdlib.h>

// свобождение памяти, занятой стеком
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

int peek_list(StackList *stack)
{
    if (is_empty_list(stack))
        return 0;

    return stack->top->symbol;
}

// инициализация стека
void init_stack_list(StackList *stack)
{
    stack->top = NULL;
}

// инициализация списка свободных областей
void init_free_list(FreeList *free_list)
{
    free_list->count = 0;
}

// проверка, пуст ли стек
int is_empty_list(StackList *stack)
{
    return stack->top == NULL;
}

// дбавление элемента в стек
void push_list(StackList *stack, char value)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node)
    {
        new_node->symbol = value;
        new_node->next = stack->top;
        stack->top = new_node;
    }
}

// извлечение элемента из стека
char pop_list(StackList *stack, FreeList *free_list)
{
    if (is_empty_list(stack))
        return '\0';

    Node *temp = stack->top;
    char value = temp->symbol;
    stack->top = stack->top->next;

    if (free_list->count < MAX_SIZE)
    {
        free_list->pointers[free_list->count++] = temp;
    }
    free(temp);
    return value;
}

// печать содержимого стека
void print_stack_list(StackList *stack)
{
    if (is_empty_list(stack))
    {
        printf("Стек пуст\n");
        return;
    }
    printf("Текущий стек\n");
    Node *current = stack->top;
    while (current)
    {
        printf("\'%d\': %p\n", current->symbol, (void *)current);
        current = current->next;
    }
    printf("\n");
}

// печать списка свободных областей
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
        printf("%p\n", (void *)free_list->pointers[i]);
    }
}

void sort_stack_list(StackList *stack1, StackList *stack2, StackList *sorted_stack, FreeList *free_list)
{
    StackList temp_stack;
    FreeList temp_free_list;

    init_stack_list(sorted_stack);
    init_stack_list(&temp_stack);
    init_free_list(&temp_free_list);

    while (!is_empty_list(stack1))
    {
        push_list(sorted_stack, pop_list(stack1, free_list));
    }
    while (!is_empty_list(stack2))
    {
        push_list(sorted_stack, pop_list(stack2, free_list));
    }

    while (!is_empty_list(sorted_stack))
    {
        int temp = pop_list(sorted_stack, free_list);

        while (!is_empty_list(&temp_stack) && peek_list(&temp_stack) > temp)
        {
            push_list(sorted_stack, pop_list(&temp_stack, &temp_free_list));
        }

        push_list(&temp_stack, temp);
    }

    while (!is_empty_list(&temp_stack))
    {
        push_list(sorted_stack, pop_list(&temp_stack, &temp_free_list));
    }

    free_stack_list(&temp_stack);
}