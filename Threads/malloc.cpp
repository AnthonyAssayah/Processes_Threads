#include "malloc.hpp"
#include <stdio.h>
#include <unistd.h>

static block_t* head = NULL;

//remove a memory block from our free memory linked list
void remove_block(block_t *block)
{
    if(!block->prev)//head
    {
        if(block->next)
        {
            head = block->next;
            head->prev = NULL;
        }
        else
        {
            head = NULL;
        }
    }
    else
    {
        if(!block->next)//tail
        {
            block->prev->next = NULL;
        }
        else
        {
            block->prev->next = block->next;
            block->next->prev = block->prev;
        }

    }
}

//Adds a block to our free memory linked list, that is sorted by pointer adress from small to bigg
void add_block(block_t *block)
{
    block->next = NULL;
    block->prev = NULL;
    if(head == NULL || (unsigned long)head > (unsigned long)block )
    {
        if(head == NULL)
        {
            head = block;
        }
        else
        {
            head->prev = block;
            block->next = head;
            head = block;
        }
    }
    else
    {
        block_t *current = head;
        while(current->next && (unsigned long)current->next < (unsigned long)block)
        {
            current = current->next;
        }
        block->prev = current;
        block->next = current->next;
		current->next = block;
        current->next->prev = block;
    }
    
}


block_t* split(block_t* block, size_t size)
{
    void *free_mem = (void*)((unsigned long)block + sizeof(block_t));
    block_t *new_block = (block_t*)((unsigned long)free_mem+size);
    new_block->size = block->size - (size + sizeof(block_t));
    block->size = size;

    return new_block;
}


void free_(void *pointer)
{
    add_block((block_t*)((unsigned long)pointer - sizeof(block_t)));
}

void* malloc_(size_t size)
{
    block_t *current = head;
    block_t *new_block;
    void *free_memory;
    //before asking the OS for new memory we should check if our memory list for free blocks 
    //that are suitable for our needs
    while(current != NULL)
    {
        if(current->size == size)
        {
            free_memory = (void*)((unsigned long)current + sizeof(block_t));
            //found a suitable block (either exactly the needed size or bigger)
            remove_block(current);
            return free_memory;
        }
        else if(current->size > size)
        {
            //split function takes "current" block and cuts it to the desired size
            // and then returns the pointer to a new block_t with the remainder of the memory
            new_block = split(current,size);
            add_block(new_block);
            free_memory = (void*)((unsigned long)current + sizeof(block_t));
            remove_block(current);

            return free_memory;
        }

        current = current->next;
    }
    //we didnt find a suitable free memory block so we should ask the OS
    //for more memory using sbrk()
    
    current = (block_t*)sbrk(size+sizeof(block_t));
    if(current == NULL)
    {
        perror("sbrk");
    }
    current->prev = NULL;
    current->next = NULL;
    current->size = size;
    
    return (void*)((unsigned long)current+sizeof(block_t));
}


void print_blocks()
{
    block_t* temp = head;
    if(head == NULL)
    {
        printf("NULL\n");
    }
    while(temp)
    {
        printf("%p,",temp);
        temp = temp->next;
    }
    printf("\n");
}

// int main()
// {
//     // block_t *a = (block_t*)malloc(sizeof(block_t));
//     // block_t *b = (block_t*)malloc(sizeof(block_t));
//     // block_t *c = (block_t*)malloc(sizeof(block_t));
    
//     // add_block(c);
//     // add_block(b);
//     // add_block(a);

//     // print();

//     // remove_block(c);
//     // print();
//     // add_block(c);
//     // print();

//     int *arr = (int*)malloc_(10);

//     for (int i = 0; i < 10; i++)
//     {
//         arr[i] = i;
//     }
//     for (int i = 0; i < 10; i++)
//     {
//         printf("ARR [%d]\n",arr[i]);
//     }

//     printf("pointr addr of big array is %p\n",arr);
//     free_(arr);
//     int *arr2 = (int*)malloc_(5);
//     int *arr3 = (int*)malloc_(5);
//     printf("arr2[%p] arr3[%p]\n",arr2,arr3);
    
//     for (int i = 0; i < 5; i++)
//     {
//         arr2[i] = i;
//         arr3[i] = (-1)*i;

//     }
//     for (int i = 0; i < 5; i++)
//     {
//         printf("arr2[%d] arr3[%d]\n",arr2[i],arr3[i]);
//     }
    




//     return 0;
// }