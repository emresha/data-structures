#include "stack_array.h"
#include "stack_list.h"
#include "time_measure.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void random_nums(int *digits, size_t len)
{
    for (size_t i = 0; i < len - 1; i++)
    {
        digits[i] = rand() % 100;
    }
}

static void benchmark_stack_array(size_t sizes[], size_t count)
{
    printf("Операции со стеком, реализованном в виде массива\n");

    char *texts[] = {
        "Запись в стек",
        "Удаление из стека",
        "Сортировка стека"};

    printf("\n%s\n", texts[0]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        random_nums(str, size);

        start_time = clock();
        for (size_t j = 0; j < size; j++)
        {
            push_array(&stack, str[j]);
        }
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }

    printf("\n%s\n", texts[1]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        random_nums(str, size);

        start_time = clock();
        while (!is_empty_array(&stack))
            pop_array(&stack);
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }

    printf("\n%s\n", texts[2]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackArray stack1, stack2, sorted_stack;
        init_stack_array(&stack1);
        init_stack_array(&stack2);
        init_stack_array(&sorted_stack);
        random_nums(str, size);

        for (size_t j = 0; j < size; j++)
            push_array(&stack1, str[j]);

        random_nums(str, size);

        for (size_t j = 0; j < size; j++)
            push_array(&stack2, str[j]);

        start_time = clock();
        sort_stacks(&stack1, &stack2, &sorted_stack);
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack1));
    }
}

static void benchmark_stack_list(size_t sizes[], size_t count)
{
    char *texts[] = {
        "Запись в стек",
        "Удаление из стека",
        "Сортировка стека"};

    printf("Операции со стеком, реализованном в виде списка\n");
    printf("\n%s\n", texts[0]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackList stack;
        init_stack_list(&stack);
        random_nums(str, size);

        start_time = clock();
        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack, str[j]);
        }
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
        free_stack_list(&stack);
    }

    printf("\n%s\n", texts[1]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackList stack;
        FreeList free_list;
        init_stack_list(&stack);
        init_free_list(&free_list);
        random_nums(str, size);

        start_time = clock();
        while (!is_empty_list(&stack))
            pop_list(&stack, &free_list);
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
        free_stack_list(&stack);
    }

    printf("\n%s\n", texts[2]);
    printf("|Количество элементов          |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        int str[size];
        clock_t start_time;
        StackList stack1, stack2, sorted_stack;
        FreeList free_list;
        init_stack_list(&stack1);
        init_stack_list(&stack2);
        init_stack_list(&sorted_stack);
        init_free_list(&free_list);
        random_nums(str, size);

        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack1, str[j]);
        }

        random_nums(str, size);
        
        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack2, str[j]);
        }

        start_time = clock();
        sort_stack_list(&stack1, &stack2, &sorted_stack, &free_list);
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack1));
        free_stack_list(&stack1);
    }
}

void run_array_timing(void)
{
    size_t sizes[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    size_t count = sizeof(sizes) / sizeof(sizes[0]);
    benchmark_stack_array(sizes, count);
}

void run_list_timing(void)
{
    size_t sizes[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    size_t count = sizeof(sizes) / sizeof(sizes[0]);
    benchmark_stack_list(sizes, count);
}
