#include <stdlib.h>
#include <stdio.h>

#include "active_object.h"
#include "queue.h"

void* runAO(void* temp_ao) {
  AO *ao = (AO*)temp_ao;
  while (ao->run) {
    void* handle = ao->f1 (deQ (ao->que));
    void* res = ao->f2 (handle);
  }
  free(ao->pid);
  free(ao);
  printf("ao terminated\n");
}

AO* newAO(queue* Q, void* f1, void* f2) {
  AO *ao = (AO*)malloc(sizeof(AO));
  ao->f1 = f1;
  ao->f2 = f2;
  ao->que = Q;
  ao->run = 1;
  ao->pid = (pthread_t*)malloc(sizeof(pthread_t));
  pthread_create(ao->pid, NULL, runAO, (void*) ao);
  printf("ao created\n");
  return ao;
}

void destroyAO(AO* ao) {
  printf("destroying ao\n");
  ao->run=false;
  free(ao->pid);
  free(ao);
}
