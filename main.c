#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_MANTISSA_DIGITS 40
#define MAX_INT_DIGITS 30
#define ERR_OK 0
#define ERR_IO 1
#define ERR_RANGE 2

// структура большого вещественного числа
typedef struct
{
    char sign;
    char mantissa[MAX_MANTISSA_DIGITS + 1];
    int exponent;
} big_float_t;

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

int multiply_big_numbers(const char *mantissa, const char *big_int, char *result, int *decimal_shift)
{
    char int_mantissa[MAX_MANTISSA_DIGITS + 1];
    convert_mantissa_to_int(mantissa, int_mantissa, decimal_shift);

    int len_mantissa = strlen(int_mantissa);
    int len_big_int = strlen(big_int);
    int len_result = len_mantissa + len_big_int;

    int temp_result[MAX_MANTISSA_DIGITS + MAX_INT_DIGITS] = {0};

    // столбик
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

int multiply(big_float_t *number, const char *big_int, big_float_t *result)
{
    if (strlen(big_int) > MAX_INT_DIGITS)
    {
        return ERR_RANGE;
    }

    result->sign = (number->sign == '-') ? '-' : '+';

    char mult_result[MAX_MANTISSA_DIGITS + MAX_INT_DIGITS + 1];
    int decimal_shift;

    int ret = multiply_big_numbers(number->mantissa, big_int, mult_result, &decimal_shift);
    // printf("%s, %d, %d\n", mult_result, decimal_shift, number->exponent);
    if (ret != ERR_OK)
    {
        return ret;
    }

    // здесь подсчитывается экспонента
    result->exponent = number->exponent - decimal_shift + (int)strlen(mult_result);

    // printf("%zu\n", number->exponent - decimal_shift + (int)strlen(mult_result));

    // результат с точностью до 30 цифр, так что завершаем тут строку
    mult_result[MAX_INT_DIGITS + 1] = '\0';
    strcpy(result->mantissa, mult_result);

    return ERR_OK;
}

int input_big_float(big_float_t *number)
{
    printf("Введите число в формате ±m.n E ±K: ");
    if (scanf(" %c%s E %d", &number->sign, number->mantissa, &number->exponent) != 3)
    {
        return ERR_IO;
    }

    if ((number->sign != '+' && number->sign != '-') || strlen(number->mantissa) > MAX_MANTISSA_DIGITS)
    {
        return ERR_RANGE;
    }

    return ERR_OK;
}

int input_big_int(char *big_int)
{
    printf("Введите целое число до 30 цифр: ");
    if (scanf("%s", big_int) != 1)
    {
        return ERR_IO;
    }

    if (strlen(big_int) > MAX_INT_DIGITS)
    {
        return ERR_RANGE;
    }

    return ERR_OK;
}

int main(void)
{
    big_float_t number, result;
    char big_int[MAX_INT_DIGITS + 1];

    int ret = input_big_float(&number);
    if (ret != ERR_OK)
    {
        printf("Ошибка ввода числа. Код ошибки: %d\n", ret);
        return ret;
    }

    ret = input_big_int(big_int);
    if (ret != ERR_OK)
    {
        printf("Ошибка ввода целого числа. Код ошибки: %d\n", ret);
        return ret;
    }

    ret = multiply(&number, big_int, &result);
    if (ret != ERR_OK)
    {
        printf("Ошибка умножения. Код ошибки: %d\n", ret);
        return ret;
    }

    int new_exp = result.exponent;

    printf("Результат: %c0.%s E %d\n", result.sign, result.mantissa, new_exp);

    return ERR_OK;
}
