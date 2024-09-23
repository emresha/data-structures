#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_MANTISSA_DIGITS 40
#define MAX_INT_DIGITS 30
#define ERR_OK 0
#define ERR_IO 1
#define ERR_RANGE 2

// структура для хранения числа с плавающей точкой
// sign - знак числа
// mantissa - мантисса числа
// exponent - показатель степени
typedef struct
{
    char sign;
    char mantissa[MAX_MANTISSA_DIGITS + 1];
    int exponent;
} big_float_t;

// преобразование мантиссы числа с плавающей точкой в целое число
// mantissa - мантисса числа
// int_mantissa - целая часть мантиссы
// decimal_pos - позиция десятичной точки
void convert_mantissa_to_int(const char *mantissa, char *int_mantissa, int *decimal_pos)
{
    int len = strlen(mantissa);
    int pos = 0;
    *decimal_pos = 0;

    // убираем десятичную точку
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

// умножение мантиссы на целое число
// mantissa - мантисса числа
// big_int - целое число
// result - результат умножения
// decimal_shift - сдвиг десятичной точки в результате
int multiply_big_numbers(const char *mantissa, char *big_int, char *result, int *decimal_shift)
{
    char int_mantissa[MAX_MANTISSA_DIGITS + 1];
    convert_mantissa_to_int(mantissa, int_mantissa, decimal_shift);

    int len_mantissa = strlen(int_mantissa);
    int len_big_int = strlen(big_int);

    int len_result = len_mantissa + len_big_int;

    // результат умножения не может быть больше MAX_MANTISSA_DIGITS
    int temp_result[MAX_MANTISSA_DIGITS + MAX_INT_DIGITS] = {0};

    // умножение столбиком
    for (int i = len_mantissa - 1; i >= 0; i--)
    {
        if (!isdigit(int_mantissa[i]))
            return ERR_IO;
        for (int j = len_big_int - 1; j >= 0; j--)
        {
            // printf("%c\n", big_int[j]);
            if (!isdigit(big_int[j]))
                return ERR_IO;
            // умножение цифр
            int product = (int_mantissa[i] - '0') * (big_int[j] - '0');
            int pos = i + j + 1;
            temp_result[pos] += product;

            // перенос разряда
            temp_result[pos - 1] += temp_result[pos] / 10;
            temp_result[pos] %= 10;
        }
    }

    // приведение результата к строке
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

    // если результат равен нулю
    if (index == 0)
    {
        result[0] = '0';
        result[1] = '\0';
    }
    else
        result[index] = '\0';

    return ERR_OK;
}

// умножение числа с плавающей точкой на целое число
// number - число с плавающей точкой
// big_int - целое число
// result - результат умножения
int multiply(big_float_t *number, char *big_int, big_float_t *result)
{
    // проверка на допустимое количество цифр в целом числе
    if (strlen(big_int) > MAX_INT_DIGITS)
    {
        return ERR_RANGE;
    }

    // установка знака результата
    result->sign = (number->sign == '-') ? '-' : '+';

    // установка знака результата от знака целого числа
    if (big_int[0] == '-' || big_int[0] == '+')
    {
        if (big_int[0] == '-')
            result->sign = (result->sign == '-') ? '+' : '-';
        
        for (int i = 0; i < strlen(big_int); i++)
        {
            big_int[i] = big_int[i + 1];
        }

        big_int[strlen(big_int)] = '\0';
        // printf("\'%s\'\n", big_int);
    }


    char mult_result[MAX_MANTISSA_DIGITS + MAX_INT_DIGITS + 1];
    int decimal_shift;

    // умножение мантиссы на целое число
    int ret = multiply_big_numbers(number->mantissa, big_int, mult_result, &decimal_shift);
    // printf("%s, %d, %d\n", mult_result, decimal_shift, number->exponent);
    if (ret != ERR_OK)
    {
        return ret;
    }

    result->exponent = number->exponent - decimal_shift;
    strcpy(result->mantissa, mult_result);

    return ERR_OK;
}

// ввод числа с плавающей точкой
// number - число с плавающей точкой
int input_big_float(big_float_t *number)
{
    char exp_sign;
    printf("Введите число в формате ±m.n E ±K: ");
    // считывание числа в формате ±m.n E ±K
    if (scanf(" %c%s E %c%u", &number->sign, number->mantissa, &exp_sign, &number->exponent) != 4)
        return ERR_IO;

    if (exp_sign != '-' && exp_sign != '+')
        return ERR_IO;

    if (exp_sign == '-')
        number->exponent = -number->exponent;

    // проверка введенного числа
    if ((number->sign != '+' && number->sign != '-') || strlen(number->mantissa) > MAX_MANTISSA_DIGITS)
        return ERR_RANGE;

    return ERR_OK;
}

// ввод целого числа
int input_big_int(char *big_int)
{
    printf("Введите целое число до 30 цифр: ");
    // считывание целого числа
    if (scanf("%s", big_int) != 1)
    {
        return ERR_IO;
    }

    // проверка введенного числа
    if (strlen(big_int) > MAX_INT_DIGITS)
    {
        return ERR_RANGE;
    }

    return ERR_OK;
}

// удаление нулей в конце числа и перенос их в показатель степени
// result - результат умножения
void delete_zeros(big_float_t *result)
{
    int len = strlen(result->mantissa);
    int zeros = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        if (result->mantissa[i] == '0')
        {
            zeros++;
        }
        else
        {
            break;
        }
    }
    result->mantissa[len - zeros] = '\0';
    result->exponent += zeros;
}

int main(void)
{
    big_float_t number, result;
    char big_int[MAX_INT_DIGITS + 1];

    // ввод числа с плавающей точкой и целого числа
    int ret = input_big_float(&number);
    if (ret != ERR_OK)
    {
        printf("Ошибка ввода числа. Код ошибки: %d\n", ret);
        return ret;
    }

    // ввод целого числа
    ret = input_big_int(big_int);
    if (ret != ERR_OK)
    {
        printf("Ошибка ввода целого числа. Код ошибки: %d\n", ret);
        return ret;
    }

    // умножение числа с плавающей точкой на целое число
    ret = multiply(&number, big_int, &result);
    if (ret != ERR_OK)
    {
        printf("Ошибка умножения. Код ошибки: %d\n", ret);
        return ret;
    }

    // удаление нулей в конце числа и перенос их в показатель степени
    delete_zeros(&result);

    if (result.mantissa[0] == '\0')
    {
        printf("Результат: +0.0 E 0\n");
        return ERR_OK;
    }

    int add_exp;

    if (strchr(result.mantissa, '.') == NULL)
    {
        add_exp = 0;
    }
    else
    {
        add_exp = -1;
    }

    // printf("\'%s\', %d %d %zu\n", result.mantissa, result.exponent, add_exp, strlen(result.mantissa));

    char exp_sign = (result.exponent < 0) ? '\0' : '+';

    if (strlen(result.mantissa) > 35)
    {
        result.mantissa[35] = '\0';
    }

    printf("Результат: %c0.%s E %c%d\n", result.sign, result.mantissa, exp_sign, result.exponent + (int)strlen(result.mantissa) + add_exp);

    return ERR_OK;
}
