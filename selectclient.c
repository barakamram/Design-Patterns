#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "5678"
#define MAX 30

int connected ,sock;
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
      return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void *recvF(void *arg) {
    char buf[1024] = {0};
    int len = 0;
    while ((len = recv(sock, buf, 1024, 0)) != -1) {
        if (!len) {
          connected = 0;
          break;
        }
        printf("\nRECV: %s\n",buf);
        if (!strcmp(buf, "exit")) {
          connected = 0;
          break;
        }
        bzero(buf, 1024);
    }
    return 0;
}
void *sendF(void *arg) {
    char c[1024] = {0};
    while (connected != 0) {
        printf(">> ");
        gets(c);
        if (strncmp(c,"exit",4) == 0) {
            send(sock,"exit",4,0);
            connected = 0;
            break;
        }
        if (send(sock, c, strlen(c) + 1, 0) == -1)
          perror("error in send");
        bzero(c, 1024);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char buf[MAX];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    if (argc != 2) {
        fprintf(stderr,"please insert address after './selectclient ' \n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("error in socket");
            continue;
        }
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("error in connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "connection failed\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("successfully connected to %s\n", s);
    connected = 1;
    freeaddrinfo(servinfo);
    pthread_t pid1,pid2;
    pthread_create(&pid1, NULL, recvF, NULL);
    pthread_create(&pid2, NULL, sendF, NULL);
    pthread_join(pid1, NULL);
    pthread_join(pid1, NULL);
    pthread_kill(pid2, 0);
    pthread_kill(pid2, 0);
    close(sock);
    return 0;
}
