/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int my_count = 0;

void thread() {
    for (int i = 0; i < 100; i++) {
        //我开了5个线程模拟并发写，怎么没有冲突，每次都是500的结果
        my_count++;
    }
}


int main(void) {
    int ret;
    pthread_t thread_id_1, thread_id_2, thread_id_3,thread_id_4,thread_id_5;
    ret = pthread_create(&thread_id_1, NULL, (void *) thread, NULL);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    ret = pthread_create(&thread_id_2, NULL, (void *) thread, NULL);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    ret = pthread_create(&thread_id_3, NULL, (void *) thread, NULL);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    ret = pthread_create(&thread_id_4, NULL, (void *) thread, NULL);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    ret = pthread_create(&thread_id_5, NULL, (void *) thread, NULL);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    pthread_join(thread_id_1,NULL);
    pthread_join(thread_id_2,NULL);
    pthread_join(thread_id_3,NULL);
    pthread_join(thread_id_4,NULL);
    pthread_join(thread_id_5,NULL);

    printf("my account is %d\n", my_count);
    printf("This is the main end.\n");
    return (0);
}
