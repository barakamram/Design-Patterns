#pragma once
#include <stdbool.h>
#include <pthread.h>

#define nullptr ((void*)0)

struct node {
    void* value;
    struct node *prev;
    struct node *next;
};
typedef struct queue {
    struct node *head;
    struct node *tail;
    int size;
    pthread_mutex_t p_mutex;
    pthread_cond_t p_cond;
    void* lastQ;
}queue;

queue* createQ();
void destroyQ(queue*);
bool enQ(void*, queue*);
void* deQ(queue*);
