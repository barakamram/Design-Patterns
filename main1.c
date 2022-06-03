#include"queue.h"
#include <string.h>
#include <stdlib.h>

queue* createQ() {
    queue* Q = (queue*)malloc(sizeof(queue));

    Q->head = Q->tail = nullptr;
    Q->lastQ = nullptr;
    Q->size = 0;
    pthread_mutex_init(&Q->p_mutex, NULL);
    pthread_cond_init(&Q->p_cond, NULL);
    return Q;
}

void destroyQ(queue* Q) {
    while (Q->size!=0)
        deQ(Q);
    if (Q->lastQ!=NULL)
        free(Q->lastQ);
    pthread_cond_broadcast(&Q->p_cond);
    pthread_cond_destroy(&Q->p_cond);
    pthread_mutex_destroy(&Q->p_mutex);
    free(Q);
}

bool enQ(void* n, queue* Q) {
    pthread_mutex_lock(&(Q->p_mutex));
    if (Q->size==0) {
        struct node* new_node = (struct node*)malloc(sizeof(struct node));
        new_node->value = malloc(2048);
        memcpy(new_node->value, n, 2048);
        new_node->next = new_node->prev = nullptr;
        Q->head = Q->tail = new_node;
        Q->size++;
        pthread_mutex_unlock(&(Q->p_mutex));
        pthread_cond_signal(&Q->p_cond);
        return true;
    }
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->value = malloc(2048);
    memcpy(new_node->value, n, 2048);
    Q->tail->prev = new_node;
    new_node->next = Q->tail;
    new_node->prev = nullptr;
    Q->tail = new_node;
    Q->size++;
    pthread_mutex_unlock(&(Q->p_mutex));
    return true;
}

void* deQ(queue* Q) {
    pthread_mutex_lock(&(Q->p_mutex));
    if (Q->size==-1)
        return NULL;
    if (Q->size==0)
        pthread_cond_wait(&Q->p_cond, &Q->p_mutex);
    if (Q->size==0)
        pthread_exit(NULL);
    if (Q->lastQ!=NULL)
        free(Q->lastQ);
    if (Q->size==1) {
        Q->lastQ = malloc(2048);
        memcpy(Q->lastQ, Q->head->value, 2048);
        free(Q->head->value);
        free(Q->head);
        Q->size--;
        pthread_mutex_unlock(&(Q->p_mutex));
        return Q->lastQ;
    }
    Q->lastQ = malloc(2048);
    memcpy(Q->lastQ, Q->head->value, 2048);
    struct node* temp_node = Q->head->prev;
    temp_node->next = nullptr;
    free(Q->head->value);
    free(Q->head);
    Q->head = temp_node;
    Q->size--;
    pthread_mutex_unlock(&(Q->p_mutex));
    return Q->lastQ;
}
