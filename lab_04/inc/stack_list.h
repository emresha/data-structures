#ifndef STACK_LIST_H__
#define STACK_LIST_H__

#define MAX_SIZE 1000

typedef struct Node
{
    char symbol;
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

int is_palindrome_list(StackList *stack, FreeList *free_list);

int is_empty_list(StackList *stack);

#endif
