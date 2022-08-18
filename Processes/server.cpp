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
#include <fcntl.h>
#include <sys/mman.h>
#include "stack.hpp"

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 1024 + sizeof("OUTPUT:")

#define WORD_SIZE 1024

#define STACK_MAX_SIZE 1024*1024*80 //10.485 MB

int flag = 1;

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
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
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
    //Using mmap and MAP_SHARED we share our stack's memory with all the processes.
    
    char *stack = (char *)mmap(NULL, STACK_MAX_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //variable top will help us know where to put our new word in the stack.
    int *stack_top = (int *)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *stack_top = 0;
    //SET UP FOR THE FCNTL LOCK
    //Open a file called LOCK_FILE, open a relevant FD, then using fcntl we lock that FD when necessary.
    char *file = (char*)malloc(sizeof("LOCK_FILE"));
    file = "LOCK_FILE";
    int fd;
    struct flock lock;
    FILE *dst = fopen(file, "w");
    fd = open(file, O_WRONLY);
    memset(&lock, 0, sizeof(lock));

    printf("server: waiting for connections...\n");
    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork())
        { // this is the child process
            // close(sockfd); // child doesn't need the listener
            char buf[MAXDATASIZE];
            memset(buf, '\0', MAXDATASIZE);
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_RDLCK;
            while (1)
            {
                // RECIEVE MESSEGE FROM CLIENT
                
                int t = recv(new_fd, buf, MAXDATASIZE, 0);
                if (t == -1)
                {
                    perror("recv");
                }
                else if (t == 0) // no new msg read
                {
                    continue;
                }

                // PROCCESS MESSAGE FROM CLIENT
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
                    //lock the FD
                    lock.l_type = F_WRLCK;
                    fcntl(fd, F_SETLKW, &lock);

                    push(stack,index,stack_top);

                    //unlock the FD 
                    lock.l_type = F_UNLCK;
                    fcntl(fd, F_SETLKW, &lock);
                }
                else if (strcmp(command, "POP  ") == 0)
                {
                    //lock the FD
                    lock.l_type = F_WRLCK;
                    fcntl(fd, F_SETLKW, &lock);

                    pop(stack,stack_top);

                    //unlock the FD 
                    lock.l_type = F_UNLCK;
                    fcntl(fd, F_SETLKW, &lock);
                }
                else if (strcmp(command, "TOP  ") == 0)
                {
                    if(*stack_top==0)
                    {
                        
                    }
                    //lock the FD
                    lock.l_type = F_WRLCK;
                    fcntl(fd, F_SETLKW, &lock);

                    const char *to_send = top(stack,stack_top);

                    send(new_fd, to_send, MAXDATASIZE, 0);
                    printf("%s\n",to_send);
                    free((void*)to_send);
                    //unlock the FD 
                    lock.l_type = F_UNLCK;
                    fcntl(fd, F_SETLKW, &lock);
                }
                else
                {
                    printf("Error reading a command\n");
                }
            }
            close(fd);
            close(new_fd);
            exit(0);
        }
        close(new_fd); // parent doesn't need this
    }

    return 0;
}