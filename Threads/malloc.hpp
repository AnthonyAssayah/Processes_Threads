#include <stdlib.h>

typedef struct NODE__{
    size_t size;
    struct NODE__* next;
    struct NODE__* prev;
    
}block_t;


void* malloc_(size_t size);

void free_(void *ptr);