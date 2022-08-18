#include "stack.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "malloc.hpp"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void push(pnode *head, char *val)
{
    pthread_mutex_lock(&lock);
    
    if ((*head) == NULL)
    {
        *head = (pnode)malloc_(sizeof(node));
        (*head)->data = (char*)malloc_(sizeof(val));
        strcpy((*head)->data,val);
        (*head)->next = NULL;
        pthread_mutex_unlock(&lock);


        return;
    }
    pnode temp = (pnode)malloc_(sizeof(node));
    temp->data = (char*)malloc_(sizeof(val));
    strcpy((temp)->data,val);
    temp->next = *head;
    *head = temp;
    pthread_mutex_unlock(&lock);

}

char *pop(pnode *head)
{
    pthread_mutex_lock(&lock);

    if (*head == NULL)
    {
        printf("Error: pop() called when stack is empty\n");
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    char *temp_val = (*head)->data;
    pnode temp_node = *head;
    free_((*head)->data);
    if ((*head)->next != NULL)
    {
        *head = (*head)->next;
        free_(temp_node);
    }
    else
    {
        *head = NULL;
        free_(temp_node);
    }
    pthread_mutex_unlock(&lock);
    return temp_val;
}

const char *top(pnode head)
{   
    pthread_mutex_lock(&lock);

    if (head == NULL)
    {
        pthread_mutex_unlock(&lock);
        return "Error: top() called when stack is empty\n";
    }
    pthread_mutex_unlock(&lock);
    return head->data;
}

void print(pnode head)
{
    while (head != NULL)
    {
        printf("%s, ", head->data);
        head = head->next;
    }
    printf("\n");
}

void clear(pnode *head)
{
    while(*head!=NULL)
    {
        pop(head);
    }
}
