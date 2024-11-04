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
        new_node->data = value;
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
    char value = temp->data;
    stack->top = stack->top->next;

    if (free_list->count < STACK_SIZE)
    {
        free_list->addresses[free_list->count++] = temp;
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
    printf("Текущий стек (список)\n");
    Node *current = stack->top;
    while (current)
    {
        printf("| %c | %p |\n", current->data, (void *)current);
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
        printf("|%p|\n", (void *)free_list->addresses[i]);
    }
}

// проверка того, является ли стек палиндромом
int is_palindrome_list(StackList *stack, FreeList *free_list)
{
    int stack_size = 0;
    char element;
    StackList temp_stack_check, temp_stack_restore;
    FreeList temp_free_check, temp_free_restore;

    // инициализация временных стеков и списков
    init_stack_list(&temp_stack_check);
    init_stack_list(&temp_stack_restore);
    init_free_list(&temp_free_check);
    init_free_list(&temp_free_restore);

    // перенос элементов из исходного стека во временный стек для проверки
    while (!is_empty_list(stack))
    {
        stack_size++;
        push_list(&temp_stack_check, pop_list(stack, free_list));
    }

    // восстановление исходного стека
    while (!is_empty_list(&temp_stack_check))
    {
        push_list(stack, pop_list(&temp_stack_check, &temp_free_check));
    }

    // перенос элементов из исходного стека в два временных стека
    while (!is_empty_list(stack))
    {
        element = pop_list(stack, free_list);
        push_list(&temp_stack_check, element);
        push_list(&temp_stack_restore, element);
    }

    // Восстановление исходного стека
    while (!is_empty_list(&temp_stack_check))
    {
        push_list(stack, pop_list(&temp_stack_check, &temp_free_check));
    }

    // перенос половины элементов из исходного стека во временный стек для проверки
    for (size_t i = 0; i < (stack_size / 2); i++)
    {
        push_list(&temp_stack_check, pop_list(stack, free_list));
    }

    // если размер стека нечетный, пропускаем средний элемент
    if (stack_size % 2 != 0)
    {
        pop_list(stack, free_list);
    }

    // сравнение элементов из двух временных стеков
    while (!is_empty_list(&temp_stack_check) || !is_empty_list(stack))
    {
        if (pop_list(&temp_stack_check, &temp_free_check) != pop_list(stack, free_list))
        {
            // ели элементы не совпадают, восстанавливаем исходный стек и возвращаем 0
            while (!is_empty_list(stack))
            {
                pop_list(stack, free_list);
            }
            for (size_t i = 0; i < stack_size; i++)
            {
                push_list(stack, pop_list(&temp_stack_restore, &temp_free_restore));
            }
            free_stack_list(&temp_stack_check);
            free_stack_list(&temp_stack_restore);
            return 0;
        }
    }

    // восстановление исходного стека
    for (size_t i = 0; i < stack_size; i++)
    {
        push_list(stack, pop_list(&temp_stack_restore, &temp_free_restore));
    }
    free_stack_list(&temp_stack_check);
    free_stack_list(&temp_stack_restore);

    return 1;
}
