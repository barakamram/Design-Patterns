#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#define PORT "6789"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
    int sockfd, len, inf;
    char buf[1024];
    struct addrinfo hints, *si, *p;
    char c[INET6_ADDRSTRLEN];
    if (argc != 2) {
        fprintf(stderr,"please insert address after './client ' \n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((inf = getaddrinfo(argv[1], PORT, &hints, &si)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(inf));
        return 1;
    }
    for(p = si; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("error in socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("error in connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "connection failed\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),c ,sizeof c);
    printf("Connecting to %s\n", c);
    freeaddrinfo(si);
    char temp[2048] = {'\0'};
    char str[2048] = {'\0'};
    scanf("%s", str);
    while (1) {
        if(send(sockfd, str, strlen(str), 0) == -1){
            printf("error in send");
            exit(1);
        }
        bzero(str, 2048);
        if ((len = recv(sockfd, str,2048, 0)) == -1) {
            perror("error in recv");
            exit(1);
        }
        str[len] = '\0';
        printf("--- Caesar Cipher: ");
        for (int i = 0; i < strlen(str); i++) {
            printf("%c", str[i]);
        }
        printf(" ---\n");
        bzero(str, 2048);
        scanf("%s", str);
    }
    close(sockfd);
    return 0;
}
