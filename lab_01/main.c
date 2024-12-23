#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_MANTISSA_DIGITS 40
#define MAX_INT_DIGITS 30
#define MAX_RES_DIGITS 30
#define MAX_INIT_RES_DIGITS 70
#define ERR_OK 0
#define ERR_IO 1
#define ERR_RANGE 2

typedef struct
{
    char sign;
    char mantissa[MAX_MANTISSA_DIGITS + 1];
    int exponent;
}
big_float_t;

// Функция для преобразования мантиссы в целое число
void convert_mantissa_to_int(const char *mantissa, char *int_mantissa, int *decimal_pos)
{
    int len = strlen(mantissa);
    int pos = 0;
    *decimal_pos = 0;

    for (int i = 0; i < len; i++)
    {
        if (mantissa[i] == '.')
        {
            *decimal_pos = len - i - 1;
            continue;
        }
        int_mantissa[pos++] = mantissa[i];
    }
    int_mantissa[pos] = '\0';
}

// Функция для умножения больших чисел
int multiply_big_numbers(const char *mantissa, char *big_int, char *result, int *decimal_shift)
{
    char int_mantissa[MAX_MANTISSA_DIGITS + 1];
    convert_mantissa_to_int(mantissa, int_mantissa, decimal_shift);

    int len_mantissa = strlen(int_mantissa);
    int len_big_int = strlen(big_int);

    int len_result = len_mantissa + len_big_int;

    int temp_result[MAX_INIT_RES_DIGITS] = {0};

    for (int i = len_mantissa - 1; i >= 0; i--)
    {
        if (!isdigit(int_mantissa[i]))
            return ERR_IO;
        for (int j = len_big_int - 1; j >= 0; j--)
        {
            if (!isdigit(big_int[j]))
                return ERR_IO;
            int product = (int_mantissa[i] - '0') * (big_int[j] - '0');
            int pos = i + j + 1;
            temp_result[pos] += product;

            temp_result[pos - 1] += temp_result[pos] / 10;
            temp_result[pos] %= 10;
        }
    }

    int index = 0;
    int leading_zero = 1;
    for (int i = 0; i < len_result; i++)
    {
        if (temp_result[i] == 0 && leading_zero)
        {
            continue;
        }
        leading_zero = 0;
        result[index++] = temp_result[i] + '0';
    }

    if (index == 0)
    {
        result[0] = '0';
        result[1] = '\0';
    }
    else
        result[index] = '\0';

    return ERR_OK;
}

int multiply(big_float_t *number, char *big_int, big_float_t *result, bool debug)
{
    if (strlen(big_int) > MAX_INT_DIGITS)
    {
        return ERR_RANGE;
    }

    result->sign = (number->sign == '-') ? '-' : '+';

    if (big_int[0] == '-' || big_int[0] == '+')
    {
        if (big_int[0] == '-')
            result->sign = (result->sign == '-') ? '+' : '-';

        for (int i = 0; big_int[i] != '\0'; i++)
        {
            big_int[i] = big_int[i + 1];
        }
    }

    char mult_result[MAX_INIT_RES_DIGITS + 1];
    int decimal_shift;

    int ret = multiply_big_numbers(number->mantissa, big_int, mult_result, &decimal_shift);
    if (ret != ERR_OK)
        return ret;

    result->exponent = number->exponent - decimal_shift;

    int result_len = (int)strlen(mult_result);

    if (debug)
        printf("mult_result: %s\n", mult_result);

    if (result_len > MAX_RES_DIGITS)
    {
        char rounding_digit = mult_result[MAX_RES_DIGITS];

        mult_result[MAX_RES_DIGITS] = '\0';

        if (rounding_digit >= '5')
        {
            int carry = 1;
            for (int i = MAX_RES_DIGITS - 1; i >= 0 && carry; i--)
            {
                if (mult_result[i] < '9')
                {
                    mult_result[i]++;
                    carry = 0;
                }
                else
                {
                    mult_result[i] = '0';
                }
            }
            if (carry)
            {
                memmove(&mult_result[1], mult_result, MAX_RES_DIGITS);
                mult_result[0] = '1';
                result->exponent++;
            }
        }

        result->exponent += (result_len - MAX_RES_DIGITS);
    }
    else
    {
        result->exponent += (result_len - strlen(mult_result));
    }

    strcpy(result->mantissa, mult_result);

    return ERR_OK;
}


int check_str(char *str)
{
    int rc = ERR_OK;

    for (size_t i = 0; i < strlen(str); i++)
    {
        if (!isdigit((unsigned char)str[i]))
            return ERR_IO;
    }

    return rc;
}

