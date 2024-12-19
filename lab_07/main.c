#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 101

typedef struct HashNode
{
    int key;
    struct HashNode *next;
} HashNode;

typedef struct HashTable
{
    HashNode **table;
    size_t occupied;
    int size;
} HashTable;

void rehash(HashTable **hash_table_ptr, int *current_size);

typedef struct TreeNode
{
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct AVLNode
{
    int value;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

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
    TreeNode *new_node = create_node(value);

    // Если дерево пустое, возвращаем новый узел как корень
    if (!root)
    {
        return new_node;
    }

    TreeNode *current = root;
    TreeNode *parent = NULL;

    // Ищем подходящее место для вставки
    while (current)
    {
        parent = current;
        if (value < current->value)
        {
            current = current->left;
        }
        else if (value > current->value)
        {
            current = current->right;
        }
        else
        {
            // Значение уже существует, возвращаем исходное дерево
            free(new_node);
            return root;
        }
    }

    // Вставляем новый узел в найденное место
    if (value < parent->value)
    {
        parent->left = new_node;
    }
    else
    {
        parent->right = new_node;
    }

    return root;
}

// Поиск узла
TreeNode *search(TreeNode *root, int value, int *comparisons)
{
    *comparisons = 0;
    while (root)
    {
        (*comparisons)++;
        if (value == root->value)
            return root;
        else if (value < root->value)
            root = root->left;
        else
            root = root->right;
    }
    return NULL;
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

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int height(AVLNode *node)
{
    return (node) ? node->height : 0;
}

AVLNode *create_avl_node(int value)
{
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    node->value = value;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

AVLNode *rotate_right(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode *rotate_left(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int get_balance(AVLNode *node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

AVLNode *balance_node(AVLNode *node)
{
    if (!node)
        return NULL;

    // Обновляем высоту узла
    node->height = 1 + max(height(node->left), height(node->right));

    int balance = get_balance(node);

    // LL Case
    if (balance > 1 && get_balance(node->left) >= 0)
    {
        return rotate_right(node);
    }

    // LR Case
    if (balance > 1 && get_balance(node->left) < 0)
    {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    // RR Case
    if (balance < -1 && get_balance(node->right) <= 0)
    {
        return rotate_left(node);
    }

    // RL Case
    if (balance < -1 && get_balance(node->right) > 0)
    {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node; // Если дисбаланса нет, узел не меняется
}

// Рекурсивно балансируем поддеревья, затем сам узел
AVLNode *balance_avl_subtree(AVLNode *node)
{
    if (!node)
        return NULL;

    node->left = balance_avl_subtree(node->left);
    node->right = balance_avl_subtree(node->right);

    return balance_node(node);
}

// Балансируем всё дерево
AVLNode *balance_avl_tree(AVLNode *root)
{
    return balance_avl_subtree(root);
}

AVLNode *insert_avl(AVLNode *root, int value)
{
    AVLNode *new_node = create_avl_node(value);

    // Если дерево пустое, просто возвращаем новый узел
    if (!root)
    {
        return new_node;
    }

    AVLNode *current = root;
    AVLNode *parent = NULL;
    AVLNode **link_to_update = NULL;

    // Поиск места для вставки
    while (current)
    {
        parent = current;
        if (value < current->value)
        {
            link_to_update = &current->left;
            current = current->left;
        }
        else if (value > current->value)
        {
            link_to_update = &current->right;
            current = current->right;
        }
        else
        {
            // Значение уже существует, возвращаем исходное дерево
            free(new_node);
            return root;
        }
    }

    // Вставка нового узла
    *link_to_update = new_node;

    // Восстановление баланса
    current = parent;
    while (current)
    {
        current->height = 1 + max(height(current->left), height(current->right));
        int balance = get_balance(current);

        if (balance > 1 && value < current->left->value)
        {
            if (current == root)
            {
                root = rotate_right(current);
            }
            else if (parent->left == current)
            {
                parent->left = rotate_right(current);
            }
            else
            {
                parent->right = rotate_right(current);
            }
            break;
        }

        if (balance < -1 && value > current->right->value)
        {
            if (current == root)
            {
                root = rotate_left(current);
            }
            else if (parent->left == current)
            {
                parent->left = rotate_left(current);
            }
            else
            {
                parent->right = rotate_left(current);
            }
            break;
        }

        if (balance > 1 && value > current->left->value)
        {
            current->left = rotate_left(current->left);
            if (current == root)
            {
                root = rotate_right(current);
            }
            else if (parent->left == current)
            {
                parent->left = rotate_right(current);
            }
            else
            {
                parent->right = rotate_right(current);
            }
            break;
        }

        if (balance < -1 && value < current->right->value)
        {
            current->right = rotate_right(current->right);
            if (current == root)
            {
                root = rotate_left(current);
            }
            else if (parent->left == current)
            {
                parent->left = rotate_left(current);
            }
            else
            {
                parent->right = rotate_left(current);
            }
            break;
        }

        // Переход к следующему уровню
        parent = current;
        current = (value < current->value) ? current->left : current->right;
    }

    return root;
}

AVLNode *insert_avl_unbalanced(AVLNode *root, int value)
{
    AVLNode *new_node = create_avl_node(value);

    if (!root)
    {
        return new_node;
    }

    AVLNode *current = root;
    AVLNode *parent = NULL;

    while (current)
    {
        parent = current;
        if (value < current->value)
        {
            current = current->left;
        }
        else if (value > current->value)
        {
            current = current->right;
        }
        else
        {
            free(new_node);
            return root;
        }
    }

    if (value < parent->value)
    {
        parent->left = new_node;
    }
    else
    {
        parent->right = new_node;
    }

    return root;
}

int search_avl(AVLNode *node, int value, int *comparisons)
{
    *comparisons = 0;
    while (node)
    {
        (*comparisons)++;
        if (value == node->value)
            return 1;
        else if (value < node->value)
            node = node->left;
        else
            node = node->right;
    }
    return 0;
}

void free_avl(AVLNode *node)
{
    if (!node)
        return;
    free_avl(node->left);
    free_avl(node->right);
    free(node);
}

void read_from_file_avl(const char *filename, AVLNode **root)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        exit(EXIT_FAILURE);
    }
    int value;
    while (fscanf(file, "%d", &value) == 1)
        *root = insert_avl(*root, value);

    if (!feof(file))
    {
        printf("Ошибка чтения файла.\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

// Генерация строк для файла Graphviz для AVL-дерева
void generate_dot_avl(AVLNode *root, FILE *f)
{
    if (root == NULL)
        return;

    if (root->left)
    {
        fprintf(f, "    %d -> %d;\n", root->value, root->left->value);
        generate_dot_avl(root->left, f);
    }
    if (root->right)
    {
        fprintf(f, "    %d -> %d;\n", root->value, root->right->value);
        generate_dot_avl(root->right, f);
    }

    // Узлы без детей (листовые)
    if (!root->left && !root->right)
    {
        fprintf(f, "    %d;\n", root->value);
    }
}

AVLNode *delete_node_avl(AVLNode *root, int value)
{
    if (!root)
        return root; // Узел не найден

    // Стандартное удаление в BST
    if (value < root->value)
    {
        root->left = delete_node_avl(root->left, value);
    }
    else if (value > root->value)
    {
        root->right = delete_node_avl(root->right, value);
    }
    else
    {
        // Узел найден
        if (!root->left || !root->right)
        {
            // Узел с одним или нулем детей
            AVLNode *temp = root->left ? root->left : root->right;
            free(root);
            return temp;
        }
        else
        {
            // Узел с двумя детьми
            AVLNode *temp = root->right;
            while (temp->left)
            {
                temp = temp->left; // Ищем минимальный узел в правом поддереве
            }
            root->value = temp->value;                               // Копируем значение
            root->right = delete_node_avl(root->right, temp->value); // Удаляем дубликат
        }
    }

    // Обновляем высоту
    root->height = 1 + max(height(root->left), height(root->right));

    // Проверяем баланс
    int balance = get_balance(root);

    // LL случай
    if (balance > 1 && get_balance(root->left) >= 0)
        return rotate_right(root);

    // LR случай
    if (balance > 1 && get_balance(root->left) < 0)
    {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    // RR случай
    if (balance < -1 && get_balance(root->right) <= 0)
        return rotate_left(root);

    // RL случай
    if (balance < -1 && get_balance(root->right) > 0)
    {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root; // Возвращаем корень
}

// Экспорт AVL-дерева в файл формата .dot
void export_to_dot_avl(AVLNode *root, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Ошибка при создании .dot файла.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "digraph AVLTree {\n");
    fprintf(f, "    node [fontname=\"Arial\", shape=circle];\n");
    generate_dot_avl(root, f);
    fprintf(f, "}\n");
    fclose(f);
}

int hash_function(int key, int table_size)
{
    int index = key % table_size;
    return (index < 0) ? index + table_size : index;
}

HashTable *create_hash_table()
{
    HashTable *hash_table = (HashTable *)malloc(sizeof(HashTable));
    hash_table->table = (HashNode **)calloc(TABLE_SIZE, sizeof(HashNode *));
    hash_table->size = TABLE_SIZE;
    hash_table->occupied = 0;

    return hash_table;
}

void insert_hash(HashTable *hash_table, int key)
{
    int index = hash_function(key, hash_table->size);

    // Создаем новый узел
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (!new_node)
    {
        printf("Ошибка выделения памяти для хеш-узла.\n");
        exit(EXIT_FAILURE);
    }
    new_node->key = key;
    new_node->next = NULL;

    // Если ячейка пуста, добавляем узел
    if (!hash_table->table[index])
    {
        hash_table->table[index] = new_node;
    }
    else
    {
        // Обработка коллизий методом цепочек
        HashNode *current = hash_table->table[index];
        while (current->next)
        {
            current = current->next;
        }
        current->next = new_node;
    }

    // Увеличиваем счетчик заполненных элементов
    hash_table->occupied++;

    // Проверяем необходимость рехеширования
    if ((double)hash_table->occupied / hash_table->size > 0.75)
    {
        rehash(&hash_table, &hash_table->size);
    }
}

int search_hash(HashTable *hash_table, int key, int *comparisons)
{
    int index = hash_function(key, hash_table->size);
    if (comparisons)
        *comparisons = 0;

    HashNode *node = hash_table->table[index];
    while (node)
    {
        if (comparisons)
            (*comparisons)++;
        if (node->key == key)
            return 1;
        node = node->next;
    }
    return 0;
}

int delete_hash(HashTable *hash_table, int key)
{
    int index = hash_function(key, hash_table->size);
    HashNode *node = hash_table->table[index];
    HashNode *prev = NULL;
    if (search_hash(hash_table, key, NULL))
    {
        while (node)
        {
            if (node->key == key)
            {
                if (prev)
                    prev->next = node->next;
                else
                    hash_table->table[index] = node->next;
                free(node);
                hash_table->occupied--;
                return 1;
            }
            prev = node;
            node = node->next;
        }
    }
    else
        return 1;

    return 0;
}

void free_hash_table(HashTable *hash_table)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode *node = hash_table->table[i];
        while (node)
        {
            HashNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(hash_table->table);
    free(hash_table);
}

void rehash(HashTable **hash_table_ptr, int *current_size)
{
    int new_size = (*current_size) * 2; // Увеличиваем размер таблицы
    HashTable *new_table = create_hash_table(new_size);
    new_table->table = (HashNode **)calloc(new_size, sizeof(HashNode *));
    new_table->size = new_size;

    // Переносим данные из старой таблицы в новую
    for (int i = 0; i < *current_size; i++)
    {
        HashNode *node = (*hash_table_ptr)->table[i];
        while (node)
        {
            insert_hash(new_table, node->key);
            node = node->next;
        }
    }

    free_hash_table(*hash_table_ptr);
    *hash_table_ptr = new_table;
    *current_size = new_size;
}

typedef struct HashNodeLinear
{
    int key;
    int is_occupied; // Флаг занятости (1 - занято, 0 - свободно)
} HashNodeLinear;

typedef struct HashTableLinear
{
    HashNodeLinear *table; // Массив узлов
    size_t size;           // Размер таблицы
    size_t occupied;       // Количество занятых ячеек
} HashTableLinear;

// Хеш-функция
int hash_function_linear(int key, int table_size)
{
    return key % table_size;
}

// Создание хэш-таблицы
HashTableLinear *create_hash_table_linear(size_t size)
{
    HashTableLinear *hash_table = (HashTableLinear *)malloc(sizeof(HashTableLinear));
    hash_table->size = size;
    hash_table->occupied = 0;
    hash_table->table = (HashNodeLinear *)calloc(size, sizeof(HashNodeLinear)); // Все ячейки свободны
    return hash_table;
}

#define LOAD_FACTOR 0.75

void rehash_linear(HashTableLinear **hash_table_ptr)
{
    HashTableLinear *old_table = *hash_table_ptr;
    size_t new_size = old_table->size * 2; // Увеличиваем размер таблицы
    HashTableLinear *new_table = create_hash_table_linear(new_size);

    // Переносим данные из старой таблицы в новую
    for (size_t i = 0; i < old_table->size; i++)
    {
        if (old_table->table[i].is_occupied)
        {
            size_t index = old_table->table[i].key % new_table->size;

            // Пробируем линейно, пока не найдем свободный слот
            while (new_table->table[index].is_occupied)
            {
                index = (index + 1) % new_table->size;
            }

            // Копируем данные
            new_table->table[index].key = old_table->table[i].key;
            new_table->table[index].is_occupied = 1;
        }
    }

    // Освобождаем память старой таблицы
    free(old_table->table);
    free(old_table);

    *hash_table_ptr = new_table;
}

// Вставка элемента с линейной адресацией
void insert_hash_linear(HashTableLinear *hash_table, int key)
{
    if ((double)hash_table->occupied / hash_table->size > LOAD_FACTOR)
    {
        printf("Рехеширование...\n");
        rehash_linear(&hash_table); // Выполняем рехеширование при переполнении
    }

    int index = hash_function_linear(key, hash_table->size);

    // Линейное пробирование
    while (hash_table->table[index].is_occupied)
    {
        if (hash_table->table[index].key == key)
        {
            // Ключ уже существует, выходим
            return;
        }
        index = (index + 1) % hash_table->size;
    }

    // Вставляем новый ключ
    hash_table->table[index].key = key;
    hash_table->table[index].is_occupied = 1;
    hash_table->occupied++;
}

// Поиск элемента с линейной адресацией
int search_hash_linear(HashTableLinear *hash_table, int key, int *comparisons)
{
    int index = hash_function_linear(key, hash_table->size);
    *comparisons = 0;

    // Линейное пробирование
    while (hash_table->table[index].is_occupied)
    {
        (*comparisons)++;
        if (hash_table->table[index].key == key)
        {
            return 1; // Найден
        }
        index = (index + 1) % hash_table->size;
    }

    return 0; // Не найден
}

// Удаление элемента с линейной адресацией
int delete_hash_linear(HashTableLinear *hash_table, int key)
{
    int index = hash_function_linear(key, hash_table->size);

    // Линейное пробирование
    while (hash_table->table[index].is_occupied)
    {
        if (hash_table->table[index].key == key)
        {
            // Ключ найден, "удаляем" его
            hash_table->table[index].is_occupied = 0;
            hash_table->occupied--;
            return 1; // Успешно удалено
        }
        index = (index + 1) % hash_table->size;
    }

    return 0; // Не найден
}

// Освобождение памяти хэш-таблицы
void free_hash_table_linear(HashTableLinear *hash_table)
{
    free(hash_table->table);
    free(hash_table);
}

void compare_structures(void)
{
    printf("Сравнение поиска в BST, AVL-дереве и хеш-таблице с различными размерами данных\n");

    // Таблица для BST
    printf("\nТаблица результатов для BST\n");
    printf("Размер | Время поиска (тики) | Количество сравнений | Используемая память (байты)\n");
    printf("-------------------------------------------------------------------------------\n");

    for (int size = 1000; size <= 10000; size += 1000)
    {
        int *data = malloc(size * sizeof(int));
        if (!data)
        {
            printf("Ошибка выделения памяти для массива данных.\n");
            return;
        }

        // Генерация случайных данных
        for (int i = 0; i < size; i++)
        {
            data[i] = rand() % 100000; // Случайные числа от 0 до 99999
        }

        // Выбираем случайный ключ для поиска
        int search_key = data[rand() % size];

        // Создаем BST
        TreeNode *bst_root = NULL;
        for (int i = 0; i < size; i++)
        {
            bst_root = insert(bst_root, data[i]);
        }

        // Измеряем время поиска в BST
        clock_t start, end;
        int bst_comparisons = 0;

        start = clock();
        TreeNode *current = bst_root;

        while (current)
        {
            bst_comparisons++;
            if (search_key == current->value)
                break;
            else if (search_key < current->value)
                current = current->left;
            else
                current = current->right;
        }
        end = clock();
        double bst_search_time = (double)(end - start);

        // Подсчет памяти
        size_t bst_memory = size * sizeof(TreeNode);

        // Вывод результатов для BST
        printf("%6d | %18.0f | %20d | %24zu\n", size, bst_search_time, bst_comparisons, bst_memory);

        // Очистка памяти для BST
        free_tree(bst_root);
        free(data);
    }

    printf("-------------------------------------------------------------------------------\n");

    // Таблица для хеш-таблицы с цепочками
    printf("\nТаблица результатов для Хеш-таблицы с цепочками\n");
    printf("Размер | Время поиска (тики) | Количество сравнений | Используемая память (байты)\n");
    printf("-------------------------------------------------------------------------------\n");

    for (int size = 1000; size <= 10000; size += 1000)
    {
        int *data = malloc(size * sizeof(int));
        if (!data)
        {
            printf("Ошибка выделения памяти для массива данных.\n");
            return;
        }

        // Генерация случайных данных
        for (int i = 0; i < size; i++)
        {
            data[i] = rand() % 100000; // Случайные числа от 0 до 99999
        }

        // Выбираем случайный ключ для поиска
        int search_key = data[rand() % size];

        // Создаем хеш-таблицу с цепочками
        HashTable *hash_table = (HashTable *)malloc(sizeof(HashTable));
        hash_table->table = (HashNode **)calloc(size * 2, sizeof(HashNode *));
        hash_table->size = size * 2;
        hash_table->occupied = 0;

        for (int i = 0; i < size; i++)
        {
            insert_hash(hash_table, data[i]);
        }

        // Измеряем время поиска в хеш-таблице
        clock_t start, end;
        int hash_comparisons = 0;

        start = clock();
        search_hash(hash_table, search_key, &hash_comparisons);
        end = clock();
        double hash_search_time = (double)(end - start);

        // Подсчет памяти
        size_t hash_memory = sizeof(HashTable) + hash_table->size * sizeof(HashNode *);
        for (int i = 0; i < hash_table->size; i++)
        {
            HashNode *current = hash_table->table[i];
            while (current)
            {
                hash_memory += sizeof(HashNode);
                current = current->next;
            }
        }

        // Вывод результатов для хеш-таблицы
        printf("%6d | %18.0f | %20d | %24zu\n", size, hash_search_time, hash_comparisons, hash_memory);

        // Очистка памяти для хеш-таблицы
        free_hash_table(hash_table);
        free(data);
    }

    printf("-------------------------------------------------------------------------------\n");

    // Таблица для AVL
    printf("\nТаблица результатов для AVL\n");
    printf("Размер | Время поиска (тики) | Количество сравнений | Используемая память (байты)\n");
    printf("-------------------------------------------------------------------------------\n");

    for (int size = 1000; size <= 10000; size += 1000)
    {
        int *data = malloc(size * sizeof(int));
        if (!data)
        {
            printf("Ошибка выделения памяти для массива данных.\n");
            return;
        }

        // Генерация случайных данных
        for (int i = 0; i < size; i++)
        {
            data[i] = rand() % 100000; // Случайные числа от 0 до 99999
        }

        // Выбираем случайный ключ для поиска
        int search_key = data[rand() % size];

        // Создаем AVL-дерево
        AVLNode *avl_root = NULL;
        for (int i = 0; i < size; i++)
        {
            avl_root = insert_avl_unbalanced(avl_root, data[i]);
        }

        balance_avl_tree(avl_root);

        // Измеряем время поиска в AVL
        clock_t start, end;
        int avl_comparisons = 0;

        start = clock();
        AVLNode *current = avl_root;
        while (current)
        {
            avl_comparisons++;
            if (search_key == current->value)
                break;
            else if (search_key < current->value)
                current = current->left;
            else
                current = current->right;
        }
        end = clock();
        double avl_search_time = end - start;

        // Подсчет памяти
        size_t avl_memory = size * sizeof(AVLNode);

        // Вывод результатов для AVL
        printf("%6d | %18.0f | %20d | %24zu\n", size, avl_search_time, avl_comparisons, avl_memory);

        // Очистка памяти для AVL
        free_avl(avl_root);
        free(data);
    }

    printf("-------------------------------------------------------------------------------\n");

    // Таблица для Хеш-таблицы с линейной адресацией
    printf("\nТаблица результатов для Хеш-таблицы с линейной адресацией\n");
    printf("Размер | Время поиска (тики) | Количество сравнений | Используемая память (байты)\n");
    printf("-------------------------------------------------------------------------------\n");

    for (int size = 1000; size <= 10000; size += 1000)
    {
        int *data = malloc(size * sizeof(int));
        if (!data)
        {
            printf("Ошибка выделения памяти для массива данных.\n");
            return;
        }

        // Генерация случайных данных
        for (int i = 0; i < size; i++)
        {
            data[i] = rand() % 100000; // Случайные числа от 0 до 99999
        }

        // Выбираем случайный ключ для поиска
        int search_key = data[rand() % size];

        // Создаем хеш-таблицу с линейной адресацией
        HashTableLinear *hash_table_linear = create_hash_table_linear(size * 2);

        for (int i = 0; i < size; i++)
        {
            insert_hash_linear(hash_table_linear, data[i]);
        }

        // Измеряем время поиска в хеш-таблице
        clock_t start, end;
        int hash_comparisons = 0;

        start = clock();
        search_hash_linear(hash_table_linear, search_key, &hash_comparisons);
        end = clock();
        double hash_search_time = (double)(end - start);

        // Подсчет памяти
        size_t hash_memory_linear = sizeof(HashTableLinear) + hash_table_linear->size * sizeof(HashNode);

        // Вывод результатов для хеш-таблицы с линейной адресацией
        printf("%6d | %18.0f | %20d | %24zu\n", size, hash_search_time, hash_comparisons, hash_memory_linear);

        // Очистка памяти для хеш-таблицы
        free_hash_table_linear(hash_table_linear);
        free(data);
    }

    printf("-------------------------------------------------------------------------------\n");
}

void store_inorder(TreeNode *node, TreeNode **nodes, int *index)
{
    if (!node)
        return;

    store_inorder(node->left, nodes, index);
    nodes[(*index)++] = node;
    store_inorder(node->right, nodes, index);
}

TreeNode *build_balanced_bst(TreeNode **nodes, int start, int end)
{
    if (start > end)
        return NULL;

    int mid = (start + end) / 2;
    TreeNode *root = nodes[mid];

    root->left = build_balanced_bst(nodes, start, mid - 1);
    root->right = build_balanced_bst(nodes, mid + 1, end);

    return root;
}

int count_nodes(TreeNode *node)
{
    if (!node)
        return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

TreeNode *balance_bst(TreeNode *root)
{
    if (!root)
        return NULL;

    TreeNode *current = root;

    int count = count_nodes(current);

    TreeNode **nodes = (TreeNode **)malloc(count * sizeof(TreeNode *));
    if (!nodes)
    {
        printf("Ошибка выделения памяти для балансировки дерева.\n");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    store_inorder(root, nodes, &index);

    TreeNode *balanced_root = build_balanced_bst(nodes, 0, count - 1);

    return balanced_root;
}

void read_from_file_hash(const char *filename, HashTable *hash_table)
{
    if (!hash_table || !hash_table->table)
    {
        printf("Ошибка: Хеш-таблица не инициализирована.\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла.\n");
        return;
    }

    int value;
    while (fscanf(file, "%d", &value) == 1)
    {
        printf("Чтение значения: %d\n", value); // debugging
        // еслм уже есть, то пропускаем
        if (search_hash(hash_table, value, NULL))
        {
            // printf("Значение %d уже существует в хеш-таблице.\n", value);
            continue;
        }
        else
            insert_hash(hash_table, value);
    }

    if (!feof(file))
    {
        printf("Ошибка чтения файла.\n");
    }

    fclose(file);
}

int main(void)
{
    printf("Программа для работы с бинарным деревом, AVL-деревом и хеш-таблицей.\n");

    TreeNode *bst_root = NULL; // Корень бинарного дерева поиска
    AVLNode *avl_root = NULL;  // Корень AVL-дерева
    int hash_table_size = 101; // Начальный размер хеш-таблицы
    HashTable *hash_table = create_hash_table(hash_table_size);

    srand(time(NULL));
    char filename[256];

    // Ввод имени файла
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

    read_from_file(filename, &bst_root);     // Создаем бинарное дерево поиска
    read_from_file_avl(filename, &avl_root); // Создаем AVL-дерево
    printf("Деревья успешно построены.\n");

    int choice;
    do
    {
        printf("\nВыберите действие:\n");
        printf("1. Вывести BST дерево (визуализация в Graphviz)\n");
        printf("2. Вывести AVL дерево (визуализация в Graphviz)\n");
        printf("3. Добавить число в BST и AVL дерево\n");
        printf("4. Добавить число в хеш-таблицу\n");
        printf("5. Удалить число из BST и AVL дерева\n");
        printf("6. Удалить число из хеш-таблицы\n");
        printf("7. Найти число в BST, AVL дереве и хеш-таблице\n");
        printf("8. Провести замерный эксперимент\n");
        printf("9. Сбалансировать BST дерево\n");
        printf("10. Выполнить рехеширование хеш-таблицы\n");
        printf("11. Показать хеш-таблицу (метод цепочек)\n");
        printf("12. Считать данные из файла в хеш-таблицу\n");
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
        case 1:
        {
            char command[256];
            export_to_dot(bst_root, "bst_tree.dot");
            printf("BST дерево экспортировано в bst_tree.dot.\n");

            snprintf(command, sizeof(command), "dot -Tpng bst_tree.dot -o bst_tree.png");
            if (system(command) == -1)
            {
                printf("Ошибка при создании изображения.\n");
            }
            else
            {
                printf("Изображение BST дерева создано.\n");
            }
            break;
        }
        case 2:
        {
            char command[256];
            avl_root = balance_avl_tree(avl_root);
            export_to_dot_avl(avl_root, "avl_tree.dot");
            printf("AVL дерево экспортировано в avl_tree.dot.\n");

            snprintf(command, sizeof(command), "dot -Tpng avl_tree.dot -o avl_tree.png");
            if (system(command) == -1)
            {
                printf("Ошибка при создании изображения.\n");
            }
            else
            {
                printf("Изображение AVL дерева создано.\n");
            }
            break;
        }
        case 3:
        {
            int new_value;
            printf("Введите число для добавления: ");
            if (fgets(input, sizeof(input), stdin) != NULL && sscanf(input, "%d", &new_value) == 1)
            {
                bst_root = insert(bst_root, new_value);
                avl_root = insert_avl(avl_root, new_value);
                printf("Число добавлено в BST и AVL дерево.\n");
            }
            else
            {
                printf("Ошибка ввода числа.\n");
            }
            break;
        }
        case 4:
        {
            int new_value;
            printf("Введите число для добавления в хеш-таблицу: ");
            if (fgets(input, sizeof(input), stdin) != NULL && sscanf(input, "%d", &new_value) == 1)
            {
                if (search_hash(hash_table, new_value, NULL))
                {
                    printf("Данное число уже есть в хеш-таблице.\n");
                }
                else
                {
                    insert_hash(hash_table, new_value);
                    printf("Число добавлено в хеш-таблицу.\n");
                }
            }
            else
            {
                printf("Ошибка ввода числа.\n");
            }
            break;
        }
        case 5:
        {
            int delete_value;
            printf("Введите число для удаления: ");
            if (fgets(input, sizeof(input), stdin) != NULL && sscanf(input, "%d", &delete_value) == 1)
            {
                bst_root = delete_node(bst_root, delete_value);
                avl_root = delete_node_avl(avl_root, delete_value);
                printf("Число удалено из BST и AVL дерева.\n");
            }
            else
            {
                printf("Ошибка ввода числа.\n");
            }
            break;
        }
        case 6:
        {
            int delete_value;
            printf("Введите число для удаления из хеш-таблицы: ");
            if (fgets(input, sizeof(input), stdin) != NULL && sscanf(input, "%d", &delete_value) == 1)
            {
                delete_hash(hash_table, delete_value);
                printf("Число удалено из хеш-таблицы.\n");
            }
            else
            {
                printf("Ошибка ввода числа.\n");
            }
            break;
        }
        case 7:
        {
            int search_value;
            printf("Введите число для поиска: ");
            if (fgets(input, sizeof(input), stdin) != NULL && sscanf(input, "%d", &search_value) == 1)
            {
                int hash_comparisons = 0;
                int bst_comparisons = 0;
                int avl_comparisons = 0;

                TreeNode *bst_result = search(bst_root, search_value, &bst_comparisons);
                int avl_result = search_avl(avl_root, search_value, &avl_comparisons);
                int hash_result = search_hash(hash_table, search_value, &hash_comparisons);
                printf("Результаты поиска:\n");
                printf("BST: %s кол-во сравнений: %d\n", bst_result ? "найдено" : "не найдено", bst_comparisons);
                printf("AVL: %s кол-во сравнений: %d\n", avl_result ? "найдено" : "не найдено", avl_comparisons);
                printf("Хеш-таблица: %s, кол-во сравнений: %d\n", hash_result ? "найдено" : "не найдено", hash_comparisons);
            }
            else
            {
                printf("Ошибка ввода числа.\n");
            }
            break;
        }
        case 8:
        {
            compare_structures();
            break;
        }
        case 9:
        {
            printf("Сбалансируем BST дерево.\n");
            bst_root = balance_bst(bst_root);
            printf("BST дерево успешно сбалансировано.\n");
            break;
        }
        case 10:
        {
            printf("Выполняется рехеширование хеш-таблицы.\n");
            rehash(&hash_table, &hash_table_size);
            printf("Рехеширование завершено. Новый размер таблицы: %zu\n", hash_table_size);
            break;
        }
        case 11:
        {
            printf("Содержимое хеш-таблицы (закрытая адресация):\n");
            for (size_t i = 0; i < hash_table->size; i++)
            {
                printf("[%zu]: ", i);
                if (hash_table->table[i])
                {
                    HashNode *node = hash_table->table[i];
                    while (node)
                    {
                        printf("%d -> ", node->key);
                        node = node->next;
                    }
                }
                printf("NULL\n");
            }
            break;
        }
        case 12:
        {
            read_from_file_hash(filename, hash_table);
            printf("Данные успешно считаны из файла в хеш-таблицу.\n");
            break;
        }
        case 0:
            printf("Завершение работы программы.\n");
            break;
        default:
            printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 0);

    free_tree(bst_root);
    free_avl(avl_root);
    free_hash_table(hash_table);
    return 0;
}
