#include "stack_array.h"
#include "stack_list.h"
#include <string.h>
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

int input_single_character(char *value)
{
    printf("Введите элемент для добавления: ");
    char buf[3];

    fgets(buf, 3, stdin);
    sscanf("%s", buf);

    // printf("\'%s\'\n", buffer);

    if (!strchr(buf, '\n'))
    {
        return 1;
    }
    
    *value = buf[0];
    return 0;
}

void print_menu(void)
{
    printf("Выберите функцию или способ реализации стека:\n");
    printf("1. С помощью статического массива\n");
    printf("2. С помощью односвязного списка\n");
    printf("3. Выполнить замерный эксперимент\n");
    printf("0. Выйти\n");
    printf("Ввод: ");
}

int main(void)
{
    StackArray stack_array;
    StackList stack_list;
    FreeList free_list;
    int choice, sub_choice = -1;
    char value;

    printf("Данная программа создана для работы со стеком и проверки строки, находящейся в нём на то, является ли она палиндромом.\nВ стек строку можно добавлять только посимвольно.\nМаксимальный размер стека, реализованного с помощью статического массива -- тысяча элементов.\n\n");

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
        init_stack_array(&stack_array);
    }
    else if (choice == 2)
    {
        init_stack_list(&stack_list);
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
        printf("1. Добавить элемент в стек\n");
        printf("2. Удалить элемент из стека\n");
        printf("3. Вывести стек\n");
        printf("4. Проверить текущую строку в стеке на палиндромю\n");

        if (choice == 2)
        {
            printf("5. Показать список свободных узлов.\n");
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
            while (input_single_character(&value) != 0)
            {
                printf("Неверный ввод.\n");
                value = 0;
                clean_input();
            }
            if (choice == 1)
            {
                push_array(&stack_array, value);
            }
            else
            {
                push_list(&stack_list, value);
            }
            break;

        case 2:
            if (choice == 1)
            {
                value = pop_array(&stack_array);
            }
            else
            {
                value = pop_list(&stack_list, &free_list);
            }
            if (value != '\0')
            {
                printf("Удален элемент: %c\n", value);
            }
            else
            {
                printf("Ошибка: пустой стек.\n");
            }
            break;

        case 3:
            if (choice == 1)
            {
                print_stack_array(&stack_array);
            }
            else
            {
                print_stack_list(&stack_list);
            }
            break;

        case 4:
            if (choice == 1)
            {
                if (is_empty_array(&stack_array))
                {
                    printf("Стек пуст.\n");
                    break;
                }

                if (is_palindrome_array(&stack_array))
                {
                    printf("Стек является палиндромом.\n");
                }
                else
                {
                    printf("Стек не является палиндромом.\n");
                }
            }
            else
            {
                if (is_empty_list(&stack_list))
                {
                    printf("Стек пуст.\n");
                    break;
                }

                if (is_palindrome_list(&stack_list, &free_list))
                {
                    printf("Стек является палиндромом.\n");
                }
                else
                {
                    printf("Стек не является палиндромом.\n");
                }
            }
            break;

        case 5:
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
                free_stack_list(&stack_list);
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
