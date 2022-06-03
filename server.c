#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "queue.h"
#include "active_object.h"


pthread_t thread_id[BACKLOG];
int new_fd, sockfd;
queue* first;
queue* second;
queue* third;
AO *ao1;
AO *ao2;
AO *ao3;

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *cli_thread(void *newfd) {
    int new_fd = *(int*)newfd;
    pthread_detach(pthread_self());
    char buf[1024];
    bool connection = true;
    request *val = nullptr;

    while (connection) {
        int len = recv(new_fd, buf, sizeof(buf), 0);
        if (len <=0) {
            connection = 0;
            pthread_exit(NULL);
        }
        buf[len] = '\0';
        val = (request*)malloc(sizeof(request));
        val->fd = new_fd;
        memcpy(val->txt, buf, sizeof(buf));
        printf("Client %d send new text: %s \n", val->fd, val->txt);
        enQ1(val);
    }
    free(val);
    close(new_fd);
}

void sig_handler(int signum) {
    if (signum == SIGINT) {
        for (size_t i = 0; i < BACKLOG; i++) {
            close(new_fd);
        }
        close(sockfd);
        for (size_t i = 0; i < BACKLOG; i++) {
            pthread_cancel(thread_id[i]);
        }
        destroyAO(ao1);
        destroyAO(ao2);
        destroyAO(ao3);
        destroyQ(first);
        destroyQ(second);
        destroyQ(third);
        exit(1);
    }
}

void* enQ1(void* w) {
    enQ(w, first);
    return w;
}
void* enQ2(void* w) {
    enQ(w, second);
    return w;
}
void* enQ3(void* w) {
    enQ(w, third);
    return w;
}
void* f1(void* w) {
    request* val = (request*)w;
    int size = strlen(val->txt);
    for (size_t j = 0; j < size; j++) {
        if (val->txt[j]>='a'&&val->txt[j]<='z')
            val->txt[j] = (val->txt[j]-96)%26+97;
        else if (val->txt[j]>='A'&&val->txt[j]<='Z')
            val->txt[j] = (val->txt[j]-64)%26+65;
    }
    return val;
}
void* f2(void* w) {
    request *val = (request*)w;
    int size = strlen(val->txt);
    for (size_t j = 0; j < size; j++) {
        if (val->txt[j]>='a'&&val->txt[j]<='z')
            val->txt[j] -= 32;
        else if (val->txt[j]>='A'&&val->txt[j]<='Z')
            val->txt[j] += 32;
    }
    return w;
}
void* response(void* w){
    request* val = (request*)w;
    int new_fd = val->fd;
    if (send(new_fd, val->txt, 2048, 0) == -1)
        perror("error in send");
    printf("%s has sent to client %d\n\n", val->txt,new_fd);
    return w;
}

void* same_same(void* w) {
    return w;
}

int main(void) {
    first = createQ();
    second = createQ();
    third = createQ();
    ao1 = newAO(first, f1,enQ2);
    ao2 = newAO(second, f2 , enQ3);
    ao3 = newAO(third, same_same, response);
    struct addrinfo hints, *si, *p;
    struct sigaction sa;
    struct sockaddr_storage otherAddr;
    socklen_t socklen;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int inf;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((inf = getaddrinfo(NULL, PORT, &hints, &si)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(inf));
        return 1;
    }
    for(p = si; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("error in socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("error in setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("error in bind");
            continue;
        }
        break;
    }
    freeaddrinfo(si);
    if (p == NULL)  {
        fprintf(stderr, "Binding failed\n");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1) {
        perror("error in listen");
        exit(1);
    }
    printf("Waiting for connections...\n");
    int j = 0;
    signal (SIGINT,sig_handler);
        while(1) {
        socklen = sizeof otherAddr;
        new_fd = accept(sockfd, (struct sockaddr *)&otherAddr, &socklen);
        if (new_fd == -1) {
            perror("error in accept");
            continue;
        }
        inet_ntop(otherAddr.ss_family,
        get_in_addr((struct sockaddr *)&otherAddr), s, sizeof s);
        printf("--- Got connection from %s ---\n", s);
        pthread_create(&thread_id[j%BACKLOG], NULL, cli_thread, &new_fd);
        j++;
    }
    return 0;
}
