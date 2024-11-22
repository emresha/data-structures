#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Структура узла бинарного дерева
typedef struct TreeNode
{
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode *insert(TreeNode *root, int value);

// Освобождение памяти дерева
void free_tree(TreeNode *root)
{
    if (root == NULL)
        return;

    free_tree(root->left);
    free_tree(root->right);

    free(root);
}


// Чтение чисел из файла и построение бинарного дерева
void read_from_file(const char *filename, TreeNode **root)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        exit(EXIT_FAILURE);
    }
    int value;
    while (fscanf(file, "%d", &value) == 1) 
        *root = insert(*root, value);

    if (!feof(file))
    {
        printf("Ошибка чтения файла.\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

// Генерация строк для файла Graphviz
void generate_dot(TreeNode *root, FILE *f)
{
    if (root == NULL)
        return;

    if (root->left)
    {
        fprintf(f, "    %d -> %d;\n", root->value, root->left->value);
        generate_dot(root->left, f);
    }
    if (root->right)
    {
        fprintf(f, "    %d -> %d;\n", root->value, root->right->value);
        generate_dot(root->right, f);
    }

    if (!root->left && !root->right)
    {
        fprintf(f, "    %d ;\n", root->value);
    }
}

// Экспорт бинарного дерева в файл формата .dot
void export_to_dot(TreeNode *root, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Ошибка при создании .dot файла.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "digraph BinaryTree {\n");
    fprintf(f, "    node [fontname=\"Arial\", shape=circle];\n");
    generate_dot(root, f);
    fprintf(f, "}\n");
    fclose(f);
}

// Добавление числа в файл
void add_to_file(const char *filename, int value)
{
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(file, " %d\n", value);
    fclose(file);
}

// Создание нового узла
TreeNode *create_node(int value)
{
    TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!new_node)
    {
        printf("Ошибка выделения памяти.\n");
        exit(EXIT_FAILURE);
    }
    new_node->value = value;
    new_node->left = new_node->right = NULL;
    return new_node;
}

// Вставка значения в бинарное дерево
TreeNode *insert(TreeNode *root, int value)
{
    if (!root)
        return create_node(value);
    if (value < root->value)
        root->left = insert(root->left, value);
    else if (value > root->value)
        root->right = insert(root->right, value);
    return root;
}

// Поиск узла
TreeNode *search(TreeNode *root, int value)
{
    if (!root || root->value == value)
        return root;
    if (value < root->value)
        return search(root->left, value);
    return search(root->right, value);
}

// Удаление узла
TreeNode *delete_node(TreeNode *root, int value)
{
    if (!root)
        return NULL;
    if (value < root->value)
        root->left = delete_node(root->left, value);
    else if (value > root->value)
        root->right = delete_node(root->right, value);
    else
    {
        if (!root->left)
        {
            TreeNode *temp = root->right;
            free(root);
            return temp;
        }
        if (!root->right)
        {
            TreeNode *temp = root->left;
            free(root);
            return temp;
        }
        TreeNode *temp = root->right;
        while (temp->left)
            temp = temp->left;
        root->value = temp->value;
        root->right = delete_node(root->right, temp->value);
    }
    return root;
}

// Прямой обход
void preorder(TreeNode *root)
{
    if (root)
    {
        printf("%d ", root->value);
        preorder(root->left);
        preorder(root->right);
    }
}

// Симметричный обход
void inorder(TreeNode *root)
{
    if (root)
    {
        inorder(root->left);
        printf("%d ", root->value);
        inorder(root->right);
    }
}

// Обратный обход
void postorder(TreeNode *root)
{
    if (root)
    {
        postorder(root->left);
        postorder(root->right);
        printf("%d ", root->value);
    }
}

// Подсчет узлов на каждом уровне
void count_nodes_per_level(TreeNode *root, int level, int *counts, int *max_level)
{
    if (!root)
        return;
    counts[level]++;
    if (level > *max_level)
        *max_level = level;
    count_nodes_per_level(root->left, level + 1, counts, max_level);
    count_nodes_per_level(root->right, level + 1, counts, max_level);
}

// Вывод количества узлов на каждом уровне
void print_nodes_per_level(TreeNode *root)
{
    int counts[100] = {0};
    int max_level = 0;
    count_nodes_per_level(root, 0, counts, &max_level);
    for (int i = 0; i <= max_level; i++)
    {
        printf("Уровень %d: %d узлов\n", i, counts[i]);
    }
}

