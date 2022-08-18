#include "stack.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>

#define WORD_SIZE 1024

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void push(char *stack, char *val, int *top)
{
    char *stk_string = (stack + WORD_SIZE * (*top));
    while (*val != '\0')
    {
        *stk_string = *val;
        stk_string++;
        val++;
    }
    *top += 1;
}

void pop(char *stack, int *top)
{
    if (top > 0)
    {
        memset(stack + WORD_SIZE * (*top - 1), '\0', WORD_SIZE);
        *top -= 1;
    }
    else
    {
        printf("Error, pop() called when stack is empty\n");
    }
}

const char *top(char *stack, int *top)
{

    const char *prefix = "OUTPUT:";
    char *to_send = (char *)malloc(WORD_SIZE);
    memset(to_send, '\0', WORD_SIZE);
    if (*top <= 0)
    {
        const char *err = "Error, stack is empty";
        strcat(to_send, err);
    }
    else
    {
        strcat(to_send, prefix);
        strncat(to_send, (stack + WORD_SIZE * (*top - 1)), WORD_SIZE);
    }
    
    return to_send;
}
