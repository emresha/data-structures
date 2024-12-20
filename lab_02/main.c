#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define MAX_BOOKS 5000
#define MAX_BOOKS_SORT 10000

typedef enum
{
    TECHNICAL, // техническая литература
    FICTION,   // художественная
    CHILDREN   // детская
} BookType;

typedef struct
{
    char branch[50]; // отрасль
    char origin[50]; // отечественная / переводная
    int year;        // год издания
} Technical;

typedef struct
{
    char genre[30]; // жанр
} Fiction;

typedef struct
{
    int min_age;    // мин. возраст
    char genre[30]; // жанр
} Children;

typedef union
{
    Technical tech;
    Fiction fiction;
    Children children;
} BookDetails;

typedef struct
{
    char author[50];     // автор
    char title[100];     // название
    char publisher[50];  // издатель
    int pages;           // кол-во страниц
    BookType type;       // тип книги
    BookDetails details; // детали
} Book;

typedef struct
{
    int index;
    char author[50];
} AuthorIndex;

void add_book(Book books[], int *book_count);
void delete_book_by_title(Book books[], int *book_count, const char *title);
void display_books(Book books[], int book_count);
void display_sorted_books_by_author(Book books[], int book_count);
void display_sorted_books_using_author_table(Book books[], int book_count, AuthorIndex author_table[]);
void measure_sorting_performance(void);
void display_author_fiction_books(Book books[], int book_count);

void quicksort_books(Book books[], int count);
void quicksort_author_table(AuthorIndex author_table[], int count);

void print_memory_usage();
void print_table_header();
void print_sorted_index(AuthorIndex author_table[], int count);
void print_book_table_row(const Book *book);
int compare_books_by_author(const void *a, const void *b);
int compare_author_index(const void *a, const void *b);

