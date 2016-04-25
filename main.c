/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>

void get_redis_connect(redisContext * redis_connect){
    redis_connect = redisConnect("127.0.0.1", 6379);
    if (redis_connect == NULL || redis_connect->err) {
        if (redis_connect) {
            printf("Error: %s\n", redis_connect->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }
}
void thread(int num) {
    redisContext * redis_connect;
    get_redis_connect(redis_connect);
    /*
    redis_connect = redisConnect("127.0.0.1", 6379);
    if (redis_connect == NULL || redis_connect->err) {
        if (redis_connect) {
            printf("Error: %s\n", redis_connect->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }
     */

    char redis_get_cmd[20];
    memset(redis_get_cmd, 0, 20);
    switch (num) {
        case 0:
            strcpy(redis_get_cmd, "get name");
            break;
        case 1:
            strcpy(redis_get_cmd, "get age");
            break;
        case 2:
            strcpy(redis_get_cmd, "get weight");
            break;
        case 3:
            strcpy(redis_get_cmd, "get address");
            break;
        case 4:
            strcpy(redis_get_cmd, "get birthday");
            break;
    }
    redisReply *redis_reply = redisCommand(redis_connect,redis_get_cmd);
    printf("redis_reply is no problem %d;The redis_get_cmd is%s; redis_reply->type is %d; redis_reply->str :%s\n",num,redis_get_cmd, redis_reply->type, redis_reply->str);
    redisFree(redis_connect);
}

int main(void) {


    int ret;
    pthread_t thread_id_1, thread_id_2, thread_id_3, thread_id_4, thread_id_0;
    for (int _i = 0; _i < 10; _i++) {
        for (int i = 0; i < 5; i++) {
            switch (i) {
                case 0:
                    ret = pthread_create(&thread_id_0, NULL, (void *) thread, i);
                    break;
                case 1:
                    ret = pthread_create(&thread_id_1, NULL, (void *) thread, i);
                    break;
                case 2:
                    ret = pthread_create(&thread_id_2, NULL, (void *) thread, i);
                    break;
                case 3:
                    ret = pthread_create(&thread_id_3, NULL, (void *) thread, i);
                    break;
                case 4:
                    ret = pthread_create(&thread_id_4, NULL, (void *) thread, i);
                    break;
            }
            if (ret != 0) {
                printf("pthread_create error %d\n", i);
            }
        }
        pthread_join(thread_id_0, NULL);
        pthread_join(thread_id_1, NULL);
        pthread_join(thread_id_2, NULL);
        pthread_join(thread_id_3, NULL);
        pthread_join(thread_id_4, NULL);
        printf("_i is %d\n",_i);
    }
    printf("This is the main .\n");
    return (0);
}
