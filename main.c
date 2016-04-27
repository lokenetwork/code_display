/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
void thread(int num)
{

}

int main(void)
{
    pthread_t thread_id;
    int ret=pthread_create(&thread_id,NULL,(void *) thread,15);

    printf("thread_id_1 is %x\n",thread_id);

   /*
    * 如何不用pthread_join来通信，因为我子进程可能会递归多N多子进程，直接一个pthread_join不行。好像管道可以，
    * 我就想，子进程通知main，不管哪个子进程通知都行，就是main要等待所有子进程执行完。有什么方法？
    */
    //pthread_join(thread_id_1,NULL);
    return (0);
}