void generate_file(const char *filename, int count)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Ошибка создания файла.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%d\n", rand() % 10000); // Генерация чисел от 0 до 9999
    }
    fclose(file);
}

// Поиск числа в файле
int search_in_file(const char *filename, int value)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        exit(EXIT_FAILURE);
    }

    int num, found = 0;
    while (fscanf(file, "%d", &num) == 1)
    {
        if (num == value)
        {
            found = 1;
            break;
        }
    }
    fclose(file);
    return found;
}

// Удаление числа из файла
void delete_from_file(const char *filename, int value)
{
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp)
    {
        printf("Ошибка работы с файлами.\n");
        exit(EXIT_FAILURE);
    }

    int num;
    while (fscanf(file, "%d", &num) == 1)
    {
        if (num != value)
        {
            fprintf(temp, "%d\n", num);
        }
    }
    fclose(file);
    fclose(temp);
    remove(filename);
    rename("temp.txt", filename);
}

// Функция для тестирования операций
void test_operations(const char *filename)
{
    const int iterations = 10;
    const int num_elements = 500000;
    double total_tree_insert = 0.0, total_file_insert = 0.0;
    double total_tree_search = 0.0, total_file_search = 0.0;
    double total_tree_delete = 0.0, total_file_delete = 0.0;

    for (int i = 0; i < iterations; i++)
    {
        // Создание файла и дерева
        generate_file(filename, num_elements);
        TreeNode *root = NULL;
        read_from_file(filename, &root);

        // Вставка элемента
        int insert_value = rand() % 10000;

        clock_t start_tree_insert = clock();
        root = insert(root, insert_value);
        clock_t end_tree_insert = clock();
        total_tree_insert += (double)(end_tree_insert - start_tree_insert) / CLOCKS_PER_SEC;

        clock_t start_file_insert = clock();
        add_to_file(filename, insert_value);
        clock_t end_file_insert = clock();
        total_file_insert += (double)(end_file_insert - start_file_insert) / CLOCKS_PER_SEC;

        // Поиск элемента
        int search_value = rand() % 10000;

        clock_t start_tree_search = clock();
        TreeNode *found = search(root, search_value);
        clock_t end_tree_search = clock();
        total_tree_search += (double)(end_tree_search - start_tree_search) / CLOCKS_PER_SEC;

        clock_t start_file_search = clock();
        search_in_file(filename, search_value);
        clock_t end_file_search = clock();
        total_file_search += (double)(end_file_search - start_file_search) / CLOCKS_PER_SEC;

        // Удаление элемента
        int delete_value = rand() % 10000;

        clock_t start_tree_delete = clock();
        root = delete_node(root, delete_value);
        clock_t end_tree_delete = clock();
        total_tree_delete += (double)(end_tree_delete - start_tree_delete) / CLOCKS_PER_SEC;

        clock_t start_file_delete = clock();
        delete_from_file(filename, delete_value);
        clock_t end_file_delete = clock();
        total_file_delete += (double)(end_file_delete - start_file_delete) / CLOCKS_PER_SEC;

        // Очистка памяти дерева
        free_tree(root);
    }

    // Вывод результатов
    printf("Среднее время операций за %d итераций c %d элементами:\n", iterations, num_elements);
    printf("  С бинарным деревом:\n");
    printf("    Вставка: %.12f секунд\n", total_tree_insert / iterations);
    printf("    Поиск:  %.12f секунд\n", total_tree_search / iterations);
    printf("    Удаление: %.12f секунд\n", total_tree_delete / iterations);

    printf("  С файлом:\n");
    printf("    Вставка: %.12f секунд\n", total_file_insert / iterations);
    printf("    Поиск:  %.12f секунд\n", total_file_search / iterations);
    printf("    Удаление: %.12f секунд\n", total_file_delete / iterations);
}

