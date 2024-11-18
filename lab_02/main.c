#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define MAX_BOOKS 100

typedef enum
{
    TECHNICAL,
    FICTION,
    CHILDREN
} BookType;

typedef struct
{
    char branch[50];
    char origin[20];
    int year;
} Technical;

typedef struct
{
    char genre[30];
} Fiction;

typedef struct
{
    int min_age;
    char genre[30];
} Children;

typedef union
{
    Technical tech;
    Fiction fiction;
    Children children;
} BookDetails;

typedef struct
{
    char author[50];
    char title[100];
    char publisher[50];
    int pages;
    BookType type;
    BookDetails details;
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
void measure_sorting_performance(Book books[], int book_count, AuthorIndex author_table[]);
void display_author_fiction_books(Book books[], int book_count);

void quicksort_books(Book books[], int count);
void quicksort_author_table(AuthorIndex author_table[], int count);

void print_memory_usage();
void print_table_header();
void print_book_table_row(const Book *book);
int compare_books_by_author(const void *a, const void *b);
int compare_author_index(const void *a, const void *b);

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

int main(void)
{
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
        {"Lewis", "The Chronicles of Narnia: The Lion, the Witch and the Wardrobe", "Geoffrey Bles", 206, CHILDREN, {.children = {9, "Fantasy"}}},
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
        {"Mendel", "Experiments on Plant Hybridization", "Verhandlungen des naturforschenden Vereins", 150, TECHNICAL, {.tech = {"Genetics", "Translated", 1866}}},
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
        printf("9. Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        getchar();

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
            measure_sorting_performance(books, book_count, author_table);
            break;
        case 7:
            display_sorted_books_using_author_table(books, book_count, author_table);
            break;
        case 8:
            display_author_fiction_books(books, book_count);
            break;
        case 9:
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

        while (1)
        {
            printf("Введите происхождение (Отечественная/Переводная): ");
            fgets(new_book.details.tech.origin, 20, stdin);
            new_book.details.tech.origin[strcspn(new_book.details.tech.origin, "\n")] = '\0';

            if (strcmp(new_book.details.tech.origin, "Отечественная") == 0 || strcmp(new_book.details.tech.origin, "Переводная") == 0)
            {
                break;
            }
            else
            {
                printf("Ошибка: Введите корректное происхождение (Отечественная или Переводная).\n");
            }
        }

        printf("Введите год издания: ");
        int rc = scanf("%d", &new_book.details.tech.year);
        while (rc != 1 || new_book.details.tech.year < 0)
        {
            printf("Ошибка: Введите положительное число (год): ");
            while (getchar() != '\n')
                ;
        }
        getchar();
        break;

    case FICTION:
        while (1)
        {
            printf("Введите жанр (роман, пьеса, поэзия): ");
            fgets(new_book.details.fiction.genre, 30, stdin);
            new_book.details.fiction.genre[strcspn(new_book.details.fiction.genre, "\n")] = '\0';

            if (strcmp(new_book.details.fiction.genre, "роман") == 0 || strcmp(new_book.details.fiction.genre, "пьеса") == 0 || strcmp(new_book.details.fiction.genre, "поэзия") == 0)
            {
                break;
            }
            else
            {
                printf("Ошибка: Введите корректный жанр (роман, пьеса или поэзия).\n");
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

        while (1)
        {
            printf("Введите тип детской литературы (стихи, сказки): ");
            fgets(new_book.details.children.genre, 30, stdin);
            new_book.details.children.genre[strcspn(new_book.details.children.genre, "\n")] = '\0';

            if (strcmp(new_book.details.children.genre, "стихи") == 0 || strcmp(new_book.details.children.genre, "сказки") == 0)
            {
                break;
            }
            else
            {
                printf("Ошибка: Введите корректный тип детской литературы (стихи или сказки).\n");
            }
        }
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

// Функция вывода книг
void display_books(Book books[], int book_count)
{
    print_table_header();
    for (int i = 0; i < book_count; i++)
    {
        print_book_table_row(&books[i]);
    }
}

// Функция сортировки пузырьком по автору
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


// Функция сортировки изначальной таблицы по автору
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

// Функция сортировки используя дополнительную таблицу "по ключам"
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
void measure_sorting_performance(Book books[], int book_count, AuthorIndex author_table[])
{
    struct rusage usage;

    Book temp_books[MAX_BOOKS];
    memcpy(temp_books, books, sizeof(Book) * book_count);

    clock_t start_time = clock();
    quicksort_books(temp_books, book_count);
    clock_t end_time = clock();
    printf("\nОбычная сортировка книг:\n");

    printf("\n Книги, отсортированные по автору (быстрая сортировка):\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();
    bubble_sort_books_by_author(temp_books, book_count);
    end_time = clock();

    printf("\n Книги, отсортированные по автору (сортировка пузырьком):\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    getrusage(RUSAGE_SELF, &usage);
    printf("Использование памяти: %ld KB\n", usage.ru_maxrss);

    start_time = clock();
    quicksort_author_table(author_table, book_count);
    end_time = clock();
    printf("\nСортировка по таблице ключей:\n");
    printf("Книги, отсортированные по таблице авторов (быстрая сортировка):\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    start_time = clock();
    bubble_sort_books_by_author_table(author_table, book_count);
    end_time = clock();

    printf("\nКниги, отсортированные по таблице авторов (сортировка пузырьком):\n");
    printf("Время выполнения: %.12f секунд\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    getrusage(RUSAGE_SELF, &usage);
    printf("Использование памяти: %ld KB\n", usage.ru_maxrss);
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

int compare_author_index(const void *a, const void *b)
{
    const AuthorIndex *ai1 = (const AuthorIndex *)a;
    const AuthorIndex *ai2 = (const AuthorIndex *)b;
    return strcmp(ai1->author, ai2->author);
}

void print_table_header()
{
    printf("%-20s %-30s %-20s %-10s\n", "Автор", "Название", "Издатель", "Страницы");
    printf("----------------------------------------------------------------------------------\n");
}

void print_book_table_row(const Book *book)
{
    printf("%-20s %-30s %-20s %-10d\n", book->author, book->title, book->publisher, book->pages);
}
