#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_VERTICES 1000
#define MAX_NAME_LEN 50
#define BUFFER_SIZE 256

typedef struct AdjListNode
{
    int dest;
    struct AdjListNode *next;
} AdjListNode;

typedef struct AdjList
{
    AdjListNode *head;
} AdjList;

typedef struct Graph
{
    int numVertices;
    char vertexNames[MAX_VERTICES][MAX_NAME_LEN];
    AdjList *array;
} Graph;

typedef struct Stack
{
    int *items;
    int top;
    int capacity;
} Stack;

Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack)
    {
        printf("Ошибка выделения памяти для стека.\n");
        exit(1);
    }
    stack->items = (int *)malloc(sizeof(int) * capacity);
    if (!stack->items)
    {
        printf("Ошибка выделения памяти для элементов стека.\n");
        free(stack);
        exit(1);
    }
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

void push(Stack *stack, int item)
{
    if (stack->top == stack->capacity - 1)
    {
        stack->capacity *= 2;
        stack->items = (int *)realloc(stack->items, sizeof(int) * stack->capacity);
        if (!stack->items)
        {
            printf("Ошибка перераспределения памяти для стека.\n");
            free(stack);
            exit(1);
        }
    }
    stack->items[++stack->top] = item;
}

int pop(Stack *stack)
{
    if (stack->top == -1)
        return -1;
    return stack->items[stack->top--];
}

int isEmpty(Stack *stack)
{
    return stack->top == -1;

}
void freeStack(Stack *stack)
{
    free(stack->items);
    free(stack);
}

// Функция для создания узла списка смежности
AdjListNode *newAdjListNode(int dest)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    if (!newNode)
    {
        printf("Ошибка выделения памяти для узла списка смежности.\n");
        exit(1);
    }
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// Функция для создания графа
Graph *createGraph(int numVertices)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (!graph)
    {
        printf("Ошибка выделения памяти для графа.\n");
        exit(1);
    }
    graph->numVertices = numVertices;
    graph->array = (AdjList *)malloc(sizeof(AdjList) * numVertices);
    if (!graph->array)
    {
        printf("Ошибка выделения памяти для списка смежности.\n");
        free(graph);
        exit(1);
    }
    for (int i = 0; i < numVertices; ++i)
        graph->array[i].head = NULL;
    return graph;
}

// Функция для добавления ребра
void addEdge(Graph *graph, int src, int dest)
{
    // Добавление ребра из src в dest
    AdjListNode *newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Добавление ребра из dest в src (неориентированный граф)
    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// Функция для поиска индекса вершины по имени
int findVertexIndex(Graph *graph, char name[])
{
    for (int i = 0; i < graph->numVertices; ++i)
    {
        if (strcmp(graph->vertexNames[i], name) == 0)
            return i;
    }
    return -1;
}

// Функция для очистки всех ребер графа
void clearEdges(Graph *graph)
{
    if (graph == NULL)
        return;
    for (int i = 0; i < graph->numVertices; ++i)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            AdjListNode *toFree = temp;
            temp = temp->next;
            free(toFree);
        }
        graph->array[i].head = NULL;
    }
}

