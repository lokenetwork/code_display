/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

const int max_threads_number = 100;
int current_threads_number = 0;
bool is_end = false;

void thread(int num)
{
    current_threads_number++;
    if( current_threads_number > max_threads_number ){
        is_end = true;
    }
    int i= 1;
    for(i=0;i<1;i++){
        printf("This is a pthread %d.\n",sizeof(int));
    }
    pthread_t thread_id_1;
    int ret=pthread_create(&thread_id_1,NULL,(void *) thread,2);
    if( ret != 0 ){
        printf("pthread_create error.\n");
    }

}



int main(void)
{
    pthread_t thread_id_1;
    int ret=pthread_create(&thread_id_1,NULL,(void *) thread,1);
    if( ret != 0 ){
        printf("pthread_create error.\n");
    }
    /*
     * 如何不用pthread_join来通信，因为我子进程可能会递归多N多子进程，直接一个pthread_join不行。好像管道可以，
     * 我就想，子进程通知main，不管哪个子进程通知都行。有什么方法？
     */
    //pthread_join(thread_id_1,NULL);

    //可不可以一直循环读取一个值？
    for (;;) {
        if( is_end == true ){
            break;
        }
    }

    printf("This is the main end.\n");
    return (0);
}