void generate_random_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (size_t i = 0; i < size - 1; i++)
    {
        str[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    str[size - 1] = '\0';
}

void generate_random_book(Book *book)
{
    generate_random_string(book->author, sizeof(book->author));
    generate_random_string(book->title, sizeof(book->title));
    generate_random_string(book->publisher, sizeof(book->publisher));
    book->pages = rand() % 1000 + 50;
    book->type = rand() % 3;
}

void print_author_table(const AuthorIndex author_table[], int count)
{
    printf("\nТаблица авторов (ключи):\n");
    printf("| %-5s | %-20s |\n", "Ключ", "Автор");
    printf("+-------+----------------------+\n");
    for (int i = 0; i < count; i++)
    {
        printf("| %-5d | %-20s |\n", author_table[i].index, author_table[i].author);
    }
}

void create_author_index(const Book books[], AuthorIndex author_table[], int book_count)
{
    for (int i = 0; i < book_count; i++)
    {
        author_table[i].index = i;
        strcpy(author_table[i].author, books[i].author);
    }
}

void print_table_header_id();

void print_book_table_row_id(const Book *book, int id);

void show_table_by_key_table(AuthorIndex *author_table, Book *books, int book_count)
{
    print_table_header_id();
    for (int i = 0; i < book_count; i++)
    {
        print_book_table_row_id(&books[author_table[i].index], author_table[i].index);
    }
}

int main(void)
{
    setlocale(LC_ALL, "UTF-8");
    srand((unsigned int)time(NULL));
    
    printf("Программа для взаимодействия с массивом книг, вы можете добавлять, удалять, искать книги.\nТакже имеется возможность измерения производительности разного вида сортировок.");


    int book_count = 40;
    Book books[MAX_BOOKS] = {
        {"Newton", "Principia Mathematica", "Royal Society", 500, TECHNICAL, {.tech = {"Physics", "Translated", 1687}}},
        {"Turing", "Computing Machinery and Intelligence", "Oxford Press", 150, TECHNICAL, {.tech = {"Computer Science", "Domestic", 1950}}},
        {"Orwell", "1984", "Secker & Warburg", 328, FICTION, {.fiction = {"Dystopian"}}},
        {"Tolkien", "The Lord of the Rings", "Allen & Unwin", 1178, FICTION, {.fiction = {"Fantasy"}}},
        {"Seuss", "The Cat in the Hat", "Random House", 72, CHILDREN, {.children = {4, "Rhyming"}}},
        {"Carroll", "Alice's Adventures in Wonderland", "Macmillan", 96, CHILDREN, {.children = {7, "Fantasy"}}},
        {"Einstein", "Relativity: The Special and General Theory", "Henry Holt", 168, TECHNICAL, {.tech = {"Physics", "Translated", 1916}}},
        {"Curie", "Radioactive Substances", "French Academy of Sciences", 120, TECHNICAL, {.tech = {"Chemistry", "Translated", 1903}}},
        {"Austen", "Pride and Prejudice", "T. Egerton", 279, FICTION, {.fiction = {"Romance"}}},
        {"Tolstoy", "War and Peace", "The Russian Messenger", 1225, FICTION, {.fiction = {"Historical"}}},
        {"Rowling", "Harry Potter and the Philosopher's Stone", "Bloomsbury", 223, CHILDREN, {.children = {9, "Fantasy"}}},
        {"Milne", "Winnie the Pooh", "Methuen & Co.", 160, CHILDREN, {.children = {5, "Fantasy"}}},
        {"Hawking", "A Brief History of Time", "Bantam Books", 256, TECHNICAL, {.tech = {"Cosmology", "Domestic", 1988}}},
        {"Sagan", "Cosmos", "Random House", 365, TECHNICAL, {.tech = {"Astronomy", "Domestic", 1980}}},
        {"García Márquez", "One Hundred Years of Solitude", "Harper & Row", 417, FICTION, {.fiction = {"Magic Realism"}}},
        {"Dostoevsky", "Crime and Punishment", "The Russian Messenger", 671, FICTION, {.fiction = {"Psychological"}}},
        {"Dahl", "Charlie and the Chocolate Factory", "Alfred A. Knopf", 176, CHILDREN, {.children = {8, "Adventure"}}},
        {"Lewis", "The Chronicles of Narnia", "Geoffrey Bles", 206, CHILDREN, {.children = {9, "Fantasy"}}},
        {"Darwin", "On the Origin of Species", "John Murray", 502, TECHNICAL, {.tech = {"Biology", "Domestic", 1859}}},
        {"Feynman", "The Feynman Lectures on Physics", "Addison-Wesley", 1552, TECHNICAL, {.tech = {"Physics", "Domestic", 1964}}},
        {"Lee", "To Kill a Mockingbird", "J.B. Lippincott & Co.", 281, FICTION, {.fiction = {"Southern Gothic"}}},
        {"Twain", "The Adventures of Huckleberry Finn", "Chatto & Windus", 366, FICTION, {.fiction = {"Adventure"}}},
        {"White", "Charlotte's Web", "Harper & Brothers", 192, CHILDREN, {.children = {6, "Animal Fiction"}}},
        {"Baum", "The Wonderful Wizard of Oz", "George M. Hill Company", 272, CHILDREN, {.children = {7, "Fantasy"}}},
        {"Tesla", "My Inventions", "Electrical Experimenter", 144, TECHNICAL, {.tech = {"Electrical Engineering", "Domestic", 1919}}},
        {"Maxwell", "A Treatise on Electricity and Magnetism", "Clarendon Press", 550, TECHNICAL, {.tech = {"Physics", "Domestic", 1873}}},
        {"Melville", "Moby-Dick", "Harper & Brothers", 635, FICTION, {.fiction = {"Adventure"}}},
        {"Kafka", "The Metamorphosis", "Kurt Wolff Verlag", 201, FICTION, {.fiction = {"Absurdist"}}},
        {"Potter", "The Tale of Peter Rabbit", "Frederick Warne & Co.", 72, CHILDREN, {.children = {3, "Animal Fiction"}}},
        {"Saint-Exupéry", "The Little Prince", "Reynal & Hitchcock", 96, CHILDREN, {.children = {6, "Fantasy"}}},
        {"Freud", "The Interpretation of Dreams", "Franz Deuticke", 600, TECHNICAL, {.tech = {"Psychology", "Translated", 1899}}},
        {"Babbage", "Passages from the Life of a Philosopher", "Longman, Green", 438, TECHNICAL, {.tech = {"Mathematics", "Domestic", 1864}}},
        {"Hemingway", "The Old Man and the Sea", "Charles Scribner's Sons", 127, FICTION, {.fiction = {"Adventure"}}},
        {"Woolf", "Mrs Dalloway", "Hogarth Press", 296, FICTION, {.fiction = {"Modernist"}}},
        {"Sendak", "Where the Wild Things Are", "Harper & Row", 48, CHILDREN, {.children = {4, "Fantasy"}}},
        {"Carroll", "Through the Looking-Glass", "Macmillan", 224, CHILDREN, {.children = {8, "Fantasy"}}},
        {"Edison", "The Diary of Thomas Edison", "Harper & Brothers", 350, TECHNICAL, {.tech = {"Inventions", "Domestic", 1930}}},
        {"Mendel", "Experiments on Plant Hybridization", "Verhandlungen", 150, TECHNICAL, {.tech = {"Genetics", "Translated", 1866}}},
        {"Shakespeare", "Hamlet", "Oxford University Press", 400, FICTION, {.fiction = {"Tragedy"}}},
        {"Verne", "Twenty Thousand Leagues Under the Sea", "Pierre-Jules Hetzel", 437, FICTION, {.fiction = {"Science Fiction"}}}};

    int choice;

    while (1)
    {
        printf("\nМеню:\n");
        printf("1. Добавить книгу\n");
        printf("2. Удалить книгу по названию\n");
        printf("3. Просмотреть список книг\n");
        printf("4. Просмотреть таблицу ключей\n");
        printf("5. Просмотреть книги, отсортированные по автору (обычная сортировка)\n");
        printf("6. Измерить производительность сортировок\n");
        printf("7. Просмотреть книги, отсортированные по автору (таблица авторов)\n");
        printf("8. Показать все романы автора\n");
        printf("9. Отсортировать массив индексов\n");
        printf("10. Вывести обычную таблицу по таблице ключей\n");
        printf("11. Выход\n");
        printf("Ваш выбор: ");
        char choice_input[10];
        fgets(choice_input, sizeof(choice_input), stdin);
        if (sscanf(choice_input, "%d", &choice) != 1)
        {
            printf("Ошибка: Введите число.\n");
            continue;
        }

        AuthorIndex author_table[MAX_BOOKS];
        create_author_index(books, author_table, book_count);

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
            print_author_table(author_table, book_count);
            break;
        case 5:
            display_sorted_books_by_author(books, book_count);
            break;
        case 6:
            measure_sorting_performance();
            break;
        case 7:
            display_sorted_books_using_author_table(books, book_count, author_table);
            break;
        case 8:
            display_author_fiction_books(books, book_count);
            break;
        case 9:
            print_sorted_index(author_table, book_count);
            break;
        case 10:
            show_table_by_key_table(author_table, books, book_count);
            break;
        case 11:
            exit(0);
        default:
            printf("Неверный выбор.\n");
        }
    }

    return 0;
}