// Алгоритм Хиерголзера для поиска Эйлова пути
int *findEulerPath(Graph *graph, int *pathSize)
{
    // Сначала подсчитаем общее количество ребер
    int totalEdges = 0;
    for (int i = 0; i < graph->numVertices; ++i)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            totalEdges++;
            temp = temp->next;
        }
    }
    totalEdges /= 2; // Так как граф неориентированный

    // Проверка на количество вершин с нечетной степенью
    int odd = 0;
    int start = 0;
    for (int i = 0; i < graph->numVertices; ++i)
    {
        int degree = 0;
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            degree++;
            temp = temp->next;
        }
        if (degree % 2 != 0)
        {
            odd++;
            start = i;
        }
    }

    if (odd != 0 && odd != 2)
        return NULL; // Эйлов путь не существует

    if (odd == 0)
    {
        start = 0; // Любая вершина для цикла
    }

    // Копирование графа для модификации без дублирования ребер
    Graph *localGraph = createGraph(graph->numVertices);
    for (int i = 0; i < graph->numVertices; ++i)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            if (i < temp->dest)
            { // Добавляем ребро только один раз
                addEdge(localGraph, i, temp->dest);
            }
            temp = temp->next;
        }
    }

    // Стек для хранения пути
    Stack *stack = createStack(1000);
    push(stack, start);

    // Выделение памяти для пути
    int *path = (int *)malloc(sizeof(int) * (totalEdges + 1));
    if (!path)
    {
        printf("Ошибка выделения памяти для пути.\n");
        freeStack(stack);
        // Освобождение localGraph
        for (int i = 0; i < localGraph->numVertices; ++i)
        {
            AdjListNode *temp = localGraph->array[i].head;
            while (temp)
            {
                AdjListNode *toFree = temp;
                temp = temp->next;
                free(toFree);
            }
        }
        free(localGraph->array);
        free(localGraph);
        return NULL;
    }
    *pathSize = 0;

    while (!isEmpty(stack))
    {
        int u = stack->items[stack->top];
        if (localGraph->array[u].head != NULL)
        {
            int v = localGraph->array[u].head->dest;
            push(stack, v);
            // Удаление ребра из localGraph
            // Удаляем u->v
            AdjListNode *temp = localGraph->array[u].head;
            localGraph->array[u].head = temp->next;
            free(temp);
            // Удаляем v->u
            AdjListNode *prev = NULL;
            temp = localGraph->array[v].head;
            while (temp)
            {
                if (temp->dest == u)
                    break;
                prev = temp;
                temp = temp->next;
            }
            if (temp)
            {
                if (prev)
                    prev->next = temp->next;
                else
                    localGraph->array[v].head = temp->next;
                free(temp);
            }
        }
        else
        {
            int v = pop(stack);
            path[(*pathSize)++] = v;
        }
    }

    // Проверка, является ли путь Эйловым
    if (*pathSize != totalEdges + 1)
    {
        free(path);
        path = NULL;
    }
    else
    {
        // Инвертируем путь
        for (int i = 0; i < *pathSize / 2; ++i)
        {
            int tempVal = path[i];
            path[i] = path[*pathSize - i - 1];
            path[*pathSize - i - 1] = tempVal;
        }
    }

    // Освобождение ресурсов
    for (int i = 0; i < localGraph->numVertices; ++i)
    {
        AdjListNode *temp = localGraph->array[i].head;
        while (temp)
        {
            AdjListNode *toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
    free(localGraph->array);
    free(localGraph);
    freeStack(stack);

    return path;
}

// Функция для генерации файла DOT и создания изображения графа
void generateDOT(Graph *graph, int *path, int pathSize, char filename[])
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Не удалось создать файл %s\n", filename);
        return;
    }

    fprintf(file, "graph G {\n");
    fprintf(file, "    node [shape=circle];\n");

    // Сначала определим все вершины, чтобы даже без рёбер они были отображены
    for (int i = 0; i < graph->numVertices; ++i)
    {
        // Проверяем, есть ли у вершины рёбра
        if (graph->array[i].head == NULL)
        {
            // Вершина без рёбер
            fprintf(file, "    \"%s\";\n", graph->vertexNames[i]);
        }
    }

    // Добавление всех рёбер
    for (int i = 0; i < graph->numVertices; ++i)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            if (i < temp->dest)
            { // Чтобы не дублировать рёбра
                fprintf(file, "    \"%s\" -- \"%s\";\n", graph->vertexNames[i], graph->vertexNames[temp->dest]);
            }
            temp = temp->next;
        }
    }

    // Если есть путь, выделим его цветом
    if (path != NULL && pathSize > 1)
    {
        fprintf(file, "    edge [color=red, penwidth=2.0];\n");
        for (int i = 0; i < pathSize - 1; ++i)
        {
            fprintf(file, "    \"%s\" -- \"%s\";\n", graph->vertexNames[path[i]], graph->vertexNames[path[i + 1]]);
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    // Генерация изображения графа с помощью Graphviz
    char command[300];
    snprintf(command, sizeof(command), "dot -Tpng %s -o graph_output.png", filename);
    int ret = system(command);
    if (ret == -1)
    {
        printf("Ошибка при выполнении команды Graphviz.\n");
    }
    else
    {
        printf("Файл изображения сгенерирован: graph_output.png\n");
    }
}

void freeGraph(Graph *graph)
{
    if (graph == NULL)
        return;
    for (int i = 0; i < graph->numVertices; ++i)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp)
        {
            AdjListNode *toFree = temp;
            temp = temp->next;
            free(toFree);
        }
    }
    free(graph->array);
    free(graph);
}

// Функция для очистки пути из памяти
void freePath(int *path)
{
    if (path)
        free(path);
}

// Функция для отображения меню
void displayMenu()
{
    printf("\n--- Меню программы ---\n");
    printf("1. Ввести количество городов (вершин)\n");
    printf("2. Ввести названия городов\n");
    printf("3. Ввести дороги (ребра)\n");
    printf("4. Найти Эйлов путь\n");
    printf("5. Визуализировать граф\n");
    printf("6. Выход\n");
    printf("Выберите опцию (1-6): ");
}

// Функция для чтения строки с использованием fgets и обработки новой строки
int readLine(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) == NULL)
        return 0;
    // Удаление символа новой строки, если он есть
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    return 1;
}

