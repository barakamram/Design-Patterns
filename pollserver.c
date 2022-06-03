#define PORT "5678"
#include "reactor.h"

int count, lis;
struct pollfd *pfds;
char buf[1024];
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// ************************ Reactor ************************ //
preactor newReactor() {
    preactor prctr = (preactor)(malloc(sizeof(reactor)));
    return prctr;
}
void InstallHandler(preactor reactor, pfunc newpf, int file_des) {
    reactor->pf = newpf;
    reactor->FID = file_des;
    papplication pa = (papplication)(malloc(sizeof(application)));
    pa->FID = file_des;
    pa->prc = reactor;
    pthread_create(&reactor->TID, NULL, newpf, pa);
}
void RemoveHandler(preactor prc, int fd_free) {
    pthread_join(prc->TID, NULL);
    prc->FID = -1;
    prc->pf = NULL;
}
// ********************************************************* //

int get_lis_sock(void) { //get listen sock
    int lis, inf;
    int x = 1;
    struct addrinfo hints, *ai, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((inf = getaddrinfo(NULL, PORT, &hints, &ai)) != 0){
        fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(inf));
        exit(1);
    }
    for (p = ai; p != NULL; p = p->ai_next){
        lis = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (lis < 0)
            continue;
        setsockopt(lis, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(int));
        if (bind(lis, p->ai_addr, p->ai_addrlen) < 0){
            close(lis);
            continue;
        }
        break;
    }
    freeaddrinfo(ai);
    if (p == NULL)
        return -1;
    if (listen(lis, 10) == -1)
        return -1;
    return lis;
}
void add_to_pfds(struct pollfd *pfds[], int newfd, int *count, int *size){
    if (*count == *size){
        *size *= 2;
        *pfds = realloc(*pfds, sizeof(**pfds) * (*size));
    }
    (*pfds)[*count].fd = newfd;
    (*pfds)[*count].events = POLLIN;
    (*count)++;
}
void del_from_pfds(struct pollfd pfds[], int i, int *count){
    pfds[i] = pfds[*count - 1];
    (*count)--;
}
void *threadF(void *arg){
    papplication papp = &((papplication)arg)[0];
    int newFd = papp->FID;
    char buf[1024];
    for(;;)  {
        int b = recv(newFd, buf, sizeof(buf), 0);
        if (b <= 0){
            printf("socket %d disconnected\n", newFd);
            close(newFd);
            return NULL;
        } else {
            for (int i = 0; i < count + 1; i++){
                int cli_fd = pfds[i].fd;
                if (cli_fd != lis && cli_fd != newFd)
                    send(cli_fd, buf, b, 0);
            }
            bzero(buf, 1024);
        }
    }
}
int main(void) {
    int newfd;
    struct sockaddr_storage sas;
    socklen_t addrlen;
    char c[INET6_ADDRSTRLEN];
    count = 0;
    int size = 5;
    pfds = (struct pollfd *)malloc(sizeof(*pfds) * size);
    lis = get_lis_sock();
    if (lis == -1){
        fprintf(stderr, "error in get_lis_sock\n");
        exit(1);
    }
    pfds[0].fd = lis;
    pfds[0].events = POLLIN;
    count = 1;
    printf("Poll Server waiting for connections... \n");
    for (;;){
        int p_count = poll(pfds, count, -1);
        if (p_count == -1){
            perror("error in poll");
            exit(1);
        }
        for (int i = 0; i < count; i++){
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == lis) {
                    addrlen = sizeof sas;
                    newfd = accept(lis,(struct sockaddr *)&sas,&addrlen);
                    if (newfd == -1)
                        perror("accept");
                    else {
                        add_to_pfds(&pfds, newfd, &count, &size);

                        printf("new connection from %s on " "socket %d\n", inet_ntop(sas.ss_family, get_in_addr((struct sockaddr *)&sas), c, INET6_ADDRSTRLEN),newfd);
                        preactor reactor = (preactor)newReactor();
                        InstallHandler(reactor, &threadF, newfd);
                    }
                } else {
                    int nb = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender = pfds[i].fd;
                    if (nb <= 0){
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &count);
                    } else {
                        for (int j = 0; j < count; j++){
                            int dest = pfds[j].fd;
                            if (dest != lis && dest != sender){
                                send(dest, buf, nb, 0);
                                    // perror("error in send");
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