void display_author_fiction_books(Book books[], int book_count)
{
    char author[50];
    printf("Введите фамилию автора: ");
    fgets(author, 50, stdin);
    author[strcspn(author, "\n")] = '\0';

    int found = 0;
    print_table_header();

    for (int i = 0; i < book_count; i++)
    {
        if (books[i].type == FICTION && strcmp(books[i].author, author) == 0)
        {
            print_book_table_row(&books[i]);
            found = 1;
        }
    }

    if (!found)
    {
        printf("Романы автора \"%s\" не найдены.\n", author);
    }

    return;
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
    if (strlen(new_book.author) == 0)
    {
        printf("Ошибка: Фамилия автора не может быть пустой.\n");
        return;
    }

    printf("Введите название книги: ");
    fgets(new_book.title, 100, stdin);
    new_book.title[strcspn(new_book.title, "\n")] = '\0';
    if (strlen(new_book.title) == 0)
    {
        printf("Ошибка: Название книги не может быть пустым.\n");
        return;
    }

    printf("Введите издательство: ");
    fgets(new_book.publisher, 50, stdin);
    new_book.publisher[strcspn(new_book.publisher, "\n")] = '\0';
    if (strlen(new_book.publisher) == 0)
    {
        printf("Ошибка: Издательство не может быть пустым.\n");
        return;
    }

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
        if (strlen(new_book.details.tech.branch) == 0)
        {
            printf("Ошибка: Отрасль не может быть пустой.\n");
            return;
        }

        while (1)
        {
            char origin_input[10];
            int origin_choice;
            printf("Введите происхождение (0 - Отечественная, 1 - Переводная): ");
            fgets(origin_input, sizeof(origin_input), stdin);
            if (sscanf(origin_input, "%d", &origin_choice) == 1 && (origin_choice == 0 || origin_choice == 1))
            {
                if (origin_choice == 0)
                {
                    strcpy(new_book.details.tech.origin, "Отечественная");
                }
                else
                {
                    strcpy(new_book.details.tech.origin, "Переводная");
                }
                break;
            }
            else
            {
                printf("Ошибка: Введите корректное значение (0 или 1).\n");
            }
        }

        while (1)
        {
            char year_input[10];
            printf("Введите год издания: ");
            fgets(year_input, sizeof(year_input), stdin);
            if (sscanf(year_input, "%d", &new_book.details.tech.year) == 1 && new_book.details.tech.year > 0)
            {
                break;
            }
            else
            {
                printf("Ошибка: Введите положительное число (год).\n");
            }
        }
        break;

    case FICTION:
        printf("Выберите жанр:\n");
        printf("1. Роман\n");
        printf("2. Пьеса\n");
        printf("3. Поэзия\n");
        char genre_input[10];
        int genre_choice;
        while (1)
        {
            printf("Введите номер жанра (1, 2 или 3): ");
            fgets(genre_input, sizeof(genre_input), stdin);
            if (sscanf(genre_input, "%d", &genre_choice) == 1 && genre_choice >= 1 && genre_choice <= 3)
            {
                switch (genre_choice)
                {
                case 1:
                    strcpy(new_book.details.fiction.genre, "роман");
                    break;
                case 2:
                    strcpy(new_book.details.fiction.genre, "пьеса");
                    break;
                case 3:
                    strcpy(new_book.details.fiction.genre, "поэзия");
                    break;
                }
                break;
            }
            else
            {
                printf("Ошибка: Введите корректное значение (1, 2 или 3).\n");
            }
        }
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

        printf("Выберите жанр детской литературы:\n");
        printf("1. Стихи\n");
        printf("2. Сказки\n");
        while (1)
        {
            printf("Введите номер жанра (1 или 2): ");
            fgets(genre_input, sizeof(genre_input), stdin);
            if (sscanf(genre_input, "%d", &genre_choice) == 1 && (genre_choice == 1 || genre_choice == 2))
            {
                if (genre_choice == 1)
                {
                    strcpy(new_book.details.children.genre, "стихи");
                }
                else
                {
                    strcpy(new_book.details.children.genre, "сказки");
                }
                break;
            }
            else
            {
                printf("Ошибка: Введите корректное значение (1 или 2).\n");
            }
        }
        break;
    }

    books[(*book_count)++] = new_book;
    printf("Книга добавлена успешно!\n");
}

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