int main(void)
{
    printf("Программа для работы с бинарным деревом.\n");

    TreeNode *root = NULL;
    char filename[256];

    // Запрос имени файла с числами
    while (1)
    {
        printf("Введите имя файла для чтения: ");
        if (fgets(filename, sizeof(filename), stdin) != NULL)
        {
            size_t len = strlen(filename);
            if (len > 0 && filename[len - 1] == '\n')
                filename[len - 1] = '\0';

            FILE *file = fopen(filename, "r");
            if (file)
            {
                fclose(file);
                break;
            }
            else
            {
                printf("Ошибка открытия файла. Попробуйте снова.\n");
            }
        }
    }

    // Построение дерева из файла
    read_from_file(filename, &root);
    printf("Дерево успешно построено.\n");

    int choice;
    do
    {
        printf("\nВыберите действие:\n");
        printf("1. Вывести дерево (визуализация в Graphviz)\n");
        printf("2. Вывести количество узлов на каждом уровне\n");
        printf("3. Выполнить обход дерева\n");
        printf("4. Добавить число в дерево и файл\n");
        printf("5. Удалить число из дерева\n");
        printf("6. Найти число в дереве\n");
        printf("7. Провести замерный эксперимент\n");
        printf("0. Выйти\n");
        printf("Ваш выбор: ");
        char input[10];
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Ошибка: Введите число.\n");
            continue;
        }

        switch (choice)
        {
        case 1: // Визуализация дерева
        {
            char dot_filename[256];
            snprintf(dot_filename, sizeof(dot_filename), "tree.dot");
            export_to_dot(root, dot_filename);
            printf("Дерево экспортировано в файл %s.\n", dot_filename);

            // Генерация изображения
            char command[512];
            snprintf(command, sizeof(command), "dot -Tpng %s -o tree.png", dot_filename);
            if (system(command) == 0)
            {
                printf("Изображение дерева сохранено в tree.png.\n");
                system("xdg-open tree.png > /dev/null");
            }
            else
            {
                printf("Ошибка генерации изображения.\n");
            }
            break;
        }

        case 2: // Подсчет узлов на уровнях
            printf("Количество узлов на каждом уровне:\n");
            print_nodes_per_level(root);
            break;

        case 3: // Обход дерева
        {
            printf("\nВыберите тип обхода:\n");
            printf("1. Прямой\n");
            printf("2. Симметричный\n");
            printf("3. Обратный\n");
            printf("Ваш выбор: ");
            int traverse_choice;
            char traverse_input[10];
            fgets(traverse_input, sizeof(traverse_input), stdin);
            if (sscanf(traverse_input, "%d", &traverse_choice) != 1)
            {
                printf("Ошибка: Введите число.\n");
                continue;
            }
            switch (traverse_choice)
            {
            case 1:
                printf("Прямой обход: ");
                preorder(root);
                break;
            case 2:
                printf("Симметричный обход: ");
                inorder(root);
                break;
            case 3:
                printf("Обратный обход: ");
                postorder(root);
                break;
            default:
                printf("Неверный выбор.\n");
            }
            printf("\n");
            break;
        }

        case 4: // Добавление числа
        {
            int new_value;
            printf("Введите число для добавления: ");
            char input[10];
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                if (sscanf(input, "%d", &new_value) != 1)
                {
                    printf("Ошибка: Введите корректное число.\n");
                    continue;
                }
            }

            clock_t start_tree = clock();
            root = insert(root, new_value);
            clock_t end_tree = clock();

            clock_t start_file = clock();
            add_to_file(filename, new_value);
            clock_t end_file = clock();

            printf("Число добавлено в дерево и файл.\n");
            printf("Время добавления в дерево: %.6f с\n", (double)(end_tree - start_tree) / CLOCKS_PER_SEC);
            printf("Время добавления в файл: %.6f с\n", (double)(end_file - start_file) / CLOCKS_PER_SEC);
            break;
        }

        case 5: // Удаление числа
        {
            int delete_value;
            printf("Введите число для удаления: ");
            char input[10];
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                if (sscanf(input, "%d", &delete_value) != 1)
                {
                    printf("Ошибка: Введите корректное число.\n");
                    continue;
                }
            }

            TreeNode *result = search(root, delete_value);
            if (result)
            {
                root = delete_node(root, delete_value);
                printf("Число %d удалено из дерева.\n", delete_value);
            }
            else
            {
                printf("Число %d не найдено в дереве.\n", delete_value);
            }
            break;
        }

        case 6: // Поиск числа
        {
            int search_value;
            printf("Введите число для поиска: ");
            char input[10];
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                if (sscanf(input, "%d", &search_value) != 1)
                {
                    printf("Ошибка: Введите корректное число.\n");
                    continue;
                }
            }

            TreeNode *result = search(root, search_value);
            if (result)
                printf("Число %d найдено в дереве.\n", search_value);
            else
                printf("Число %d не найдено в дереве.\n", search_value);
            break;
        }
        case 7:
            test_operations("experiment.txt");
            break;
        case 0: // Выход
            printf("Завершение работы программы.\n");
            break;

        default:
            printf("Неверный выбор. Попробуйте снова.\n");
        }

    } while (choice != 0);

    return 0;
}
