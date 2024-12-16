#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

#define ERR_OK 0
#define ERR_IO 1
#define ERR_RANGE 2

#define ERR_MEM 3
#define ERR_MENU 4
#define ERR_CANNOT_MULTIPLY 5

#define N 100

#define INPUT_BUFFER_SIZE 100

typedef struct
{
    size_t rows, cols, cnt_non_zeros;
    int *A, *IA, *JA;
} sp_matrix_t;

typedef struct {
    size_t cols, cnt_non_zeros;
    int *B, *JB;
} sp_vector_t;

int sp_matrix_alloc(sp_matrix_t *matrix, size_t rows, size_t cols, size_t cnt_non_zeros);
void sp_matrix_free(sp_matrix_t *matrix);
int sp_vector_alloc(sp_vector_t *vector, size_t cols, size_t cnt_non_zeros);
void sp_vector_free(sp_vector_t *vector);

typedef enum
{
    exit_menu,
    read_vector, 
    read_matrix,
    print_vector,
    print_matrix,
    multiply_vector_on_matrix,
    multiply_vector_on_matrix_full,
    compare_multiplication_algorithms
} menu_action_t;

void print_err(int rc);
void print_menu(void);
int read_menu_action(menu_action_t *action);
int do_menu_action(menu_action_t action, sp_matrix_t *matrix, sp_vector_t *vector);

#define PRINTABLE_ROWS 30
#define PRINTABLE_COLS 30
#define MAX_ROWS 1000
#define MAX_COLS 1000
#define MAX_CNT_NON_ZEROS_MATRIX 1000
#define MAX_CNT_NON_ZEROS_VECTOR 100
#define BASE 10

int read_vector_func(sp_vector_t *vector);
int read_matrix_func(sp_matrix_t *matrix);
int print_vector_func(const sp_vector_t *vector);
int print_matrix_func(const sp_matrix_t *matrix);
int multiply_vector_on_matrix_func(const sp_vector_t *vector, const sp_matrix_t *matrix, sp_vector_t *result, double *time);
int multiply_vector_on_matrix_full_func(const sp_vector_t *vector, const sp_matrix_t *matrix, sp_vector_t *result, double *time);

int compare_multiplication_algorithms_func(void);

// Функция для выделения памяти под разреженную матрицу
int sp_matrix_alloc(sp_matrix_t *matrix, size_t rows, size_t cols, size_t cnt_non_zeros)
{
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->cnt_non_zeros = cnt_non_zeros;

    // Выделение памяти под массив A (значения ненулевых элементов)
    matrix->A = (int *)malloc(cnt_non_zeros * sizeof(int));
    if (!matrix->A)
    {
        return ERR_MEM;
    }

    // Выделение памяти под массив IA (номера строк для элементов в A)
    matrix->IA = (int *)malloc(cnt_non_zeros * sizeof(int));
    if (!matrix->IA)
    {
        free(matrix->A);  // Освобождаем уже выделенную память
        return ERR_MEM;
    }

    // Выделение памяти под массив JA (индексы начала каждого столбца в A и IA)
    matrix->JA = (int *)malloc((cols + 1) * sizeof(int));
    if (!matrix->JA)
    {
        free(matrix->A);  // Освобождаем уже выделенную память
        free(matrix->IA); // Освобождаем уже выделенную память
        return ERR_MEM;
    }

    return ERR_OK;  // Возвращаем код успешного выполнения
}

// Функция для освобождения памяти разреженной матрицы
void sp_matrix_free(sp_matrix_t *matrix)
{
    free(matrix->A);
    free(matrix->IA);
    free(matrix->JA);
    matrix->A = NULL;
    matrix->IA = NULL;
    matrix->JA = NULL;
}

// Функция для выделения памяти под разреженный вектор-строку
int sp_vector_alloc(sp_vector_t *vector, size_t cols, size_t cnt_non_zeros)
{
    vector->cols = cols;
    vector->cnt_non_zeros = cnt_non_zeros;

    // Выделение памяти под массив B (значения ненулевых элементов)
    vector->B = (int *)malloc(cnt_non_zeros * sizeof(int));
    if (!vector->B)
        return ERR_MEM;

    // Выделение памяти под массив JB (индексы столбцов для элементов в B)
    vector->JB = (int *)malloc(cnt_non_zeros * sizeof(int));
    if (!vector->JB)
    {
        free(vector->B);  // Освобождаем уже выделенную память
        return ERR_MEM;
    }

    return ERR_OK;  // Возвращаем код успешного выполнения
}

