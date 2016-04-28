/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>

pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

int my_count = 0;

int add_type=2;


void thread() {
    for (int i = 0; i < 100000; i++) {
        if( add_type == 1 ){
            //printf("线程锁相加 starting\n");
            pthread_mutex_lock(&count_lock);
            my_count++;
            pthread_mutex_unlock(&count_lock);
        }else {
            //printf("原子相加 starting\n");
            __sync_fetch_and_add( &my_count, 1 );

        }
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
