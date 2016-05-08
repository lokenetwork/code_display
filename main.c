#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <errno.h>
#include <hiredis/hiredis.h>
#include "cJSON/cJSON.h"
#include "unidirectional_queue/unidirectional_queue.h"

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <pthread.h>
#include <strings.h>
#include <stdbool.h>


void deal_fastcgi_request(int *connfd) {

    size_t n;
    char client_request_data[3000];
    memset(client_request_data, 0, 3000);

    n = read(*connfd, client_request_data, 3000);

    if (n < 0) {
        if (errno != EINTR) {
            perror("read error");
        }
    }
    if (n == 0) {
        //connfd is closed by client
        close(*connfd);
        printf("client exit\n");
    }
    //client exit
    if (strncmp("exit", client_request_data, 4) == 0) {
        close(*connfd);
        printf("client exit\n");
    }


    printf("fastcgi request data is %s\n", client_request_data);
    //send(connfd, "loken", 5, 0);
    close(connfd);


}


int main(int argc, char **argv) {

    int listenfd, connfd;
    int serverPort = 9999;
    int listenq = 1024;
    pid_t childpid;
    char buf[MAX_SEARCH_WORDS_LEN];
    socklen_t socklen;

    struct sockaddr_in cliaddr, servaddr;
    socklen = sizeof(cliaddr);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serverPort);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket error");
        return -1;
    }

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));


    if (bind(listenfd, (struct sockaddr *) &servaddr, socklen) < 0) {
        perror("bind error");
        return -1;
    }
    if (listen(listenfd, listenq) < 0) {
        perror("listen error");
        return -1;
    }

    printf("pullwords server startup,listen on port:%d\n", serverPort);
    for (; ;) {
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);


        //创建线程处理请求
        pthread_t pthread_id;
        int pthread_create_result;
        pthread_create_result = pthread_create(&pthread_id, NULL,
                                               (void *) deal_fastcgi_request, &connfd);
        if (pthread_create_result != 0) {
            printf("pthread_create error.\n");
        }

    }
}