// Функция для ввода большого числа с плавающей запятой
int input_big_float(big_float_t *number)
{
    char input[100];
    char exp_sign;
    int exponent;
    char exponent_str[100];

    printf("Суммарная длина мантиссы (m+n) должна быть до 40 значащих цифр,\nа величина порядка K - до 5 цифр,\nцелое число должно быть длиной до 30 десятичных цифр.\n");
    printf("Результат выдаётся в форме ±0.m1 Е ±K1, где m1 - до 30 значащих цифр, а K1 - до 5 цифр.\n");
    printf("                                   1---5---10---15---20---25---30---35---40---45---50\n");
    printf("Введите число в формате ±m.n E ±K: ");

    if (fgets(input, sizeof(input), stdin) == NULL)
        return ERR_IO;

    int parsed = sscanf(input, "%c%40s E %c%99s", &number->sign, number->mantissa, &exp_sign, exponent_str);

    if (check_str(exponent_str) || strlen(exponent_str) > 6)
        return ERR_IO;

    exponent = atoi(exponent_str);

    if (parsed != 4)
        return ERR_IO;

    if (strchr(number->mantissa, '.'))
    {
        if (strlen(number->mantissa) > MAX_MANTISSA_DIGITS + 1)
            return ERR_RANGE;
    }
    else
    {
        if (strlen(number->mantissa) > MAX_MANTISSA_DIGITS)
            return ERR_RANGE;
    }

    if (number->sign != '+' && number->sign != '-')
        return ERR_IO;

    if (exp_sign == '-')
        exponent = -exponent;
    number->exponent = exponent;

    if (abs(number->exponent) > 99999)
        return ERR_RANGE;

    return ERR_OK;
}

// Функция для ввода большого целого числа
int input_big_int(char *big_int)
{
    // для ввода большого целого числа используется строка размером 100 символов
    char input[100];
    printf("                                1---5---10---15---20---25---30---35\n");
    printf("Введите целое число до 30 цифр: ");

    if (fgets(input, sizeof(input), stdin) == NULL)
        return ERR_IO;

    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) > MAX_INT_DIGITS || strlen(input) == 0)
        return ERR_RANGE;

    for (int i = 0; i < strlen(input); i++)
    {
        if (!isdigit(input[i]) && input[i] != '-' && input[i] != '+')
            return ERR_IO;
    }

    strcpy(big_int, input);
    return ERR_OK;
}

// Функция для удаления нулей из мантиссы
void delete_zeros(big_float_t *result)
{
    int len = strlen(result->mantissa);
    int zeros = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        if (result->mantissa[i] == '0')
            zeros++;
        else
            break;
    }
    result->mantissa[len - zeros] = '\0';
    result->exponent += zeros;
}

void delete_zeros_char(char *str)
{
    int len = strlen(str);
    int zeros = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        if (str[i] == '0')
            zeros++;
        else
            break;
    }

    str[len - zeros] = 0;
}

int main(int argc, char **argv)
{
    big_float_t number, result;
    char big_int[MAX_INT_DIGITS + 1];

    printf("Программа выполнения умножения вещественного и целого числа,\nвыходящего за разрядную сетку компьютера\n");

    int ret = input_big_float(&number);
    if (ret != ERR_OK)
    {
        if (ret == ERR_IO)
            printf("Ошибка ввода числа: неверный формат или недопустимые символы.\n");
        else if (ret == ERR_RANGE)
            printf("Ошибка ввода числа: превышена допустимая длина мантиссы или экспоненты.\n");
        return ret;
    }

    ret = input_big_int(big_int);
    if (ret != ERR_OK)
    {
        if (ret == ERR_IO)
            printf("Ошибка ввода целого числа: неверный формат или недопустимые символы.\n");
        else if (ret == ERR_RANGE)
            printf("Ошибка ввода целого числа: превышена допустимая длина.\n");
        return ret;
    }


    if (argc == 2 && strcmp(argv[1], "d") == 0)
        ret = multiply(&number, big_int, &result, true);
    else
        ret = multiply(&number, big_int, &result, false);
    if (ret != ERR_OK)
    {
        if (ret == ERR_IO)
            printf("Ошибка умножения: неверный формат мантиссы или целого числа.\n");
        else if (ret == ERR_RANGE)
            printf("Ошибка умножения: результат превышает допустимую длину.\n");
        return ret;
    }

    delete_zeros(&result);

    if (result.mantissa[0] == '\0')
    {
        printf("Результат: +0.0 E +0\n");
        return ERR_OK;
    }

    int add_exp;

    // printf("%d\n", result.exponent);

    if (result.exponent >= 99999 || result.exponent < -100000)
    {
        printf("Ошибка умножения: экспонента превышает допустимый диапазон.\n");
        return ERR_RANGE;
    }

    if (strchr(result.mantissa, '.') == NULL)
    {
        add_exp = 0;
    }
    else
    {
        add_exp = -1;
    }

    char exp_sign = (result.exponent < 0) ? '\0' : '+';

    printf("Результат: %c0.%s E %c%d\n", result.sign, result.mantissa, exp_sign, result.exponent + (int)strlen(result.mantissa) + add_exp);

    return ERR_OK;
}
