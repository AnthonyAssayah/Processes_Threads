#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void shell_loop()
{
    pnode head = NULL;
    while (1)
    {

        char input[BUFFER_SIZE + 6]; // input text capped at 1024, and "PUSH " size is 5 + \0
        memset(input, '\0', BUFFER_SIZE + 6 );
        scanf(" %[^\n]s", input);
        // printf("input is %s\n", input);

        char *index = input;
        char command[6];
        for (int i = 0; i < 5; i++, index++)
        {
            if (*index == '\0')
            {
                command[i] = ' ';
            }
            else
            {
                command[i] = *index;
            }
        }
        command[5] = '\0'; // null terminate our command string;

        // printf("command is %s]\n", command);
        // printf("[%s]\n",index);

        if (strcmp(command, "EXIT ") == 0)
        {
            clear(&head);
            return;
        }
        else if (strcmp(command, "PUSH ") == 0)
        {
            push(&head,index);
        }
        else if (strcmp(command, "POP  ") == 0)
        {
            pop(&head);
        }
        else if (strcmp(command, "TOP  ") == 0)
        {
            printf("OUTPUT:%s \n",top(head));
            
        }
        else if(strcmp(command, "PRINT")==0)
        {
            print(head);
        }
        else if(strcmp(command, "CLEAR")==0)
        {
            clear(&head);
        }
        else
        {
            printf("Error reading a command\n");
        }
    }
}

int main()
{
    // pnode head = NULL;
    // push(&head, "hello");
    // push(&head, "my");
    // print(head);
    // // pop(&head);
    // pop(&head);
    // pop(&head);
    // pop(&head);

    // push(&head, "hello");
    // push(&head, "my");
    // print(head);
    // pop(&head);
    // pop(&head);
    // pop(&head);
    shell_loop();

    // printf("%s\n",top(head));

    // pop(&head);
    // pop(&head);

    return 0;
}