#include "stack_array.h"
#include "stack_list.h"
#include "time_measure.h"
#include <stdio.h>

void clean_input(void)
{
    int c;
    do
    {
        c = getchar();
    }
    while (c != '\n' && c != EOF);
}

int input_single_integer(int *value)
{
    printf("Введите целое число для добавления: ");
    return scanf("%d", value) != 1;
}

void print_menu(void)
{
    printf("Выберите способ реализации стека:\n");
    printf("1. С помощью статического массива\n");
    printf("2. С помощью односвязного списка\n");
    printf("3. Выполнить замерный эксперимент\n");
    printf("0. Выйти\n");
    printf("Ввод: ");
}

int main(void)
{
    StackArray stack_array1, stack_array2, sorted_stack_array;
    StackList stack_list1, stack_list2, sorted_stack_list;
    FreeList free_list;
    int choice, sub_choice = -1;
    int value;

    printf("Данная программа позволяет сортировать целые числа, добавленные в два стека.\n\n");
    print_menu();

    while (scanf("%d", &choice) != 1 || choice < 0 || choice > 3)
    {
        clean_input();
        printf("Некорректный ввод.\n");
        clean_input();
        print_menu();
    }

    if (choice == 1)
    {
        init_stack_array(&stack_array1);
        init_stack_array(&stack_array2);
    }
    else if (choice == 2)
    {
        init_stack_list(&stack_list1);
        init_stack_list(&stack_list2);
        init_free_list(&free_list);
    }
    else if (choice == 3)
    {
        run_array_timing();
        run_list_timing();
        return 0;
    }
    else
    {
        return 0;
    }

    while (sub_choice != 0)
    {
        printf("\nВыберите действие:\n");
        printf("1. Добавить элемент в первый стек\n");
        printf("2. Добавить элемент во второй стек\n");
        printf("3. Удалить элемент из первого стека\n");
        printf("4. Удалить элемент из второго стека\n");
        printf("5. Вывести оба стека\n");
        printf("6. Выполнить сортировку\n");

        if (choice == 2)
        {
            printf("7. Показать список свободных узлов.\n");
        }

        printf("0. Выйти\n");
        printf("Ввод: ");

        if (scanf("%d", &sub_choice) != 1)
        {
            clean_input();
            sub_choice = -1;
        }
        printf("\n");

        switch (sub_choice)
        {
        case 1:
            clean_input();
            while (input_single_integer(&value) != 0)
            {
                printf("Неверный ввод.\n");
                value = 0;
                clean_input();
            }
            if (choice == 1)
            {
                push_array(&stack_array1, value);
            }
            else
            {
                push_list(&stack_list1, value);
            }
            break;

        case 2:
            clean_input();
            while (input_single_integer(&value) != 0)
            {
                printf("Неверный ввод.\n");
                value = 0;
                clean_input();
            }
            if (choice == 1)
            {
                push_array(&stack_array2, value);
            }
            else
            {
                push_list(&stack_list2, value);
            }
            break;

        case 3:
            if (choice == 1)
            {
                value = pop_array(&stack_array1);
            }
            else
            {
                value = pop_list(&stack_list1, &free_list);
            }
            if (value != 0)
            {
                printf("Удален элемент: %d\n", value);
            }
            else
            {
                printf("Ошибка: пустой стек.\n");
            }
            break;

        case 4:
            if (choice == 1)
            {
                value = pop_array(&stack_array2);
            }
            else
            {
                value = pop_list(&stack_list2, &free_list);
            }
            if (value != 0)
            {
                printf("Удален элемент: %d\n", value);
            }
            else
            {
                printf("Ошибка: пустой стек.\n");
            }
            break;

        case 5:
            if (choice == 1)
            {
                printf("Первый стек:\n");
                print_stack_array(&stack_array1);
                printf("Второй стек:\n");
                print_stack_array(&stack_array2);
            }
            else
            {
                printf("Первый стек:\n");
                print_stack_list(&stack_list1);
                printf("Второй стек:\n");
                print_stack_list(&stack_list2);
            }
            break;

        case 6:
            if (choice == 1)
            {
                init_stack_array(&sorted_stack_array);
                sort_stacks(&stack_array1, &stack_array2, &sorted_stack_array);
                printf("Отсортированный стек:\n");
                print_stack_array(&sorted_stack_array);
            }
            else
            {
                init_stack_list(&sorted_stack_list);
                sort_stack_list(&stack_list1, &stack_list2, &sorted_stack_list, &free_list);
                printf("Отсортированный стек:\n");
                print_stack_list(&sorted_stack_list);
            }
            break;

        case 7:
            if (choice == 2)
            {
                print_free_list(&free_list);
            }
            else
            {
                printf("Некорректное действие.\n");
                clean_input();
            }
            break;

        case 0:
            if (choice == 2)
            {
                free_stack_list(&stack_list1);
                free_stack_list(&stack_list2);
            }
            break;

        default:
            printf("Некорректное действие.\n");
            clean_input();
            break;
        }
    }

    return 0;
}
