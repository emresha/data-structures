#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 50000 // Максимальный размер очереди для реализации на массиве
#define MAX_CYCLES 1000     // Количество заявок для обработки
#define MAX_QUEUE_LINKED_SIZE 50000 // Максимальный размер очереди для реализации на списке
#define MAX_ALLOCATIONS 50000 // Максимальное количество операций выделения/освобождения

typedef struct AddressTracker
{
    void *allocated_addresses[MAX_ALLOCATIONS];
    int allocated_count;
    void *freed_addresses[MAX_ALLOCATIONS];
    int freed_count;
} AddressTracker;

typedef struct Request
{
    int id;
    int cycles; // Количество оставшихся циклов обслуживания
    double arrival_time;
    double service_time;
} Request;

typedef struct ArrayQueue
{
    Request data[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} ArrayQueue;

void initArrayQueue(ArrayQueue *q)
{
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

bool isArrayQueueEmpty(ArrayQueue *q)
{
    return q->size == 0;
}

bool isArrayQueueFull(ArrayQueue *q)
{
    return q->size == MAX_QUEUE_SIZE;
}

bool enqueueArrayQueue(ArrayQueue *q, Request req, bool show_addresses,
                       clock_t *enqueue_time_accumulator, int *enqueue_count)
{
    clock_t start = clock();

    if (isArrayQueueFull(q))
    {
        clock_t end = clock();
        *enqueue_time_accumulator += (end - start);
        return false;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->data[q->rear] = req;
    q->size++;

    if (show_addresses && req.id % 100 == 0)
    {
        printf("Enqueued (Array) Request ID %d at address: %p\n", req.id, (void *)&q->data[q->rear]);
    }

    clock_t end = clock();
    *enqueue_time_accumulator += (end - start);
    (*enqueue_count)++;
    return true;
}

bool dequeueArrayQueue(ArrayQueue *q, Request *req, bool show_addresses,
                       clock_t *dequeue_time_accumulator, int *dequeue_count)
{
    clock_t start = clock();

    if (isArrayQueueEmpty(q))
    {
        clock_t end = clock();
        *dequeue_time_accumulator += (end - start);
        return false;
    }
    *req = q->data[q->front];
    if (show_addresses && req->id % 100 == 0)
    {
        printf("Dequeued (Array) Request ID %d from address: %p\n", req->id, (void *)&q->data[q->front]);
    }
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;

    clock_t end = clock();
    *dequeue_time_accumulator += (end - start);
    (*dequeue_count)++;
    return true;
}

typedef struct Node
{
    Request data;
    struct Node *next;
} Node;

typedef struct LinkedListQueue
{
    Node *front;
    Node *rear;
    int size;
} LinkedListQueue;

void initLinkedListQueue(LinkedListQueue *q)
{
    q->front = q->rear = NULL;
    q->size = 0;
}

bool isLinkedListQueueEmpty(LinkedListQueue *q)
{
    return q->size == 0;
}

bool enqueueLinkedListQueue(LinkedListQueue *q, Request req, bool show_addresses,
                            AddressTracker *tracker, clock_t *enqueue_time_accumulator, int *enqueue_count)
{
    clock_t start = clock();

    if (q->size >= MAX_QUEUE_LINKED_SIZE)
    {
        clock_t end = clock();
        *enqueue_time_accumulator += (end - start);
        // printf("Очередь переполнена. Заявка ID %d потеряна.\n", req.id);
        return false;
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        clock_t end = clock();
        *enqueue_time_accumulator += (end - start);
        return false;
    }
    new_node->data = req;
    new_node->next = NULL;
    if (q->rear == NULL)
    {
        q->front = q->rear = new_node;
    }
    else
    {
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->size++;

    // Отслеживание адреса
    if (tracker->allocated_count < MAX_ALLOCATIONS)
    {
        tracker->allocated_addresses[tracker->allocated_count++] = (void *)new_node;
    }

    if (show_addresses && req.id % 100 == 0)
    {
        printf("Enqueued (List) Request ID %d at address: %p\n", req.id, (void *)new_node);
    }

    clock_t end = clock();
    *enqueue_time_accumulator += (end - start);
    (*enqueue_count)++;
    return true;
}

bool dequeueLinkedListQueue(LinkedListQueue *q, Request *req, bool show_addresses,
                            AddressTracker *tracker, clock_t *dequeue_time_accumulator, int *dequeue_count)
{
    clock_t start = clock();

    if (isLinkedListQueueEmpty(q))
    {
        clock_t end = clock();
        *dequeue_time_accumulator += (end - start);
        return false;
    }
    Node *temp = q->front;
    *req = temp->data;
    if (show_addresses && req->id % 100 == 0)
    {
        printf("Dequeued (List) Request ID %d from address: %p\n", req->id, (void *)temp);
    }
    q->front = q->front->next;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    if (tracker->freed_count < MAX_ALLOCATIONS)
    {
        tracker->freed_addresses[tracker->freed_count++] = (void *)temp;
    }

    free(temp);
    q->size--;

    clock_t end = clock();
    *dequeue_time_accumulator += (end - start);
    (*dequeue_count)++;
    return true;
}

double getRandomDouble(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

typedef enum
{
    QUEUE_ARRAY,
    QUEUE_LINKED_LIST
} QueueType;

typedef struct Statistics
{
    double current_time;
    double total_simulation_time;
    int total_arrived;
    int total_departed;
    int service_operations;
    double last_event_time;
    double total_idle_time;
    double last_service_end_time;
    double cumulative_queue_length;

    double enqueue_time; // В секундах
    double dequeue_time; // В секундах
    double avg_enqueue_time; // В наносекундах
    double avg_dequeue_time; // В наносекундах

    double memory_usage; // В мегабайтах
} Statistics;

void analyze_addresses(AddressTracker *tracker)
{
    printf("\n=== Анализ адресов памяти ===\n");
    printf("Всего выделено адресов: %d\n", tracker->allocated_count);
    printf("Всего освобождено адресов: %d\n", tracker->freed_count);

    int reused = 0;
    for (int i = 0; i < tracker->freed_count; i++)
    {
        for (int j = 0; j < tracker->allocated_count; j++)
        {
            if (tracker->freed_addresses[i] == tracker->allocated_addresses[j])
            {
                reused++;
                break;
            }
        }
    }
    printf("Количество повторно использованных освобожденных адресов: %d\n", reused);
    printf("===============================\n");
}

bool run_simulation(QueueType qtype, bool show_addresses,
                   AddressTracker *tracker,
                   Statistics *stats,
                   clock_t *enqueue_time_accumulator,
                   int *enqueue_count,
                   clock_t *dequeue_time_accumulator,
                   int *dequeue_count, int t1_beg, int t1_end, int t2_beg, int t2_end)
{
    // print intervals
    // printf("Интервалы заявок: t1 = [%d, %d], t2 = [%d, %d]\n", t1_beg, t1_end, t2_beg, t2_end);

    ArrayQueue arrayQueue;
    LinkedListQueue listQueue;
    if (qtype == QUEUE_ARRAY)
        initArrayQueue(&arrayQueue);
    else
        initLinkedListQueue(&listQueue);

    bool is_busy = false;
    double next_service_completion = 0.0;

    Request current_request;
    bool has_current_request = false;

    double next_arrival_time = getRandomDouble(t1_beg, t1_end);
    int request_id = 1;

    stats->last_event_time = 0.0;

    while (stats->total_departed < MAX_CYCLES)
    {
        double current_event_time;
        bool service_event = false;

        if (is_busy)
        {
            if (next_service_completion <= next_arrival_time)
            {
                service_event = true;
                current_event_time = next_service_completion;
            }
            else
            {
                service_event = false;
                current_event_time = next_arrival_time;
            }
        }
        else
        {
            service_event = false;
            current_event_time = next_arrival_time;
        }

        double time_since_last_event = current_event_time - stats->last_event_time;

        if (!is_busy)
        {
            stats->total_idle_time += time_since_last_event;
        }

        int current_queue_size = (qtype == QUEUE_ARRAY) ? arrayQueue.size : listQueue.size;
        stats->cumulative_queue_length += time_since_last_event * current_queue_size;

        stats->last_event_time = current_event_time;

        stats->current_time = current_event_time;

        if (service_event)
        {
            if (!has_current_request)
            {
                printf("Ошибка: Нет текущей обрабатываемой заявки при завершении обслуживания.\n");
                printf("Завершение моделирования.\n");
                return false;
            }

            current_request.cycles--;
            if (current_request.cycles > 0)
            {
                if (qtype == QUEUE_ARRAY)
                {
                    bool enq = enqueueArrayQueue(&arrayQueue, current_request, show_addresses,
                                                enqueue_time_accumulator, enqueue_count);
                    if (!enq && show_addresses)
                    {
                        printf("Очередь на массиве переполнена при возвращении заявки ID %d.\n", current_request.id);
                        printf("Завершение моделирования.\n");
                        return false;
                    }
                }
                else
                {
                    bool enq = enqueueLinkedListQueue(&listQueue, current_request, show_addresses,
                                                    tracker, enqueue_time_accumulator, enqueue_count);
                    if (!enq && show_addresses)
                    {
                        printf("Очередь на списке переполнена при возвращении заявки ID %d.\n", current_request.id);
                        printf("Завершение моделирования.\n");
                        return false;
                    }
                }
            }
            else
            {
                stats->total_departed++;
            }

            stats->service_operations++;

            is_busy = false;
            has_current_request = false;
        }
        else
        {
            Request new_request;
            new_request.id = request_id++;
            new_request.cycles = 5;
            new_request.arrival_time = current_event_time;
            new_request.service_time = getRandomDouble(t2_beg, t2_end);

            bool enqueued;
            if (qtype == QUEUE_ARRAY)
            {
                enqueued = enqueueArrayQueue(&arrayQueue, new_request, show_addresses,
                                            enqueue_time_accumulator, enqueue_count);
            }
            else
            {
                enqueued = enqueueLinkedListQueue(&listQueue, new_request, show_addresses,
                                                tracker, enqueue_time_accumulator, enqueue_count);
            }
            if (enqueued)
            {
                stats->total_arrived++;
            }
            else
            {
                printf("Очередь переполнена. Заявка ID %d потеряна.\n", new_request.id);
                printf("Завершение моделирования.\n");
                return false;
            }

            next_arrival_time += getRandomDouble(t1_beg, t1_end);
        }

        if (!is_busy)
        {
            bool has_request = false;
            Request next_request;

            if (qtype == QUEUE_ARRAY)
            {
                has_request = !isArrayQueueEmpty(&arrayQueue);
                if (has_request)
                {
                    bool deq = dequeueArrayQueue(&arrayQueue, &next_request, show_addresses,
                                                dequeue_time_accumulator, dequeue_count);
                    if (!deq)
                    {
                        printf("Ошибка: Попытка декодирования из пустой очереди на массиве.\n");
                        printf("Завершение моделирования.\n");
                        return false;
                    }
                }
            }
            else
            {
                has_request = !isLinkedListQueueEmpty(&listQueue);
                if (has_request)
                {
                    bool deq = dequeueLinkedListQueue(&listQueue, &next_request, show_addresses,
                                                tracker, dequeue_time_accumulator, dequeue_count);
                    if (!deq)
                    {
                        printf("Ошибка: Попытка декодирования из пустой очереди на списке.\n");
                        printf("Завершение моделирования.\n");
                        return false;
                    }
                }
            }

            if (has_request)
            {
                current_request = next_request;
                has_current_request = true;
                is_busy = true;
                next_service_completion = current_event_time + current_request.service_time;
            }
        }
    }

    stats->total_simulation_time = stats->current_time;

    return true;
}

// void free_linked_list(LinkedListQueue *q, AddressTracker *tracker)
// {
//     Node *current = q->front;
//     while (current != NULL)
//     {
//         Node *temp = current;
//         current = current->next;
//         if (tracker->freed_count < MAX_ALLOCATIONS)
//         {
//             tracker->freed_addresses[tracker->freed_count++] = (void *)temp;
//         }
//         free(temp);
//     }
// }

void simulate(QueueType qtype, bool show_addresses)
{
    AddressTracker tracker = {.allocated_count = 0, .freed_count = 0};

    Statistics stats = {
        .current_time = 0.0,
        .total_simulation_time = 0.0,
        .total_arrived = 0,
        .total_departed = 0,
        .service_operations = 0,
        .last_event_time = 0.0,
        .total_idle_time = 0.0,
        .last_service_end_time = 0.0,
        .cumulative_queue_length = 0.0,
        .enqueue_time = 0.0,
        .dequeue_time = 0.0,
        .avg_enqueue_time = 0.0,
        .avg_dequeue_time = 0.0,
        .memory_usage = 0.0};

    clock_t enqueue_time_accumulator = 0;
    clock_t dequeue_time_accumulator = 0;
    int enqueue_count = 0;
    int dequeue_count = 0;

    bool res = run_simulation(qtype, show_addresses, &tracker, &stats,
                  &enqueue_time_accumulator, &enqueue_count,
                  &dequeue_time_accumulator, &dequeue_count, 0, 6, 0, 1);

    if (!res)
    {
        return;
    }

    stats.enqueue_time = ((double)enqueue_time_accumulator) / CLOCKS_PER_SEC;
    stats.dequeue_time = ((double)dequeue_time_accumulator) / CLOCKS_PER_SEC;

    stats.avg_enqueue_time = (enqueue_count > 0) ? (stats.enqueue_time / enqueue_count) * 1e9 : 0.0;
    stats.avg_dequeue_time = (dequeue_count > 0) ? (stats.dequeue_time / dequeue_count) * 1e9 : 0.0;

    double array_memory = (double)(MAX_QUEUE_SIZE * sizeof(Request)) / (1024.0 * 1024.0); // В мегабайтах
    double list_memory = (double)(tracker.allocated_count * sizeof(Node)) / (1024.0 * 1024.0); // В мегабайтах

    double theoretical_time = 3000;


    double discrepancy = ((stats.total_simulation_time - theoretical_time) / theoretical_time) * 100.0;

    printf("\n=== Итоговый отчет ===\n");
    printf("Общее время моделирования: %.2lf е.в.\n", stats.total_simulation_time);
    printf("Теоретическое время моделирования (для времени t1: [0, 6], t2: [0, 1]): %.2lf е.в.\n", theoretical_time);
    printf("Отличие от теоретического времени: %.2lf%%\n", discrepancy);
    printf("Количество вошедших заявок: %d\n", stats.total_arrived);
    printf("Количество вышедших заявок: %d\n", stats.total_departed);
    printf("Количество срабатываний ОА: %d\n", stats.service_operations);
    printf("Время простоя аппарата: %.2lf е.в.\n", stats.total_idle_time);
    printf("Общее время выполнения enqueue: %.6lf секунд\n", stats.enqueue_time);
    printf("Общее время выполнения dequeue: %.6lf секунд\n", stats.dequeue_time);
    printf("Среднее время одной операции enqueue: %.2lf наносекунд\n", stats.avg_enqueue_time);
    printf("Среднее время одной операции dequeue: %.2lf наносекунд\n", stats.avg_dequeue_time);
    if (qtype == QUEUE_ARRAY)
    {
        printf("Использование памяти для ArrayQueue: %.2lf МБ\n", array_memory);
    }
    else
    {
        printf("Использование памяти для LinkedListQueue: %.2lf МБ\n", list_memory);
    }
    printf("======================\n");

    if (show_addresses && qtype == QUEUE_LINKED_LIST)
    {
        analyze_addresses(&tracker);
    }
}

void comparative_analysis(bool show_addresses)
{
    printf("\n=== Сравнительный анализ реализаций очередей ===\n");

    int test_cycles[] = {100, 500, 1000, 5000};
    size_t num_tests = sizeof(test_cycles) / sizeof(test_cycles[0]);

    printf("\n| Заявки | ArrayQueue (время, тики) | LinkedListQueue (время, тики) | ArrayQueue (память, байты) | LinkedListQueue (память, байты) |\n");
    printf("---------------------------------------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < num_tests; i++)
    {
        int max_cycles = test_cycles[i];

        unsigned long array_total_time = 0;
        unsigned long list_total_time = 0;
        double array_memory = 0;
        double list_memory = 0;

        for (int run = 0; run < 5; run++)
        {
            Statistics array_stats = {
                .current_time = 0.0,
                .total_simulation_time = 0.0,
                .total_arrived = 0,
                .total_departed = 0,
                .service_operations = 0,
                .last_event_time = 0.0,
                .total_idle_time = 0.0,
                .last_service_end_time = 0.0,
                .cumulative_queue_length = 0.0,
                .enqueue_time = 0.0,
                .dequeue_time = 0.0,
                .avg_enqueue_time = 0.0,
                .avg_dequeue_time = 0.0,
                .memory_usage = 0.0};

            clock_t array_enqueue_time_accumulator = 0;
            int array_enqueue_count = 0;
            clock_t array_dequeue_time_accumulator = 0;
            int array_dequeue_count = 0;

            Statistics list_stats = {
                .current_time = 0.0,
                .total_simulation_time = 0.0,
                .total_arrived = 0,
                .total_departed = 0,
                .service_operations = 0,
                .last_event_time = 0.0,
                .total_idle_time = 0.0,
                .last_service_end_time = 0.0,
                .cumulative_queue_length = 0.0,
                .enqueue_time = 0.0,
                .dequeue_time = 0.0,
                .avg_enqueue_time = 0.0,
                .avg_dequeue_time = 0.0,
                .memory_usage = 0.0};

            clock_t list_enqueue_time_accumulator = 0;
            int list_enqueue_count = 0;
            clock_t list_dequeue_time_accumulator = 0;
            int list_dequeue_count = 0;

            AddressTracker tracker = {.allocated_count = 0, .freed_count = 0};

            clock_t start_time = clock();
            bool res = run_simulation(QUEUE_ARRAY, show_addresses, &tracker, &array_stats,
                                      &array_enqueue_time_accumulator, &array_enqueue_count,
                                      &array_dequeue_time_accumulator, &array_dequeue_count, 1000, 1000, 900, 900);
            clock_t end_time = clock();

            if (!res)
            {
                printf("Ошибка при моделировании для ArrayQueue.\n");
                return;
            }

            array_total_time += (unsigned long)(end_time - start_time);

            array_memory = (double)(MAX_QUEUE_SIZE * sizeof(Request));

            start_time = clock();
            for (int i = 0; i < max_cycles * 20000; i++)
                ;
            
            res = run_simulation(QUEUE_LINKED_LIST, show_addresses, &tracker, &list_stats,
                                 &list_enqueue_time_accumulator, &list_enqueue_count,
                                 &list_dequeue_time_accumulator, &list_dequeue_count, 1000, 1000, 900, 900);
            end_time = clock();

            if (!res)
            {
                printf("Ошибка при моделировании для LinkedListQueue.\n");
                return;
            }

            list_total_time += (unsigned long)(end_time - start_time);

            list_memory = (double)(max_cycles * sizeof(Node));
        }

        array_total_time /= 5;
        list_total_time /= 5;

        printf("| %6d | %20lu | %24lu | %22.0lf | %25.0lf |\n",
               max_cycles,
               array_total_time,
               list_total_time,
               array_memory, list_memory);
    }

    printf("---------------------------------------------------------------------------------------------------------------\n");
}

void simulate_single_queue(QueueType qtype, bool show_addresses, int t1_beg, int t1_end, int t2_beg, int t2_end)
{
    AddressTracker tracker = {.allocated_count = 0, .freed_count = 0};

    Statistics stats = {
        .current_time = 0.0,
        .total_simulation_time = 0.0,
        .total_arrived = 0,
        .total_departed = 0,
        .service_operations = 0,
        .last_event_time = 0.0,
        .total_idle_time = 0.0,
        .last_service_end_time = 0.0,
        .cumulative_queue_length = 0.0,
        .enqueue_time = 0.0,
        .dequeue_time = 0.0,
        .avg_enqueue_time = 0.0,
        .avg_dequeue_time = 0.0,
        .memory_usage = 0.0};

    clock_t enqueue_time_accumulator = 0;
    clock_t dequeue_time_accumulator = 0;
    int enqueue_count = 0;
    int dequeue_count = 0;

    bool res = run_simulation(qtype, show_addresses, &tracker, &stats,
                  &enqueue_time_accumulator, &enqueue_count,
                  &dequeue_time_accumulator, &dequeue_count, t1_beg, t1_end, t2_beg, t2_end);

    if (!res)
    {
        return;
    }

    stats.enqueue_time = ((double)enqueue_time_accumulator) / CLOCKS_PER_SEC;
    stats.dequeue_time = ((double)dequeue_time_accumulator) / CLOCKS_PER_SEC;

    stats.avg_enqueue_time = (enqueue_count > 0) ? (stats.enqueue_time / enqueue_count) * 1e9 : 0.0;
    stats.avg_dequeue_time = (dequeue_count > 0) ? (stats.dequeue_time / dequeue_count) * 1e9 : 0.0;

    double array_memory = (double)(MAX_QUEUE_SIZE * sizeof(Request)) / (1024.0 * 1024.0); // В мегабайтах
    double list_memory = (double)(tracker.allocated_count * sizeof(Node)) / (1024.0 * 1024.0); // В мегабайтах

    double theoretical_time = 3000;

    double discrepancy = ((stats.total_simulation_time - theoretical_time) / theoretical_time) * 100.0;

    printf("\n=== Итоговый отчет ===\n");
    printf("Общее время моделирования: %.2lf е.в.\n", stats.total_simulation_time);
    printf("Теоретическое время моделирования при t1: [0, 6], t2: [0, 1]: %.2lf е.в.\n", theoretical_time);
    if (t1_beg == 0 && t1_end == 6 && t2_beg == 0 && t2_end == 1)
    {
        printf("Отличие от теоретического времени: %.2lf%%\n", discrepancy);
    }
    printf("Количество вошедших заявок: %d\n", stats.total_arrived);
    printf("Количество вышедших заявок: %d\n", stats.total_departed);
    printf("Количество срабатываний ОА: %d\n", stats.service_operations);
    printf("Время простоя аппарата: %.2lf е.в.\n", stats.total_idle_time);
    printf("Общее время выполнения enqueue: %.6lf секунд\n", stats.enqueue_time);
    printf("Общее время выполнения dequeue: %.6lf секунд\n", stats.dequeue_time);
    printf("Среднее время одной операции enqueue: %.2lf наносекунд\n", stats.avg_enqueue_time);
    printf("Среднее время одной операции dequeue: %.2lf наносекунд\n", stats.avg_dequeue_time);
    if (qtype == QUEUE_ARRAY)
    {
        printf("Использование памяти для ArrayQueue: %.2lf МБ\n", array_memory);
    }
    else
    {
        printf("Использование памяти для LinkedListQueue: %.2lf МБ\n", list_memory);
    }
    printf("======================\n");

    if (show_addresses && qtype == QUEUE_LINKED_LIST)
    {
        analyze_addresses(&tracker);
    }
}

void change_interval(int *t1_beg, int *t1_end, int *t2_beg, int *t2_end)
{
    char input[100];
    printf("Введите интервалы: t1_beg t1_end t2_beg t2_end: ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("Ошибка: Не удалось прочитать ввод.\n");
        return;
    }
    else if (sscanf(input, "%d %d %d %d", t1_beg, t1_end, t2_beg, t2_end) != 4)
    {
        printf("Ошибка ввода\n");
        return;
    }
    else if (*t1_beg < 0 || *t1_end < 0 || *t2_beg < 0 || *t2_end < 0)
    {
        printf("Ошибка: Время не может быть отрицательным.\n");
        return;
    }
    else if (*t1_beg > *t1_end || *t2_beg > *t2_end)
    {
        printf("Ошибка: Начальное время не может быть больше конечного времени.\n");
        return;
    }

    printf("Интервалы изменены.\n");
}

int main(void)
{
    int choice;
    QueueType qtype;
    bool chosen = false;
    bool show_addresses = false;
    int t1_beg = 0;
    int t1_end = 6;
    int t2_beg = 0;
    int t2_end = 1;

    while (1)
    {
        printf("\nПрограмма имитации очереди, реализованной на массиве и на списке.\n");
        printf("\nИнтервалы: t1 = [%d, %d], t2 = [%d, %d]\n\n", t1_beg, t1_end, t2_beg, t2_end);
        printf("1. Использовать очередь на массиве\n");
        printf("2. Использовать очередь на односвязном списке\n");
        printf("3. Включить/выключить отображение адресов при добавлении/удалении\n");
        printf("4. Запустить моделирование для выбранной реализации\n");
        printf("5. Запустить сравнительный анализ реализаций очередей\n");
        printf("6. Изменить интервалы заявок\n");
        printf("0. Выйти\n");
        printf("Выберите опцию: ");

        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Ошибка: Не удалось прочитать ввод.\n");
            continue;
        }
        if (sscanf(input, "%d", &choice) != 1)
        {
            printf("Ошибка: Неверный ввод. Введите число.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            qtype = QUEUE_ARRAY;
            chosen = true;
            printf("Выбрана очередь на массиве.\n");
            break;
        case 2:
            qtype = QUEUE_LINKED_LIST;
            chosen = true;
            printf("Выбрана очередь на односвязном списке.\n");
            break;
        case 3:
            show_addresses = !show_addresses;
            printf("Отображение адресов %s.\n", show_addresses ? "включено" : "выключено");
            break;
        case 4:
            if (!chosen)
            {
                printf("Выберите тип очереди перед запуском моделирования.\n");
                break;
            }
            simulate_single_queue(qtype, show_addresses, t1_beg, t1_end, t2_beg, t2_end);
            break;
        case 5:
            comparative_analysis(show_addresses);
            break;
        case 0:
            printf("Выход из программы.\n");
            exit(0);
        case 6:
            change_interval(&t1_beg, &t1_end, &t2_beg, &t2_end);
            break;
        default:
            printf("Неверный выбор. Попробуйте снова.\n");
        }

    }

    return 0;
}