void display_books(Book books[], int book_count)
{
    print_table_header();
    for (int i = 0; i < book_count; i++)
    {
        print_book_table_row(&books[i]);
    }
}

void bubble_sort_books_by_author(Book books[], int book_count)
{
    for (int i = 0; i < book_count - 1; i++)
    {
        for (int j = 0; j < book_count - i - 1; j++)
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

void display_sorted_books_by_author(Book books[], int book_count)
{
    Book temp_books[MAX_BOOKS];
    memcpy(temp_books, books, sizeof(Book) * book_count);

    quicksort_books(temp_books, book_count);
    printf("\nКниги, отсортированные по автору (быстрая сортировка):\n");
    display_books(temp_books, book_count);

    memcpy(temp_books, books, sizeof(Book) * book_count);
    bubble_sort_books_by_author(temp_books, book_count);
    printf("\nКниги, отсортированные по автору (сортировка пузырьком):\n");
    display_books(temp_books, book_count);
}

void bubble_sort_books_by_author_table(AuthorIndex author_table[], int book_count)
{
    for (int i = 0; i < book_count - 1; i++)
    {
        for (int j = 0; j < book_count - i - 1; j++)
        {
            if (strcmp(author_table[j].author, author_table[j + 1].author) > 0)
            {
                AuthorIndex temp = author_table[j];
                author_table[j] = author_table[j + 1];
                author_table[j + 1] = temp;
            }
        }
    }
}

void display_sorted_books_using_author_table(Book books[], int book_count, AuthorIndex author_table[])
{
    AuthorIndex temp_author_table[MAX_BOOKS];

    memcpy(temp_author_table, author_table, sizeof(AuthorIndex) * book_count);

    quicksort_author_table(temp_author_table, book_count);

    printf("\nКниги, отсортированные (quicksort) по таблице авторов:\n");
    print_table_header();
    for (int i = 0; i < book_count; i++)
    {
        int idx = temp_author_table[i].index;
        print_book_table_row(&books[idx]);
    }

    memcpy(temp_author_table, author_table, sizeof(AuthorIndex) * book_count);
    bubble_sort_books_by_author_table(temp_author_table, book_count);

    printf("\nКниги, отсортированные (bubblesort) по таблице авторов:\n");
    print_table_header();
    for (int i = 0; i < book_count; i++)
    {
        int idx = temp_author_table[i].index;
        print_book_table_row(&books[idx]);
    }

    return;
}

// Замерный эксперимент
void measure_sorting_performance(void)
{
    for (int book_count = 1000; book_count <= MAX_BOOKS_SORT; book_count += 1000)
    {
        Book books[MAX_BOOKS_SORT];
        AuthorIndex author_table[MAX_BOOKS_SORT];

        // Генерация массива книг
        for (int i = 0; i < book_count; i++)
        {
            generate_random_book(&books[i]);
            strncpy(author_table[i].author, books[i].author, sizeof(author_table[i].author));
            author_table[i].index = i;
        }

        printf("\nСортировка массива из %d книг (усреднённое время за 10 повторений):\n", book_count);

        double total_time = 0;

        // Быстрая сортировка книг
        for (int i = 0; i < 10; i++)
        {
            Book temp_books[MAX_BOOKS];
            memcpy(temp_books, books, sizeof(Book) * book_count);
            clock_t start_time = clock();
            quicksort_books(temp_books, book_count);
            clock_t end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
        }
        printf("Быстрая сортировка: %.12f секунд\n", total_time / 10);

        // Сортировка пузырьком книг
        total_time = 0;
        for (int i = 0; i < 10; i++)
        {
            Book temp_books[MAX_BOOKS];
            memcpy(temp_books, books, sizeof(Book) * book_count);
            clock_t start_time = clock();
            bubble_sort_books_by_author(temp_books, book_count);
            clock_t end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
        }
        printf("Сортировка пузырьком: %.12f секунд\n", total_time / 10);

        printf("Использовано памяти (массив книг), байт: %zu\n\n", sizeof(Book) * book_count);
        //
        // Быстрая сортировка таблицы авторов
        total_time = 0;
        for (int i = 0; i < 10; i++)
        {
            AuthorIndex temp_author_table[MAX_BOOKS];
            memcpy(temp_author_table, author_table, sizeof(AuthorIndex) * book_count);
            clock_t start_time = clock();
            quicksort_author_table(temp_author_table, book_count);
            clock_t end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
        }
        printf("Сортировка таблицы авторов (быстрая): %.12f секунд\n", total_time / 10);

        // Сортировка пузырьком таблицы авторов
        total_time = 0;
        for (int i = 0; i < 10; i++)
        {
            AuthorIndex temp_author_table[MAX_BOOKS];
            memcpy(temp_author_table, author_table, sizeof(AuthorIndex) * book_count);
            clock_t start_time = clock();
            bubble_sort_books_by_author_table(temp_author_table, book_count);
            clock_t end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
        }
        printf("Сортировка таблицы авторов (пузырьком): %.12f секунд\n", total_time / 10);

        printf("Использовано памяти (таблица авторов), байт: %zu\n\n", sizeof(AuthorIndex) * book_count);
    }
}

void quicksort_books(Book books[], int count)
{
    qsort(books, count, sizeof(Book), compare_books_by_author);
}

int compare_books_by_author(const void *a, const void *b)
{
    const Book *book1 = (const Book *)a;
    const Book *book2 = (const Book *)b;
    return strcmp(book1->author, book2->author);
}

void quicksort_author_table(AuthorIndex author_table[], int count)
{
    qsort(author_table, count, sizeof(AuthorIndex), compare_author_index);
}

void print_sorted_index(AuthorIndex author_table[], int count)
{
    AuthorIndex *author_table_copy = malloc(sizeof(AuthorIndex) * count);
    memmove(author_table_copy, author_table, sizeof(AuthorIndex) * count);

    qsort(author_table_copy, count, sizeof(AuthorIndex), compare_author_index);
    print_author_table(author_table_copy, count);
}

int compare_author_index(const void *a, const void *b)
{
    const AuthorIndex *ai1 = (const AuthorIndex *)a;
    const AuthorIndex *ai2 = (const AuthorIndex *)b;
    return strcmp(ai1->author, ai2->author);
}

void print_table_header()
{
    printf("%-35s %-50s %-25s %-20s %-10s\n", "Автор", "Название", "Издатель", "Страницы", "Доп. Информация");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void print_table_header_id()
{
    printf("%-5s %-35s %-50s %-25s %-20s %-10s\n", "Id", "Автор", "Название", "Издатель", "Страницы", "Доп. Информация");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void print_book_table_row_id(const Book *book, int id)
{
    char temp[100];
    strcpy(temp, book->title);
    if (strlen(book->title) > 35)
    {
        temp[32] = '.';
        temp[33] = '.';
        temp[34] = '.';
        temp[35] = 0;
    }

    char type[30];
    char type_details[200];

    if (book->type == 0)
    {
        strcpy(type, "Техническая");
        sprintf(type_details, "Отрасль: %s, %s, год издания: %d", book->details.tech.branch, book->details.tech.origin, book->details.tech.year);
    }
    else if (book->type == 1)
    {
        strcpy(type, "Художественная");
        sprintf(type_details, "Жанр: %s", book->details.fiction.genre);
    }
    else
    {
        sprintf(type_details, "Жанр: %s, мин. возраст: %d", book->details.children.genre, book->details.children.min_age);
        strcpy(type, "Детская");        
    }

    char temp_izd[100];
    strcpy(temp_izd, book->title);
    if (strlen(book->title) > 15)
    {
        temp_izd[12] = '.';
        temp_izd[13] = '.';
        temp_izd[14] = '.';
        temp_izd[15] = 0;
    }

    printf("%-5d %-30s %-40s %-20s %-10d %-30s %-30s\n", id, book->author, temp, temp_izd, book->pages, type, type_details);
}

void print_book_table_row(const Book *book)
{
    char temp[100];
    strcpy(temp, book->title);
    if (strlen(book->title) > 35)
    {
        temp[32] = '.';
        temp[33] = '.';
        temp[34] = '.';
        temp[35] = 0;
    }

    char type[30];
    char type_details[200];

    if (book->type == 0)
    {
        strcpy(type, "Техническая");
        sprintf(type_details, "Отрасль: %s, %s, год издания: %d", book->details.tech.branch, book->details.tech.origin, book->details.tech.year);
    }
    else if (book->type == 1)
    {
        strcpy(type, "Художественная");
        sprintf(type_details, "Жанр: %s", book->details.fiction.genre);
    }
    else
    {
        sprintf(type_details, "Жанр: %s, мин. возраст: %d", book->details.children.genre, book->details.children.min_age);
        strcpy(type, "Детская");        
    }

    char temp_izd[100];
    strcpy(temp_izd, book->title);
    if (strlen(book->title) > 15)
    {
        temp_izd[12] = '.';
        temp_izd[13] = '.';
        temp_izd[14] = '.';
        temp_izd[15] = 0;
    }

    printf("%-30s %-40s %-20s %-10d %-30s %-30s\n", book->author, temp, temp_izd, book->pages, type, type_details);
}