#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define MAX_BOOKS 100

// Тип литературы
typedef enum
{
    TECHNICAL,
    FICTION,
    CHILDREN
} 
BookType;

// Техническая литература
typedef struct
{
    char branch[50];
    char origin[20];
    int year;
}
Technical;

// Художественная литература
typedef struct
{
    char genre[30];
}
Fiction;

// Детская литература
typedef struct
{
    int min_age;
    char genre[30];
}
Children;

// Дополнительные поля для каждого типа литературы
typedef union
{
    Technical tech;
    Fiction fiction;
    Children children;
}
BookDetails;

// Структура книги
typedef struct
{
    char author[50];
    char title[100];
    char publisher[50];
    int pages;
    BookType type;
    BookDetails details;
}
Book;

// Функции для работы с базой данных
void add_book(Book books[], int *book_count);
void delete_book_by_title(Book books[], int *book_count, const char *title);
void display_books(Book books[], int book_count);
void display_sorted_books_by_author(Book books[], int book_count);
void display_author_novels(Book books[], int book_count, const char *author);
void measure_sorting_performance(Book books[], int book_count);

// Функции сортировки
void bubble_sort_books(Book books[], int n);
void quicksort_books(Book books[], int count);

// Вспомогательные функции
void print_memory_usage();
void print_table_header();
void print_book_table_row(const Book *book);

int main(void)
{
    int book_count = 40;
    Book books[MAX_BOOKS] = {
        {"Author1", "Technical Book 1", "Publisher1", 100, TECHNICAL, {.tech = {"Computer Science", "Domestic", 2020}}},
        {"Author2", "Technical Book 2", "Publisher2", 150, TECHNICAL, {.tech = {"Electronics", "Translated", 2018}}},
        {"Author3", "Fiction Book 1", "Publisher3", 200, FICTION, {.fiction = {"Novel"}}},
        {"Author4", "Fiction Book 2", "Publisher4", 250, FICTION, {.fiction = {"Poetry"}}},
        {"Author5", "Children Book 1", "Publisher5", 50, CHILDREN, {.children = {5, "Fairy Tales"}}},
        {"Author6", "Children Book 2", "Publisher6", 60, CHILDREN, {.children = {7, "Poems"}}},
        {"Author7", "Technical Book 3", "Publisher7", 120, TECHNICAL, {.tech = {"Physics", "Domestic", 2019}}},
        {"Author8", "Technical Book 4", "Publisher8", 180, TECHNICAL, {.tech = {"Mathematics", "Translated", 2017}}},
        {"Author9", "Fiction Book 3", "Publisher9", 220, FICTION, {.fiction = {"Play"}}},
        {"Author10", "Fiction Book 4", "Publisher10", 270, FICTION, {.fiction = {"Novel"}}},
        {"Author11", "Children Book 3", "Publisher11", 55, CHILDREN, {.children = {6, "Fairy Tales"}}},
        {"Author12", "Children Book 4", "Publisher12", 65, CHILDREN, {.children = {8, "Poems"}}},
        {"Author13", "Technical Book 5", "Publisher13", 130, TECHNICAL, {.tech = {"Chemistry", "Domestic", 2021}}},
        {"Author14", "Technical Book 6", "Publisher14", 190, TECHNICAL, {.tech = {"Biology", "Translated", 2016}}},
        {"Author15", "Fiction Book 5", "Publisher15", 230, FICTION, {.fiction = {"Novel"}}},
        {"Author16", "Fiction Book 6", "Publisher16", 280, FICTION, {.fiction = {"Poetry"}}},
        {"Author17", "Children Book 5", "Publisher17", 70, CHILDREN, {.children = {9, "Fairy Tales"}}},
        {"Author18", "Children Book 6", "Publisher18", 75, CHILDREN, {.children = {10, "Poems"}}},
        {"Author19", "Technical Book 7", "Publisher19", 140, TECHNICAL, {.tech = {"Engineering", "Domestic", 2022}}},
        {"Author20", "Technical Book 8", "Publisher20", 200, TECHNICAL, {.tech = {"Astronomy", "Translated", 2015}}},
        {"Author21", "Fiction Book 7", "Publisher21", 240, FICTION, {.fiction = {"Play"}}},
        {"Author22", "Fiction Book 8", "Publisher22", 290, FICTION, {.fiction = {"Novel"}}},
        {"Author23", "Children Book 7", "Publisher23", 80, CHILDREN, {.children = {11, "Fairy Tales"}}},
        {"Author24", "Children Book 8", "Publisher24", 85, CHILDREN, {.children = {12, "Poems"}}},
        {"Author25", "Technical Book 9", "Publisher25", 150, TECHNICAL, {.tech = {"Geology", "Domestic", 2023}}},
        {"Author26", "Technical Book 10", "Publisher26", 210, TECHNICAL, {.tech = {"Meteorology", "Translated", 2014}}},
        {"Author27", "Fiction Book 9", "Publisher27", 250, FICTION, {.fiction = {"Novel"}}},
        {"Author28", "Fiction Book 10", "Publisher28", 300, FICTION, {.fiction = {"Poetry"}}},
        {"Author29", "Children Book 9", "Publisher29", 90, CHILDREN, {.children = {13, "Fairy Tales"}}},
        {"Author30", "Children Book 10", "Publisher30", 95, CHILDREN, {.children = {14, "Poems"}}},
        {"Author31", "Technical Book 11", "Publisher31", 160, TECHNICAL, {.tech = {"Oceanography", "Domestic", 2024}}},
        {"Author32", "Technical Book 12", "Publisher32", 220, TECHNICAL, {.tech = {"Geography", "Translated", 2013}}},
        {"Author33", "Fiction Book 11", "Publisher33", 260, FICTION, {.fiction = {"Play"}}},
        {"Author34", "Fiction Book 12", "Publisher34", 310, FICTION, {.fiction = {"Novel"}}},
        {"Author35", "Children Book 11", "Publisher35", 100, CHILDREN, {.children = {15, "Fairy Tales"}}},
        {"Author36", "Children Book 12", "Publisher36", 105, CHILDREN, {.children = {16, "Poems"}}},
        {"Author37", "Technical Book 13", "Publisher37", 170, TECHNICAL, {.tech = {"Anthropology", "Domestic", 2025}}},
        {"Author38", "Technical Book 14", "Publisher38", 230, TECHNICAL, {.tech = {"Sociology", "Translated", 2012}}},
        {"Author39", "Fiction Book 13", "Publisher39", 270, FICTION, {.fiction = {"Novel"}}},
        {"Author40", "Fiction Book 14", "Publisher40", 320, FICTION, {.fiction = {"Poetry"}}}
    };

    int choice;

    while (1)
    {
        printf("\nМеню:\n");
        printf("1. Добавить книгу\n");
        printf("2. Удалить книгу по названию\n");
        printf("3. Просмотреть список книг\n");
        printf("4. Просмотреть книги, отсортированные по автору\n");
        printf("5. Измерить производительность сортировок\n");
        printf("6. Поиск романов автора\n");
        printf("7. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            add_book(books, &book_count);
            break;
        case 2:
        {
            char title[100];
            printf("Введите название книги для удаления: ");
            fgets(title, 100, stdin);
            title[strcspn(title, "\n")] = '\0';
            delete_book_by_title(books, &book_count, title);
            break;
        }
        case 3:
            display_books(books, book_count);
            break;
        case 4:
            display_sorted_books_by_author(books, book_count);
            break;
        case 5:
            measure_sorting_performance(books, book_count);
            break;
        case 6:
        {
            char author[50];
            printf("Введите фамилию автора: ");
            fgets(author, 50, stdin);
            author[strcspn(author, "\n")] = '\0';
            display_author_novels(books, book_count, author);
            break;
        }
        case 7:
            exit(0);
        default:
            printf("Неверный выбор.\n");
        }
    }

    return 0;
}

