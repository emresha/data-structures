#ifndef STACK_LIST_H__
#define STACK_LIST_H__

#define MAX_SIZE 10000

typedef struct Node
{
    int symbol;
    struct Node *next;
}
Node;

typedef struct
{
    Node *top;
}
StackList;

typedef struct
{
    Node *pointers[MAX_SIZE];
    int count;
}
FreeList;

void free_stack_list(StackList *stack);

void init_stack_list(StackList *stack);

void init_free_list(FreeList *free_list);

void push_list(StackList *stack, char value);

char pop_list(StackList *stack, FreeList *free_list);

void print_stack_list(StackList *stack);

void print_free_list(FreeList *free_list);

void sort_stack_list(StackList *stack1, StackList *stack2, StackList *sorted_stack, FreeList *free_list);

int is_empty_list(StackList *stack);

#endif