// Функция для освобождения памяти разреженного вектора-строки
void sp_vector_free(sp_vector_t *vector)
{
    free(vector->B);
    free(vector->JB);
    vector->B = NULL;
    vector->JB = NULL;
}

int read_vector_func(sp_vector_t *vector)
{
    if (vector->B)
        sp_vector_free(vector);

    size_t cols, count_non_zeros;
    printf("\nВвод разреженного вектора-строки:\n\n");

    printf("Введите количество столбцов и количество ненулевых элементов в векторе через пробел: ");
    char input[100];
    if (!fgets(input, sizeof(input), stdin) || sscanf(input, "%zu%zu", &cols, &count_non_zeros) != 2 || cols == 0 || cols > MAX_COLS || count_non_zeros == 0 || count_non_zeros > cols)
        return ERR_IO;

    // count_non_zeros = percent * cols / 100;
    int rc = sp_vector_alloc(vector, cols, count_non_zeros);
    if (rc != ERR_OK)
        return rc;

    int answer;
    printf("\nЗаполнить вектор самостоятельно (0)/автоматически (1)? (0/1): ");
    char answer_input[10];
    if (!fgets(answer_input, sizeof(answer_input), stdin) || sscanf(answer_input, "%d", &answer) != 1 || answer < 0 || answer > 1)
    {
        sp_vector_free(vector);
        return ERR_IO;
    }

    for (size_t i = 0; i < count_non_zeros; i++)
        vector->JB[i] = -1;

    if (answer == 0)
    {
        // Ручной ввод
        for (int i = 0; i < count_non_zeros; ++i)
        {
            printf("Введите ненулевой элемент и индекс ненулевого элемента через пробел: ");
            int value, index;
            char element_input[100];
            if (!fgets(element_input, sizeof(element_input), stdin) || sscanf(element_input, "%d%d", &value, &index) != 2 || value == 0 || index < 0 || index >= cols)
            {
                sp_vector_free(vector);
                return ERR_IO;
            }

            bool duplicate = false;
            for (int j = 0; j < i; ++j)
            {
                if (vector->JB[j] == index)
                {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate)
            {
                printf("  [Позиция уже занята]\n");
                --i;
                continue;
            }

            vector->B[i] = value;
            vector->JB[i] = index;
        }
    }
    else // automatic
    {
        srand(time(NULL));
        for (size_t i = 0; i < count_non_zeros; ++i)
        {
            vector->B[i] = rand() % BASE + 1;

            int index;
            bool unique;
            do
            {
                unique = true;
                index = rand() % cols; 
                for (size_t j = 0; j < i; ++j)
                {
                    if (vector->JB[j] == index)
                    {
                        unique = false;
                        break;
                    }
                }
            } while (!unique);

            vector->JB[i] = index;
        }
    }

    printf("Вектор успешно введен.\n");
    return ERR_OK;
}

int read_matrix_func(sp_matrix_t *matrix)
{
    if (matrix->A)
        sp_matrix_free(matrix);

    size_t rows, cols, count_non_zeros;
    printf("\nВвод разреженной матрицы:\n\n");

    printf("Введите количество строк, количество столбцов и количество ненулевых элементов в матрице через пробел: ");
    char input[100];
    if (!fgets(input, sizeof(input), stdin) || sscanf(input, "%zu%zu%zu", &rows, &cols, &count_non_zeros) != 3 || rows == 0 || rows > MAX_ROWS || cols == 0 || cols > MAX_COLS || count_non_zeros == 0 || count_non_zeros > cols * rows)
        return ERR_IO;

    int rc = sp_matrix_alloc(matrix, rows, cols, count_non_zeros);
    if (rc != ERR_OK)
        return rc;

    int answer;
    printf("\nЗаполнить вектор самостоятельно (0)/автоматически (1)? (0/1): ");
    char answer_input[10];
    if (!fgets(answer_input, sizeof(answer_input), stdin) || sscanf(answer_input, "%d", &answer) != 1 || answer < 0 || answer > 1)
    {
        sp_matrix_free(matrix);
        return ERR_IO;
    }

    int **full_matrix = (int **)malloc(matrix->rows * sizeof(int *));
    for (size_t i = 0; i < matrix->rows; i++)
    {
        full_matrix[i] = (int *)calloc(matrix->cols, sizeof(int));
    }

    if (!answer)
    {
        for (int i = 0; i < count_non_zeros; i++)
        {
            int col, value, row;
            bool is_correct = false;
            do
            {
                printf("Введите ненулевой элемент, номер строки и номер столбца через пробел: ");
                char element_input[100];
                if (!fgets(element_input, sizeof(element_input), stdin) || sscanf(element_input, "%d%d%d", &value, &row, &col) != 3 || value == 0 || row < 0 || row >= rows || col < 0 || col >= cols)
                {
                    sp_matrix_free(matrix);

                    for (size_t i = 0; i < matrix->rows; i++)
                    {
                        free(full_matrix[i]);
                    }
                    free(full_matrix);

                    return ERR_IO;
                }
                if (full_matrix[row][col] == 0)
                {
                    full_matrix[row][col] = value;
                    is_correct = true;
                }
                else
                    printf("  [Позиция уже занята]\n  ");

            } while (!is_correct);
        }
    }
    else
    {
        // auto
        srand(time(NULL));
        for (size_t i = 0; i < count_non_zeros; ++i)
        {
            bool is_correct = false;
            do
            {
                int value = rand() % BASE + 1;
                int row = rand() % rows;
                int col = rand() % cols;
                if (full_matrix[row][col] == 0)
                {
                    full_matrix[row][col] = value;
                    is_correct = true;
                }
            } while (!is_correct);
        }
    }

    size_t cur = 0;
    for (size_t j = 0; j < cols; j++)
    {
        matrix->JA[j] = -1;
        for (size_t i = 0; i < rows; i++)
            if (full_matrix[i][j])
            {
                matrix->A[cur] = full_matrix[i][j];
                matrix->IA[cur] = i;
                if (matrix->JA[j] == -1)
                    matrix->JA[j] = cur;
                cur++;
            }
    }

    matrix->JA[0] = 0;
    matrix->JA[cols] = cur;
    for (size_t i = cols - 1; i >= 1; i--)
        if (matrix->JA[i] == -1)
            matrix->JA[i] = matrix->JA[i + 1];

    for (size_t i = 0; i < matrix->rows; i++)
        free(full_matrix[i]);

    free(full_matrix);

    printf("Матрица успешно введена.\n");
    return ERR_OK;
}

int print_vector_func(const sp_vector_t *vector)
{
    if (!vector->B)
    {
        printf("Вектор не введён.\n");
        return ERR_IO;
    }

    if (vector->cols <= PRINTABLE_COLS)
    {
        printf("Полный вектор:\n");
        for (size_t col = 0; col < vector->cols; ++col)
        {
            int num = 0;
            for (size_t j = 0; !num && j < vector->cnt_non_zeros; j++)
                if ((size_t)vector->JB[j] == col)
                    num = vector->B[j];
            printf("%d ", num);
        }
        printf("\n");
    }

    printf("Разреженный вектор:\n");
    printf("  B: ");
    for (size_t i = 0; i < vector->cnt_non_zeros; ++i)
        printf("%d ", vector->B[i]);
    printf("\n");

    printf("  JB: ");
    for (size_t i = 0; i < vector->cnt_non_zeros; ++i)
        printf("%d ", vector->JB[i]);
    printf("\n");

    return ERR_OK;
}

int print_matrix_func(const sp_matrix_t *matrix)
{
    if (!matrix->A)
    {
        printf("\nМатрица не введена.\n");
        return ERR_IO;
    }

    // вывод всех элементов матрицы (если размер <= 30x30)
    if (matrix->cols <= PRINTABLE_COLS && matrix->rows <= PRINTABLE_ROWS)
    {
        int **full_matrix = (int **)malloc(matrix->rows * sizeof(int *));
        for (size_t i = 0; i < matrix->rows; i++)
            full_matrix[i] = (int *)calloc(matrix->cols, sizeof(int));

        // Заполняем полную матрицу значениями из разреженного представления
        size_t cur = 0;
        for (size_t j = 0; j < matrix->cols; j++)
        {
            size_t cnt = matrix->JA[j + 1] - matrix->JA[j];
            for (size_t i = cur; i < cur + cnt; i++)
                full_matrix[matrix->IA[i]][j] = matrix->A[i];
            cur += cnt;
        }

        // Выводим полную матрицу
        printf("Полная матрица:\n");
        for (size_t i = 0; i < matrix->rows; i++)
        {
            for (size_t j = 0; j < matrix->cols; j++)
                printf("%d ", full_matrix[i][j]);

            printf("\n");
        }

        for (size_t i = 0; i < matrix->rows; i++)
            free(full_matrix[i]);

        free(full_matrix);
    }

    printf("Разреженная матрица:\n");
    printf("  A: ");
    for (size_t i = 0; i < matrix->cnt_non_zeros; ++i)
        printf("%d ", matrix->A[i]);

    printf("\n");

    printf("  IA: ");
    for (size_t i = 0; i < matrix->cnt_non_zeros; ++i)
        printf("%d ", matrix->IA[i]);
    printf("\n");

    printf("  JA: ");
    for (size_t i = 0; i <= matrix->cols; ++i)
        printf("%d ", matrix->JA[i]);
    printf("\n");

    return ERR_OK;
}

int multiply_vector_on_matrix_func(const sp_vector_t *vector, const sp_matrix_t *matrix, sp_vector_t *result, double *time)
{
    if (!vector->B || !matrix->A)
    {
        printf("Вектор или матрица не введены.\n");
        return ERR_IO;
    }

    if (vector->cols != matrix->rows)
        return ERR_CANNOT_MULTIPLY;

    int *temp_result = calloc(matrix->cols, sizeof(int));
    if (!temp_result)
        return ERR_MEM;

    clock_t start = clock();

    size_t ind_ja = 0;
    for (size_t j = 0; j < matrix->cols; j++)
    {
        size_t cnt = matrix->JA[j + 1] - matrix->JA[j];
        for (size_t i = ind_ja; i < ind_ja + cnt; i++)
        {
            int m_value = matrix->A[i];
            int row = matrix->IA[i];
            for (size_t k = 0; k < vector->cnt_non_zeros; k++)
                if (row == vector->JB[k])
                    temp_result[j] += m_value * vector->B[k];
        }
        ind_ja += cnt;
    }

    // Подсчет количества ненулевых элементов
    size_t non_zero_count = 0;
    for (size_t i = 0; i < matrix->cols; i++)
        if (temp_result[i] != 0)
            non_zero_count++;

    clock_t end = clock();
    *time = (double)(end - start) / CLOCKS_PER_SEC;

    // Выделение памяти под результирующий вектор
    int rc = sp_vector_alloc(result, matrix->cols, non_zero_count);
    if (rc != ERR_OK)
    {
        free(temp_result);
        return rc;
    }

    // Заполнение результирующего вектора
    size_t index = 0;
    for (size_t i = 0; i < matrix->cols; i++)
    {
        if (temp_result[i] != 0)
        {
            result->B[index] = temp_result[i];
            result->JB[index] = i;
            index++;
        }
    }

    free(temp_result);
    return ERR_OK;
}

int multiply_vector_on_matrix_full_func(const sp_vector_t *vector, const sp_matrix_t *matrix, sp_vector_t *result, double *time)
{
    if (!vector->B || !matrix->A)
    {
        printf("Вектор или матрица не введены.\n");
        return ERR_IO;
    }

    if (vector->cols != matrix->rows)
        return ERR_CANNOT_MULTIPLY;

    // Создание полной матрицы из разреженной
    int **full_matrix = (int **)malloc(matrix->rows * sizeof(int *));
    if (!full_matrix)
        return ERR_MEM;

    for (size_t i = 0; i < matrix->rows; i++)
        full_matrix[i] = (int *)calloc(matrix->cols, sizeof(int));
    if (!full_matrix)
    {
        free(full_matrix);
        return ERR_MEM;
    }

    size_t cur = 0;
    for (size_t j = 0; j < matrix->cols; j++)
    {
        size_t cnt = matrix->JA[j + 1] - matrix->JA[j];
        for (size_t i = cur; i < cur + cnt; i++)
            full_matrix[matrix->IA[i]][j] = matrix->A[i];
        cur += cnt;
    }

    // Преобразование разреженного вектора в полный
    int *full_vector = malloc(vector->cols * sizeof(int));
    if (!full_vector)
    {
        for (size_t i = 0; i < matrix->rows; i++)
            free(full_matrix[i]);
        free(full_matrix);
        return ERR_MEM;
    }

    for (size_t col = 0; col < vector->cols; ++col)
    {
        int num = 0;
        for (size_t j = 0; j < vector->cnt_non_zeros; j++)
            if ((size_t)vector->JB[j] == col)
            {
                num = vector->B[j];
                break;
            }
        full_vector[col] = num;
    }

    int *temp_b = calloc(matrix->cols, sizeof(int));
    if (!temp_b)
    {
        free(full_vector);
        for (size_t i = 0; i < matrix->rows; i++)
            free(full_matrix[i]);
        free(full_matrix);
        return ERR_MEM;
    }

    clock_t start = clock();

    for (size_t j = 0; j < matrix->cols; j++)
    {
        for (size_t i = 0; i < matrix->rows; i++)
            temp_b[j] += full_matrix[i][j] * full_vector[i];
    }

    clock_t end = clock();
    *time = (double)(end - start) / CLOCKS_PER_SEC;

    // Подсчет количества ненулевых элементов
    size_t non_zeros = 0;
    for (size_t i = 0; i < matrix->cols; i++)
        if (temp_b[i])
            non_zeros++;

    // Выделение памяти под результирующий вектор
    int rc = sp_vector_alloc(result, matrix->cols, non_zeros);
    if (rc != ERR_OK)
    {
        free(temp_b);
        free(full_vector);
        for (size_t i = 0; i < matrix->rows; i++)
            free(full_matrix[i]);
        free(full_matrix);
        return rc;
    }

    // Заполнение результирующего вектора
    size_t ind = 0;
    for (size_t i = 0; i < matrix->cols; i++)
        if (temp_b[i])
        {
            result->B[ind] = temp_b[i];
            result->JB[ind] = i;
            ind++;
        }

    // Освобождение временных ресурсов
    free(temp_b);
    free(full_vector);
    for (size_t i = 0; i < matrix->rows; i++)
        free(full_matrix[i]);
    free(full_matrix);

    return ERR_OK;
}

static int generate_sparse_matrix(sp_matrix_t *matrix, size_t rows, size_t cols, size_t percent)
{
    size_t count_non_zeros = percent * rows * cols / 100;

    int rc = sp_matrix_alloc(matrix, rows, cols, count_non_zeros);
    if (rc != ERR_OK)
        return ERR_MEM;

    // Создаем и инициализируем нулями полную матрицу
    int **full_matrix = (int **)malloc(matrix->rows * sizeof(int *));
    for (size_t i = 0; i < matrix->rows; i++) {
        full_matrix[i] = (int *)calloc(matrix->cols, sizeof(int));
    }

    // auto
    srand(time(NULL));
    for (size_t i = 0; i < count_non_zeros; ++i)
    {
        bool is_correct = false;
        do
        {
            int value = rand() % BASE + 1; // Генерация случайных значений
            int row = rand() % rows; // Генерация случайных строк
            int col = rand() % cols;   // Генерация случайных столбцов
            if (full_matrix[row][col] == 0)
            {
                full_matrix[row][col] = value;
                is_correct = true;
            }             
        }
        while (!is_correct);
    }

    size_t cur = 0;
    for (size_t j = 0; j < cols; j++)
    {
        matrix->JA[j] = -1;
        for (size_t i = 0; i < rows; i++)
            if (full_matrix[i][j])
            {
                matrix->A[cur] = full_matrix[i][j];
                matrix->IA[cur] = i;
                if (matrix->JA[j] == -1)
                    matrix->JA[j] = cur;
                cur++;
            }
    }

    matrix->JA[0] = 0;
    matrix->JA[cols] = cur;
    for (size_t i = cols - 1; i >= 1; i--)
        if (matrix->JA[i] == -1)
            matrix->JA[i] = matrix->JA[i + 1];

    // Освобождаем память, выделенную под полную матрицу
    for (size_t i = 0; i < matrix->rows; i++) {
        free(full_matrix[i]);
    }
    free(full_matrix);

    return ERR_OK;
}

static int generate_sparse_vector(sp_vector_t *vector, size_t cols, size_t percent)
{
    size_t count_non_zeros = percent * cols / 100;

    int rc = sp_vector_alloc(vector, cols, count_non_zeros);
    if (rc != ERR_OK)
        return rc;

    for (size_t i = 0; i < count_non_zeros; i++)
        vector->JB[i] = -1;
    
    // Автоматическая генерация
    srand(time(NULL));
    for (size_t i = 0; i < count_non_zeros; ++i) {
        vector->B[i] = rand() % BASE + 1; // Генерация случайного ненулевого элемента
        
        int index;
        bool unique;
        do {
            unique = true;
            index = rand() % cols; // Генерация случайного индекса
            // Проверка на уникальность индекса
            for (size_t j = 0; j < i; ++j) {
                if (vector->JB[j] == index) {
                    unique = false;
                    break;
                }
            }
        } while (!unique); // Повторяем генерацию, пока индекс не станет уникальным
        
        vector->JB[i] = index;
    }

    return ERR_OK;
}

int compare_multiplication_algorithms_func(void)
{
    srand(time(NULL));
    size_t sizes[] = {10, 50, 100, 500};
    size_t percents[] = {10, 20, 30, 40, 50, 100};
    // Параметры матрицы и вектора

    printf("| %s\t| %s\t| %s\t| %s\t| %s\t| %s\t|\n", "Размер", "Процент", "Время (разреженная)", "Время (стандартная)", "Память (разреженная)", "Память (стандартная)");

    for (size_t w = 0; w < sizeof(sizes) / sizeof(sizes[0]); w++)
    {
        for (size_t i = 0; i < sizeof(percents) / sizeof(percents[0]); i++)
        {
            int rows = sizes[w], cols = sizes[w];

            // Инициализация матрицы и вектора
            sp_matrix_t matrix;
            sp_vector_t vector;
            sp_vector_t result;

            int rc = generate_sparse_matrix(&matrix, rows, cols, percents[i]);
            if (rc != ERR_OK)
                return rc;

            rc = generate_sparse_vector(&vector, cols, percents[i]);
            if (rc != ERR_OK)
            {
                sp_matrix_free(&matrix);
                return rc;
            }

            double time_full, time_sparse;
            double time_full_avg, time_sparse_avg;

            for (size_t i = 0; i < N; i++)
            {
                // Измерение времени для умножения с использованием разреженной матрицы
                rc = multiply_vector_on_matrix_func(&vector, &matrix, &result, &time_sparse);
                if (rc != ERR_OK)
                {
                    sp_matrix_free(&matrix);
                    sp_vector_free(&vector);
                    return rc;
                }

                // Измерение времени для стандартного умножения матрицы на вектор
                rc = multiply_vector_on_matrix_full_func(&vector, &matrix, &result, &time_full);
                if (rc != ERR_OK)
                {
                    sp_matrix_free(&matrix);
                    sp_vector_free(&vector);
                    return rc;
                }
                time_full_avg += time_full;
                time_sparse_avg += time_sparse;
            }

            time_full_avg /= N;
            time_sparse_avg /= N;

            // Вывод результатов

            printf("| %10zu\t| %10zu\t| %21.6f | %21.6f | %16zu\t| %16zu\t|\n", sizes[w], percents[i], time_sparse_avg, time_full_avg, (matrix.cols + 1 + matrix.cnt_non_zeros * 2) * 4 + 3 * sizeof(size_t), (matrix.cols * matrix.rows * 4));

            // Освобождение памяти
            sp_matrix_free(&matrix);
            sp_vector_free(&vector);
            sp_vector_free(&result);
        }
    }

    return ERR_OK;
}

int main(void)
{
    setbuf(stdout, NULL);

    int rc;
    int action;
    sp_matrix_t matrix = {0};
    sp_vector_t vector = {0};
    sp_vector_t result = {0}; // Результирующий вектор

    while (true)
    {
        printf("Программа, выполняющая умножение разреженного вектора-строки на разреженную матрицу\n");
        printf("Меню:\n");
        printf(" 1) Ввести разреженный вектор-строку\n");
        printf(" 2) Ввести разреженную матрицу\n");
        printf(" 3) Отобразить разреженный вектор-строку\n");
        printf(" 4) Отобразить разреженную матрицу\n");
        printf(" 5) Умножить разреженный вектор-строку на разреженную матрицу\n");
        printf(" 6) Умножить разреженный вектор-строку на разреженную матрицу стандартным методом\n");
        printf(" 7) Сравнить время выполнения двух алгоритмов умножения\n");
        printf(" 0) Выход\n");

        printf("Ваш ввод: ");
        char input_buffer[INPUT_BUFFER_SIZE];

        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
        {
            printf("Ошибка ввода.\n");
            continue;
        }

        size_t len = strlen(input_buffer);
        if (len > 0 && input_buffer[len - 1] == '\n')
            input_buffer[len - 1] = '\0';

        int tmp;

        if (sscanf(input_buffer, "%d", &tmp) != 1)
        {
            printf("Ошибка ввода.\n");
            continue;
        }

        if (tmp < 0 || tmp > 7)
        {
            printf("Неверный пункт меню. Попробуйте снова.\n");
            continue;
        }

        action = tmp;
        if (action == 0)
            break;

        double time;
        rc = ERR_OK;
        switch (action)
        {
            case 1: // read_vector
                rc = read_vector_func(&vector);
                break;
            case 2: // read_matrix
                rc = read_matrix_func(&matrix);
                break;
            case 3: // print_vector
                rc = print_vector_func(&vector);
                break;
            case 4: // print_matrix
                rc = print_matrix_func(&matrix);
                break;
            case 5: // multiply_vector_on_matrix
                rc = multiply_vector_on_matrix_func(&vector, &matrix, &result, &time);
                if (rc == ERR_OK)
                {
                    printf("Результат умножения (разреженный вектор):\n");
                    print_vector_func(&result);
                    printf("Время выполнения: %.6f секунд.\n", time);
                    sp_vector_free(&result); // Освобождение памяти после использования
                }
                break;
            case 6: // multiply_vector_on_matrix_full
                rc = multiply_vector_on_matrix_full_func(&vector, &matrix, &result, &time);
                if (rc == ERR_OK)
                {
                    printf("Результат умножения (разреженный вектор):\n");
                    print_vector_func(&result);
                    printf("Время выполнения: %.6f секунд.\n", time);
                    sp_vector_free(&result); // Освобождение памяти после использования
                }
                break;
            case 7: // compare_multiplication_algorithms
                rc = compare_multiplication_algorithms_func();
                break;
        }

        if (rc != ERR_OK)
        {
            switch (rc)
            {
                case ERR_MENU:
                    printf("Неверный пункт меню.\n");
                    break;
                case ERR_IO:
                    printf("Ошибка ввода.\n");
                    break;
                case ERR_RANGE:
                    printf("Ошибка диапазона ввода.\n");
                    break;
                case ERR_MEM:
                    printf("Ошибка выделения памяти.\n");
                    break;
                case ERR_CANNOT_MULTIPLY:
                    printf("Ошибка: невозможно выполнить умножение.\n");
                    break;
                default:
                    printf("Неизвестная ошибка.\n");
                    break;
            }
        }
    }

    // Освобождение памяти перед завершением программы
    sp_matrix_free(&matrix);
    sp_vector_free(&vector);
    sp_vector_free(&result);

    return ERR_OK;
}
