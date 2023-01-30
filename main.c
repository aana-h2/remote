#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/unistd.h>

#define CONSUMER 50
#define PRODUCER 1

// 使用一个cond_variable和mutex对此进行保护。
pthread_mutex_t mutex_v = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_v = PTHREAD_COND_INITIALIZER;
int v = 0; // 小于10时就进行生成，大于0时才能进行消费。

// 类似于3个消费者，一个生产者。
_Noreturn void *inc(void *_) {
    while (1) {
        pthread_mutex_lock(&mutex_v);
        if (v < 5) {
            v = v + 3 > 5 ? 5 : v + 3;
            pthread_cond_signal(&cond_v);
            pthread_mutex_unlock(&mutex_v);
            usleep(1000);
//            printf("producer(%ld) -- %d\n", pthread_self(), v);
        }
        pthread_mutex_unlock(&mutex_v);
    }
}

_Noreturn void *dec(void *_) {
    while (1) {
        pthread_mutex_lock(&mutex_v);
//        pthread_cond_wait(&cond_v, &mutex_v);

        if (v < 1) {
            pthread_cond_wait(&cond_v, &mutex_v);
        }
        v -= 1;
        if (v < 0){
            exit(-1);
        }
//        printf("consumer(%ld) -- %d\n", pthread_self(), v);
        pthread_mutex_unlock(&mutex_v);

        usleep(100);
    }
}

int main() {
    pthread_t producer[PRODUCER];
    pthread_t consumers[CONSUMER];

    for (int i = 0; i < PRODUCER; ++i) pthread_create(&producer[i], NULL, inc, NULL);
    for (int i = 0; i < CONSUMER; ++i) pthread_create(&consumers[i], NULL, dec, NULL);

    for (int i = 0; i < PRODUCER; ++i) pthread_join(producer[i], NULL);
    for (int i = 0; i < CONSUMER; ++i) pthread_join(consumers[i], NULL);

    return 0;
}
