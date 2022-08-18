/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "stack.hpp"

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 1024

int i;
pnode head;
int flag;

struct _ARGS
{
    char *data;
    int sock;
};

void *thread_func(void *args)
{
    int new_fd = *(int *)args;
    char buf[MAXDATASIZE];
    int flag = 1;
    while (flag == 1)
    {
        memset(buf, '\0', MAXDATASIZE);
        int t = recv(new_fd, buf, MAXDATASIZE, 0);
        if (t == -1)
        {
            perror("recv");
        }
        else if (t == 0)
        {
            continue;
        }
        else // Parsing the input from the client:
        {
            // printf("input is %s\n", buf);

            char *index = buf;
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

            if (strcmp(command, "PUSH ") == 0)
            {
                

                push(&head, index);

            }
            else if (strcmp(command, "POP  ") == 0)
            {
                pop(&head);

            }
            else if (strcmp(command, "SHUT ") == 0)
            {
                clear(&head);
                break;

            }
            else if (strcmp(command, "TOP  ") == 0)
            {
                const char *prefix = "OUTPUT:";
                char to_send[MAXDATASIZE];
                memset(to_send,'\0',MAXDATASIZE);
                strcat(to_send,prefix);
                strncat(to_send,top(head),MAXDATASIZE-sizeof(prefix));
                printf("%s\n", to_send);

                send(new_fd, to_send, MAXDATASIZE, 0);

            }
            else if (strcmp(command, "PRINT") == 0)
            {
                print(head);

            }
            else if (strcmp(command, "CLEAR") == 0)
            {

                clear(&head);

            }
            else
            {
                printf("Error reading a command\n");
            }
        }
    }
    close(new_fd);

    pthread_exit(&i);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    head = NULL; // our stack's pointer

    // init our mutex lock

    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    flag = 1;
    int clients[10] = {0};
    while (flag == 1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        // for (int i = 0; i < 10; i++)
        // {
        //     if(clients[i]==0)
        //     {
        //         client[i] = new_fd;
        //         break;
        //     }
        // }
        
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t id;
        int ret;

        ret = pthread_create(&id, NULL, &thread_func, &new_fd);
        if (ret == 0)
        {
            printf("Thread created successfully.\n");
        }
        else
        {
            printf("Thread not created.\n");
        }
    }
    
    return 0;
}