int main(void)
{
    Graph *graph = NULL;
    int pathSize = 0;
    int *path = NULL;

    while (1)
    {
        displayMenu();
        char buffer[BUFFER_SIZE];
        if (!readLine(buffer, BUFFER_SIZE))
        {
            printf("Ошибка чтения ввода.\n");
            continue;
        }

        int option;
        if (sscanf(buffer, "%d", &option) != 1)
        {
            printf("Неверный ввод. Пожалуйста, введите число от 1 до 6.\n");
            continue;
        }

        switch (option)
        {
        case 1:
        {
            if (graph != NULL)
            {
                freeGraph(graph);
                graph = NULL;
            }
            if (path != NULL)
            {
                freePath(path);
                path = NULL;
                pathSize = 0;
            }
            printf("Введите количество городов (вершин): ");
            if (!readLine(buffer, BUFFER_SIZE))
            {
                printf("Ошибка чтения ввода.\n");
                break;
            }
            int numVertices;
            if (sscanf(buffer, "%d", &numVertices) != 1 || numVertices <= 0 || numVertices > MAX_VERTICES)
            {
                printf("Неверный ввод количества городов. Ожидалось положительное целое число до %d.\n", MAX_VERTICES);
                break;
            }
            graph = createGraph(numVertices);
            printf("Количество городов установлено на %d.\n", numVertices);
            break;
        }
        case 2:
        {
            if (graph == NULL)
            {
                printf("Сначала введите количество городов (опция 1).\n");
                break;
            }
            printf("Введите названия городов по одному. Максимальная длина имени: %d символов.\n", MAX_NAME_LEN - 1);
            for (int i = 0; i < graph->numVertices; ++i)
            {
                printf("Город %d: ", i + 1);
                if (!readLine(buffer, BUFFER_SIZE))
                {
                    printf("Ошибка чтения ввода.\n");
                    i--; // Повторить ввод для этого города
                    continue;
                }
                if (sscanf(buffer, "%49s", graph->vertexNames[i]) != 1)
                { // Ограничение на длину
                    printf("Неверный ввод названия города. Попробуйте снова.\n");
                    i--; // Повторить ввод для этого города
                    continue;
                }
            }
            printf("Названия городов успешно введены.\n");
            break;
        }
        case 3:
        {
            if (graph == NULL)
            {
                printf("Сначала введите количество городов и их названия (опции 1 и 2).\n");
                break;
            }
            // Очистка существующих дорог перед добавлением новых
            clearEdges(graph);
            printf("Существующие дороги удалены.\n");

            // Освобождение ранее найденного пути, так как граф изменился
            if (path != NULL)
            {
                freePath(path);
                path = NULL;
                pathSize = 0;
            }

            printf("Введите количество дорог (ребер): ");
            if (!readLine(buffer, BUFFER_SIZE))
            {
                printf("Ошибка чтения ввода.\n");
                break;
            }
            int numEdges;
            if (sscanf(buffer, "%d", &numEdges) != 1 || numEdges < 0)
            {
                printf("Неверный ввод количества дорог. Ожидалось неотрицательное целое число.\n");
                break;
            }
            printf("Введите дороги в формате 'город1 город2' по одной на строку:\n");
            for (int i = 0; i < numEdges; ++i)
            {
                printf("Дорога %d: ", i + 1);
                if (!readLine(buffer, BUFFER_SIZE))
                {
                    printf("Ошибка чтения ввода.\n");
                    i--; // Повторить ввод для этой дороги
                    continue;
                }
                char srcName[MAX_NAME_LEN], destName[MAX_NAME_LEN];
                if (sscanf(buffer, "%49s %49s", srcName, destName) != 2)
                {
                    printf("Неверный формат ввода. Ожидалось два названия городов.\n");
                    i--; // Повторить ввод для этой дороги
                    continue;
                }
                int src = findVertexIndex(graph, srcName);
                int dest = findVertexIndex(graph, destName);
                if (src == -1 || dest == -1)
                {
                    printf("Города '%s' или '%s' не найдены среди введенных.\n", srcName, destName);
                    i--; // Повторить ввод для этой дороги
                    continue;
                }
                addEdge(graph, src, dest);
            }
            printf("Дороги успешно введены.\n");
            break;
        }
        case 4:
        {
            if (graph == NULL || graph->numVertices == 0)
            {
                printf("Сначала введите граф (опции 1-3).\n");
                break;
            }
            if (path != NULL)
            {
                freePath(path);
                path = NULL;
            }
            path = findEulerPath(graph, &pathSize);
            if (path)
            {
                printf("Найден Эйлов путь:\n");
                for (int i = 0; i < pathSize; ++i)
                {
                    printf("%s", graph->vertexNames[path[i]]);
                    if (i != pathSize - 1)
                        printf(" -> ");
                }
                printf("\n");
            }
            else
            {
                printf("Эйлов путь не существует в данной системе дорог.\n");
            }
            break;
        }
        case 5:
        {
            if (graph == NULL)
            {
                printf("Сначала введите граф (опции 1-3).\n");
                break;
            }
            char filename[100] = "graph_output.dot";
            generateDOT(graph, path, pathSize, filename);
            // Генерация изображения уже происходит внутри функции generateDOT
            break;
        }
        case 6:
        {
            printf("Выход из программы.\n");
            // Освобождение памяти
            freeGraph(graph);
            freePath(path);
            exit(0);
        }
        default:
        {
            printf("Неверный выбор. Пожалуйста, выберите опцию от 1 до 6.\n");
        }
        }
    }

    return 0;
}
