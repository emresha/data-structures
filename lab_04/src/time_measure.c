#include "stack_array.h"
#include "stack_list.h"
#include "time_measure.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void random_string(char str[], size_t len)
{
    for (size_t i = 0; i < len - 1; i++)
    {
        str[i] = 'A' + rand() % 26;
    }
    str[len - 1] = '\0';
}

static void create_palindrome(char *str, size_t len)
{
    size_t half_len = len / 2;
    for (size_t i = 0; i < half_len; i++)
    {
        char c = 'A' + rand() % 26;
        str[i] = c;
        str[len - i - 1] = c;
    }
    if (len % 2 != 0)
    {
        str[half_len] = 'A' + rand() % 26;
    }
    str[len] = '\0';
}

static void benchmark_stack_array(size_t sizes[], size_t count)
{
    printf("***Операции со стеком, реализованном в виде массива***\n");

    char *texts[] = {
        "Запись в стек",
        "Удаление из стека",
        "Проверка на палиндром (является)",
        "Проверка на палиндром (не является)"};

    printf("\n%s\n", texts[0]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        random_string(str, size);

        start_time = clock();
        for (size_t j = 0; j < size; j++)
        {
            push_array(&stack, str[j]);
        }
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }

    printf("\n%s\n", texts[1]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        random_string(str, size);

        start_time = clock();
        while (!is_empty_array(&stack))
            pop_array(&stack);

        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }

    printf("\n%s\n", texts[2]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        create_palindrome(str, size);

        for (size_t j = 0; j < size; j++)
            push_array(&stack, str[j]);

        start_time = clock();

        is_palindrome_array(&stack);

        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }

    printf("\n%s\n", texts[3]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");

    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackArray stack;
        init_stack_array(&stack);
        create_palindrome(str, size);

        for (size_t j = 0; j < size; j++)
            push_array(&stack, str[j]);

        start_time = clock();

        is_palindrome_array(&stack);

        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
    }
}

static void benchmark_stack_list(size_t sizes[], size_t count)
{
    char *texts[] = {
        "Запись в стек",
        "Удаление из стека",
        "Проверка на палиндром (является)",
        "Проверка на палиндром (не является)"};

    printf("***Операции со стеком, реализованном в виде списка***\n");
    printf("\n%s\n", texts[0]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackList stack;
        init_stack_list(&stack);
        random_string(str, size);

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
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackList stack;
        FreeList free_list;
        init_stack_list(&stack);
        init_free_list(&free_list);
        random_string(str, size);

        start_time = clock();
        while (!is_empty_list(&stack))
        {
            pop_list(&stack, &free_list);
        }
        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
        free_stack_list(&stack);
    }

    printf("\n%s\n", texts[2]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");
    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackList stack;
        FreeList free_list;
        init_stack_list(&stack);
        init_free_list(&free_list);
        create_palindrome(str, size);

        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack, str[j]);
        }

        start_time = clock();

        is_palindrome_list(&stack, &free_list);

        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
        free_stack_list(&stack);
    }

    printf("\n%s\n", texts[3]);
    printf("|Количество символов           |Время в тиках  |Объем стека (в байтах)|\n");

    for (size_t i = 0; i < count; i++)
    {
        size_t size = sizes[i];
        char str[size];
        clock_t start_time;
        StackList stack;
        FreeList free_list;
        init_stack_list(&stack);
        create_palindrome(str, size);

        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack, str[j]);
        }
        start_time = clock();

        is_palindrome_list(&stack, &free_list);

        start_time = clock() - start_time;

        printf("|%-30zu|%-15ld|%-22zu|\n", size, start_time, 9 * size + sizeof(stack));
        free_stack_list(&stack);
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
