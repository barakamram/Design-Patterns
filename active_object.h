#include <pthread.h>
#include "queue.h"

typedef struct active_object {
    void* (*f1)(void*);
    void* (*f2)(void*);
    queue* que;
    pthread_t *pid;
    int run;
}AO;

AO *newAO(queue* Q, void* f1, void* f2);
void* runAO(void*);
void destroyAO(AO*);
