
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;
pthread_mutex_t mtx;

class guard{
    public:
    guard() {
      pthread_mutex_lock(&mtx);
      cout << "starts guarding" << endl;
    }

    ~guard() {
      pthread_mutex_unlock(&mtx);
      cout << "finishes guarding" << endl;
    }
};
void* guarding(void* n) {
    guard g;
    sleep(5);
    cout << n << endl;
    return 0;
}

int main() {
    pthread_t pid1, pid2;
    int n = 1;
    int m = 5;
    pthread_create(&pid1,NULL,&guarding,&n);
    pthread_create(&pid2,NULL,&guarding,&m);
    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
}
