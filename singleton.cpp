#include <iostream>
#include <pthread.h>

using namespace std;
pthread_mutex_t p_mutex;

template <typename T>
class Singleton {
    private:
      static Singleton *instance;
      T t;
      Singleton(T t_temp) {
        t = t_temp;
      }
    public:
      static Singleton *Instance(T t_temp);
      void Destroy();
};

template <typename T>
Singleton<T> *Singleton<T>::instance = NULL;

template <typename T>
Singleton<T> *Singleton<T>::Instance(T t_temp) {
    if (instance == NULL) {
        pthread_mutex_lock(&p_mutex);
        instance = new Singleton(t_temp);
    }
    pthread_mutex_unlock(&p_mutex);
    return instance;
}

template <typename T>
void Singleton<T>::Destroy() {
    delete(instance);
    instance = nullptr;
}

int main() {
    FILE *f;
    Singleton<FILE *> *x = Singleton<FILE *>::Instance(f);
    Singleton<FILE *> *y = Singleton<FILE *>::Instance(f);
    cout << "file1: "<< x << endl;
    cout << "file2: "<< y <<endl;
}
