#define PORT "6789"
#define BACKLOG 15
#define nullptr ((void*)0)

typedef struct request {
  int fd;
  char txt[1024];
}request;

void* enQ1(void* w);
void* enQ2(void* w);
void* enQ3(void* w);
void* f1(void* w);
void* f2(void* w);
void* response(void* w);
void* same_same(void* w);