// Функция добавления книги
void add_book(Book books[], int *book_count)
{
    if (*book_count >= MAX_BOOKS)
    {
        printf("База данных переполнена.\n");
        return;
    }

    Book new_book;

    printf("Введите фамилию автора: ");
    fgets(new_book.author, 50, stdin);
    new_book.author[strcspn(new_book.author, "\n")] = '\0';

    printf("Введите название книги: ");
    fgets(new_book.title, 100, stdin);
    new_book.title[strcspn(new_book.title, "\n")] = '\0';

    printf("Введите издательство: ");
    fgets(new_book.publisher, 50, stdin);
    new_book.publisher[strcspn(new_book.publisher, "\n")] = '\0';

    printf("Введите количество страниц: ");
    while (scanf("%d", &new_book.pages) != 1 || new_book.pages <= 0)
    {
        printf("Ошибка: Введите положительное целое число страниц: ");
        while (getchar() != '\n')
            ; // Очистка буфера
    }
    getchar();

    printf("Выберите тип литературы (0 - техническая, 1 - художественная, 2 - детская): ");
    while (scanf("%d", (int *)&new_book.type) != 1 || new_book.type < 0 || new_book.type > 2)
    {
        printf("Ошибка: Введите корректный тип (0, 1 или 2): ");
        while (getchar() != '\n')
            ;
    }
    getchar();

    switch (new_book.type)
    {
    case TECHNICAL:
        printf("Введите отрасль: ");
        fgets(new_book.details.tech.branch, 50, stdin);
        new_book.details.tech.branch[strcspn(new_book.details.tech.branch, "\n")] = '\0';

        printf("Введите происхождение (Отечественная/Переводная): ");
        fgets(new_book.details.tech.origin, 20, stdin);
        new_book.details.tech.origin[strcspn(new_book.details.tech.origin, "\n")] = '\0';

        printf("Введите год издания: ");
        while (scanf("%d", &new_book.details.tech.year) != 1 || new_book.details.tech.year < 0)
        {
            printf("Ошибка: Введите положительное число (год): ");
            while (getchar() != '\n')
                ;
        }
        getchar();
        break;

    case FICTION:
        printf("Введите жанр (роман, пьеса, поэзия): ");
        fgets(new_book.details.fiction.genre, 30, stdin);
        new_book.details.fiction.genre[strcspn(new_book.details.fiction.genre, "\n")] = '\0';
        break;

    case CHILDREN:
        printf("Введите минимальный возраст: ");
        while (scanf("%d", &new_book.details.children.min_age) != 1 || new_book.details.children.min_age < 0)
        {
            printf("Ошибка: Введите положительное число (возраст): ");
            while (getchar() != '\n')
                ;
        }
        getchar();

        printf("Введите тип детской литературы (стихи, сказки): ");
        fgets(new_book.details.children.genre, 30, stdin);
        new_book.details.children.genre[strcspn(new_book.details.children.genre, "\n")] = '\0';
        break;
    }

    books[(*book_count)++] = new_book;
    printf("Книга добавлена успешно!\n");
}

