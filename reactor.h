#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#define nullptr ((void*)0)
typedef void *(*pfunc)(void *arg);

typedef struct Reactor {
    int FID;
    pthread_t TID;
    pfunc pf;
} reactor, *preactor;

typedef struct application {
    int FID;
    preactor prc;
} application, *papplication;

preactor newReactor();
void InstallHandler(preactor reactor, pfunc newpf, int file_des);
void RemoveHandler(preactor prc, int fd_free);
