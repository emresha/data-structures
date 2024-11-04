#include "stack_array.h"
#include "stack_list.h"
#include "timing.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void fill_string(char str[], size_t len)
{
   for (size_t i = 0; i < len - 1; i++)
    {
        str[i] = 'A' + rand() % 26;
    }
   str[len - 1] = '\0';
}

static void generate_palindrome(char *str, size_t len) 
{
    size_t half_length = len / 2;

    for (size_t i = 0; i < half_length; i++) 
    {
        char random_char = 'A' + rand() % 26;
        str[i] = random_char;
        str[len - i - 1] = random_char;
    }

    if (len % 2 != 0) 
    {
        str[half_length] = 'A' + rand() % 26;
    }

    str[len] = '\0';
}


void make_time_calc_array(void)
{
    size_t sizes[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    size_t length = sizeof(sizes) / sizeof(sizes[0]);
    size_t size;

    printf("Запись в стек (массив)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackArray stack_array;
        init_stack_array(&stack_array);
        fill_string(string, size);

        current_time = clock();
        for (size_t j = 0; j < size; j++)
        { 
            push_array(&stack_array, string[j]);
        }
        current_time = clock() - current_time;

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, sizeof(int) + stack_array.top + 1);
        // printf("%ld\n", current_time);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nУдаление из стека (массив)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackArray stack_array;
        init_stack_array(&stack_array);

        fill_string(string, size);


        for (size_t j = 0; j < size; j++)
        {
            push_array(&stack_array, string[j]);
        }

        current_time = clock();
        while(!is_empty_array(&stack_array))
        {
            pop_array(&stack_array);
        }
        current_time = clock() - current_time;

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, sizeof(int) + stack_array.top + 1);
        // printf("%ld\n", current_time);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nПроверка стека на палиндром (строки являются палиндромом) (массив)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackArray stack_array;
        init_stack_array(&stack_array);

        generate_palindrome(string, size);

        for (size_t j = 0; j < size; j++)
        {
            push_array(&stack_array, string[j]);
        }

        current_time = clock();
        int rc = is_palindrome_array(&stack_array);
        current_time = (clock() - current_time);

        if (rc != 1)
        {
            printf("err\n");
        }

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, size + stack_array.top + 1);
        // printf("%ld\n", current_time);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nПроверка стека на палиндром (строки не являются палиндромом) (массив)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackArray stack_array;
        init_stack_array(&stack_array);

        fill_string(string, size);

        for (size_t j = 0; j < size; j++)
        {
            push_array(&stack_array, string[j]);
        }

        current_time = clock();
        int rc = is_palindrome_array(&stack_array);
        current_time += (clock() - current_time);

        if (rc != 0)
        {
            printf("err\n");
        }

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, sizeof(int) + stack_array.top + 1);
        // printf("%ld\n", current_time);
        // printf("%zu\n", i);
        // break;
    }
    
}

void make_time_calc_list(void)
{
    size_t sizes[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    size_t length = sizeof(sizes) / sizeof(sizes[0]);
    size_t size;

    printf("Запись в стек (список)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;

        StackList stack_list;

        init_stack_list(&stack_list);
        fill_string(string, size);

        current_time = clock();
        for (size_t j = 0; j < size; j++)
        { 
            push_list(&stack_list, string[j]);
        }
        current_time = clock() - current_time;

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, 9 * size + sizeof(stack_list));
        // printf("%ld\n", current_time);
        free_stack_list(&stack_list);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nУдаление из стека (список)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackList stack_list;
        FreeList free_list;


        init_stack_list(&stack_list);
        init_free_list(&free_list);

        fill_string(string, size);


        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack_list, string[j]);
        }
        // print_stack_list(&stack_list);

        current_time = clock();
        while(!is_empty_list(&stack_list))
        {
            pop_list(&stack_list, &free_list);
        }
        current_time = clock() - current_time;

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, 9 * size + sizeof(stack_list));
        // printf("%ld\n", current_time);
        free_stack_list(&stack_list);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nПроверка стека на палиндром (строки являются палиндромом) (список)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackList stack_list;
        FreeList free_list;

        init_stack_list(&stack_list);
        init_free_list(&free_list);

        generate_palindrome(string, size);

        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack_list, string[j]);
        }

        current_time = clock();
        int rc = is_palindrome_list(&stack_list, &free_list);
        current_time = (clock() - current_time);

        if (rc != 1)
        {
            printf("err\n");
        }

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, 9 * size + sizeof(stack_list));
        // printf("%ld\n", current_time);
        free_stack_list(&stack_list);
        // printf("%zu\n", i);
        // break;
    }

    printf("\nПроверка стека на палиндром (строки не являются палиндромом) (список)\n");
    printf("|Количество символов в стеке|Время в тиках|Объем стека (байт)|\n");

    for (size_t i = 0; i < length; i++)
    {
        size = sizes[i];
        char string[size];
        clock_t current_time;
        StackList stack_list;
        FreeList free_list;

        init_stack_list(&stack_list);

        fill_string(string, size);

        for (size_t j = 0; j < size; j++)
        {
            push_list(&stack_list, string[j]);
        }
        // printf("ss\n");
        current_time = clock();
        int rc = is_palindrome_list(&stack_list, &free_list);
        current_time += (clock() - current_time);

        if (rc != 0)
        {
            printf("err\n");
        }

        printf("|%-27zu|%-13ld|%-18zu|\n", size, current_time, 9 * size + sizeof(stack_list));
        // printf("%ld\n", current_time);
        free_stack_list(&stack_list);
        // printf("%zu\n", i);
        // break;
    }
    
}