// Функция удаления книги по названию
void delete_book_by_title(Book books[], int *book_count, const char *title)
{
    int found = 0;
    for (int i = 0; i < *book_count; i++)
    {
        if (strcmp(books[i].title, title) == 0)
        {
            found = 1;
            for (int j = i; j < *book_count - 1; j++)
            {
                books[j] = books[j + 1];
            }
            (*book_count)--;
            printf("Книга удалена успешно.\n");
            return;
        }
    }
    if (!found)
    {
        printf("Книга с названием \"%s\" не найдена.\n", title);
    }
}

void display_author_novels(Book books[], int book_count, const char *author)
{
    int found = 0;
    printf("\nРоманы автора %s:\n", author);
    for (int i = 0; i < book_count; i++)
    {
        if (strcmp(books[i].author, author) == 0 && books[i].type == FICTION &&
            strcmp(books[i].details.fiction.genre, "роман") == 0)
        {
            printf("Название: %s, Издательство: %s, Страниц: %d\n",
                   books[i].title, books[i].publisher, books[i].pages);
            found = 1;
        }
    }

    if (!found)
    {
        printf("Романы не найдены.\n");
    }
}

// Функции сортировки и измерения производительности
void bubble_sort_books(Book books[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1 - i; j++)
        {
            if (strcmp(books[j].author, books[j + 1].author) > 0)
            {
                Book temp = books[j];
                books[j] = books[j + 1];
                books[j + 1] = temp;
            }
        }
    }
}

// Функция сравнения для qsort
int compare_books_by_author(const void *a, const void *b)
{
    const Book *book1 = (const Book *)a;
    const Book *book2 = (const Book *)b;
    return strcmp(book1->author, book2->author);
}

// Функция быстрой сортировки с использованием qsort
void quicksort_books(Book books[], int count)
{
    qsort(books, count, sizeof(Book), compare_books_by_author);
}

void display_sorted_books_by_author(Book books[], int book_count)
{
    Book temp_books[MAX_BOOKS];
    memcpy(temp_books, books, sizeof(Book) * book_count);

    bubble_sort_books(temp_books, book_count);
    printf("\nКниги, отсортированные по автору (пузырьковая сортировка):\n");
    display_books(temp_books, book_count);

    memcpy(temp_books, books, sizeof(Book) * book_count);
    quicksort_books(temp_books, book_count);
    printf("\nКниги, отсортированные по автору (быстрая сортировка):\n");
    display_books(temp_books, book_count);
}

// Функция измерения производительности сортировок
void measure_sorting_performance(Book books[], int book_count)
{
    struct rusage usage;

    Book temp_books[MAX_BOOKS];
    memcpy(temp_books, books, sizeof(Book) * book_count);

    clock_t start_time = clock();
    bubble_sort_books(temp_books, book_count);
    clock_t end_time = clock();
    printf("\nПузырьковая сортировка:\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    getrusage(RUSAGE_SELF, &usage);
    printf("Использование памяти: %ld KB\n", usage.ru_maxrss);

    memcpy(temp_books, books, sizeof(Book) * book_count);

    start_time = clock();
    quicksort_books(temp_books, book_count);
    end_time = clock();
    printf("\nБыстрая сортировка:\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    getrusage(RUSAGE_SELF, &usage);
    printf("Использование памяти: %ld KB\n", usage.ru_maxrss);
}

// Вспомогательные функции для вывода книг
void display_books(Book books[], int book_count)
{
    print_table_header();
    for (int i = 0; i < book_count; i++)
    {
        print_book_table_row(&books[i]);
    }
}

void print_table_header()
{
    printf("%-20s %-30s %-20s %-10s\n", "Автор", "Название", "Издательство", "Страницы");
    printf("------------------------------------------------------------\n");
}

void print_book_table_row(const Book *book)
{
    printf("%-20s %-30s %-20s %-10d\n", book->author, book->title, book->publisher, book->pages);
}

void print_memory_usage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Использование памяти: %ld KB\n", usage.ru_maxrss);
}
