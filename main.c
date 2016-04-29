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
#include "cJSON.h"

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <pthread.h>
#include <strings.h>
#include <stdbool.h>

#define MAXLINE 1024
//分词结果的最大长度
#define WORD_RESULT_LEN 200
//可处理的最大的客户端发送的搜索词长度
#define MAX_SEARCH_WORDS_LEN 200
//最多开多少个线程处理分词
#define MAX_CUT_WORD_THREAD_NUMBER 30
char *english_pattern = "^[^\u4e00-\u9fa5]++";

//typedef struct sockaddr  SA;
const int8_t chinese_word_byte = sizeof("字") - 1;
const int8_t chinese_char_byte = sizeof("字");
const char *redis_get_common = "get ";
struct TREE_CUT_WORDS_STUFF {
    char *start_word_item;
    bool have_first_not_cn_word;
    char *first_not_cn_word;
    char *had_deal_search_word;
};

//父线程传参給子线程的结构体
struct TREE_CUT_WORDS_DATA {
    //父级的分词结果
    char *cut_word_result;

    //父级还没处理，剩余的搜索词
    char *remainder_words;

    //父级的分词权重
    int *words_weight;

};

char *substring(char *ch, int pos, int length) {
    char *pch = ch;
//定义一个字符指针，指向传递进来的ch地址。
    char *subch = (char *) calloc(sizeof(char), length + 1);
//通过calloc来分配一个length长度的字符数组，返回的是字符指针。
    int i;
//只有在C99下for循环中才可以声明变量，这里写在外面，提高兼容性。
    pch = pch + pos;
//是pch指针指向pos位置。
    for (i = 0; i < length; i++) {
        subch[i] = *(pch++);
//循环遍历赋值数组。
    }
    subch[length] = '\0';//加上字符串结束符。
    return subch;       //返回分配的字符数组地址。
}

//初始化分词结果结构体
void init_tree_cut_words_data(struct TREE_CUT_WORDS_DATA *cut_words_data) {
    (*cut_words_data).cut_word_result = (char *) malloc(WORD_RESULT_LEN);
    memset((*cut_words_data).cut_word_result, 0, WORD_RESULT_LEN);
    (*cut_words_data).words_weight = (int *) malloc(sizeof(int));
    memset((*cut_words_data).words_weight, 0, sizeof(int));
    (*cut_words_data).remainder_words = (char *) malloc(MAX_SEARCH_WORDS_LEN);
    memset((*cut_words_data).remainder_words, 0, MAX_SEARCH_WORDS_LEN);
}
void add_words_weight(int *weight, int type) {
    switch (type) {
        case 1:
            *weight += 5;
            break;
        case 2:
            *weight += 10;
            break;
    }
}
/*从字符串的左边截取n个字符*/
char *left(char *dst, char *src, int n) {
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if (n > len) n = len;
    while (n--) *(q++) = *(p++);
    *(q++) = '\0'; /*有必要吗？很有必要*/
    return dst;
}

/*方法一，不改变字符串a,b, 通过malloc，生成第三个字符串c, 返回局部指针变量*/
char *join_char(char *a, char *b) {
    char *c = (char *) malloc(strlen(a) + strlen(b) + 1); //局部变量，用malloc申请内存
    memset(c, 0, strlen(a) + strlen(b) + 1);
    if (c == NULL) exit(1);
    char *tempc = c; //把首地址存下来
    while (*a != '\0') {
        *c++ = *a++;
    }
    while ((*c++ = *b++) != '\0') { ;
    }
    //注意，此时指针c已经指向拼接之后的字符串的结尾'\0' !
    return tempc;//返回值是局部malloc申请的指针变量，需在函数调用结束后free之
}

/*
 * return 1: success
 * return 0: error;
 * */
int8_t pecl_regx_match(PCRE2_SPTR subject, PCRE2_SPTR pattern, int *match_offset, int *match_str_lenght) {

    pcre2_code *re;
    PCRE2_SPTR name_table;

    int crlf_is_newline;
    int errornumber;
    int i;
    int namecount;
    int name_entry_size;
    int rc;
    int utf8;

    uint32_t option_bits;
    uint32_t newline;

    PCRE2_SIZE erroroffset;
    PCRE2_SIZE *ovector;

    size_t subject_length;
    pcre2_match_data *match_data;



    //char * _pattern = "[2-9]";
    char *_pattern = "[^\u4e00-\u9fa5]++";

    subject_length = strlen((char *) subject);

    re = pcre2_compile(
            pattern,               /* the pattern */
            PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
            0,                     /* default options */
            &errornumber,          /* for error number */
            &erroroffset,          /* for error offset */
            NULL);                 /* use default compile context */

    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("PCRE2 compilation failed at offset %d: %s\n", (int) erroroffset,
               buffer);
        return 0;
    }

    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    rc = pcre2_match(
            re,                   /* the compiled pattern */
            subject,              /* the subject string */
            subject_length,       /* the length of the subject */
            0,                    /* start at offset 0 in the subject */
            0,                    /* default options */
            match_data,           /* block for storing the result */
            NULL);                /* use default match context */
    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                break;
                /*
                Handle other special cases if you like
                */
            default:
                printf("Matching error %d\n", rc);
                break;
        }
        pcre2_match_data_free(match_data);   /* Release memory used for the match */
        pcre2_code_free(re);                 /* data and the compiled pattern. */
        return 0;
    }
    ovector = pcre2_get_ovector_pointer(match_data);
    *match_offset = (int) ovector[0];


    if (rc == 0) {
        printf("ovector was not big enough for all the captured substrings\n");
    }
    for (i = 0; i < rc; i++) {
        size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
        *match_str_lenght = (int) substring_length;


        PCRE2_SPTR substring_start = subject + ovector[2 * i];

        if (i > 1) {
            //wrong. i can't large than one.lazy do.
        }
    }
}

void add_word_item_to_single_tree_result(char *tree_result, const char *word_item) {
    strcat(tree_result, ",");
    strcat(tree_result, word_item);
}

//中文裁剪
void chinese_word_cut(char *tree_result_word, char **search_word_point, int *words_weight, const char *chinese_words) {
    strcat(tree_result_word, ",");
    strcat(tree_result_word, chinese_words);
    add_words_weight(words_weight, 2);
    //搜索词前进
    *search_word_point = *search_word_point + strlen(chinese_words);
}

//匹配不到redis词条最后裁剪方法
void not_match_word_end_cut(char *tree_result_word, char **search_word_point, int *words_weight) {
    strcat(tree_result_word, ",");
    strcat(tree_result_word, *search_word_point);
    add_words_weight(words_weight, 1);
    //搜索词前进
    int remain_word_lenght = (int) strlen(*search_word_point);
    *search_word_point = *search_word_point + remain_word_lenght;
}




void get_redis_connect(redisContext **redis_connect) {
    *redis_connect = redisConnect("127.0.0.1", 6379);
    if (redis_connect == NULL || (*redis_connect)->err) {
        if (redis_connect) {
            printf("Error: %s\n", (*redis_connect)->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }
}



/*
 * 裁剪英文作为一个词,
 * 1 stand for success,
 * 小于1代表失败
 **/
int english_word_cut(char *tree_result_word, char **search_word_point, int *words_weight) {
    int not_chinese_match_offset, not_chinese_match_str_lenght;
    int pecl_regx_match_result = pecl_regx_match((PCRE2_SPTR) (*search_word_point), (PCRE2_SPTR) english_pattern,
                                                 &not_chinese_match_offset,
                                                 &not_chinese_match_str_lenght);

    //匹配到英文先处理英文
    if (pecl_regx_match_result > 0) {
        add_words_weight(words_weight, 2);
        //取出非中文
        char not_chinese_word[not_chinese_match_str_lenght + 1];
        memset(not_chinese_word, 0, not_chinese_match_str_lenght + 1);
        strncpy(not_chinese_word, *search_word_point, not_chinese_match_str_lenght);
        strcat(not_chinese_word, "\0");
        add_word_item_to_single_tree_result(tree_result_word, not_chinese_word);
        *search_word_point =
                *search_word_point + not_chinese_match_str_lenght;

    }
    return 1;
}

//有多少个词条就开多少个线程来模拟分叉树算法，选出词条最多的一个或几个作为分词结果
int _tree_cut_words(struct TREE_CUT_WORDS_DATA *_parent_cut_words_data) {
    //copy一份父级的分词结果到子进程，不要直接用指针指向父进程的分词结果
    struct TREE_CUT_WORDS_DATA tree_cut_words_data;
    init_tree_cut_words_data(&tree_cut_words_data);
    tree_cut_words_data = *_parent_cut_words_data;
    free(_parent_cut_words_data);
    english_word_cut(tree_cut_words_data.cut_word_result, &(tree_cut_words_data.remainder_words),
                     tree_cut_words_data.words_weight);
    //取出第一个汉子，查询redis词条，for循环生产不同的父级分词结果，然后传给新建的子进程,进入递归
    char *first_chinese_word;
    first_chinese_word = (char *) malloc(chinese_char_byte);
    memset(first_chinese_word, 0, chinese_char_byte);
    if (!first_chinese_word) {
        //lazy do.每一个可能的错误都要做处理或日志记录.not only malloc
        printf("Not Enough Memory!/n");
    }

    strncpy(first_chinese_word, tree_cut_words_data.remainder_words, chinese_word_byte);
    strcat(first_chinese_word, "\0");
    char *get_word_item_cmd = join_char(redis_get_common, first_chinese_word);

    //用完释放内存
    free(first_chinese_word);

    redisContext *redis_connect;
    get_redis_connect(&redis_connect);

    redisReply *redis_reply = redisCommand(redis_connect, get_word_item_cmd);
    printf("redis_reply->str is %s\n",redis_reply->str);
    char *match_words_json_str = redis_reply->str;
    free(get_word_item_cmd);
    cJSON *match_words_json = cJSON_Parse(match_words_json_str);



    //判断是否查找到redis词条
    if (redis_reply->type == REDIS_REPLY_STRING) {
        //以后测试下，在for里面值传递，看看有没并发写问题。lazy do
        for (int tree_arg_index = 0; tree_arg_index < cJSON_GetArraySize(match_words_json); tree_arg_index++) {
            cJSON *subitem = cJSON_GetArrayItem(match_words_json, tree_arg_index);
            printf("subitem point is %p\n",subitem);
            //判断词条是否包含在搜索词里,如果在，进入树分词算法
            if (strstr(tree_cut_words_data.remainder_words, subitem->valuestring) != NULL) {
                printf("strstr right\n");
                //这里只能用数组传参数，要不会有并发写问题,以后实际测试一下，应该不用数组也行

                //复制一份分词结果,生出父级分词结果
                struct TREE_CUT_WORDS_DATA * parent_cut_words_data = ( struct TREE_CUT_WORDS_DATA * ) malloc(sizeof(struct TREE_CUT_WORDS_DATA)) ;
                memset(parent_cut_words_data,0,sizeof(struct TREE_CUT_WORDS_DATA));
                init_tree_cut_words_data(parent_cut_words_data);

                //这里复制可能有些问题
                //*parent_cut_words_data = tree_cut_words_data;
                strcpy(parent_cut_words_data->cut_word_result,tree_cut_words_data.cut_word_result);
                strcpy(parent_cut_words_data->remainder_words,tree_cut_words_data.remainder_words);
                parent_cut_words_data->words_weight = tree_cut_words_data.words_weight;

                chinese_word_cut(parent_cut_words_data->cut_word_result, &(parent_cut_words_data->remainder_words),
                                 parent_cut_words_data->words_weight, subitem->valuestring);

                //还有词没分完，进入递归分词
                if( strlen(parent_cut_words_data->remainder_words) > 0 ){
                    pthread_t pthread_id;
                    int pthread_create_result;
                    pthread_create_result = pthread_create(&pthread_id, NULL,
                                                           (void *) _tree_cut_words, parent_cut_words_data);
                    if (pthread_create_result != 0) {
                        //deal the error,lazy do.
                        printf("Create pthread error!\n");
                    }
                }else{
                    printf("result cut_word_result is %s,remainder_words is %s,words_weight is %d;\n",
                           parent_cut_words_data->cut_word_result,parent_cut_words_data->remainder_words,*(parent_cut_words_data->words_weight));
                }
            }

        }
    } else {
        not_match_word_end_cut(tree_cut_words_data.cut_word_result, &(tree_cut_words_data.remainder_words),
                               tree_cut_words_data.words_weight);
        printf("result cut_word_result is %s,remainder_words is %s,words_weight is %d;\n",
               tree_cut_words_data.cut_word_result,tree_cut_words_data.remainder_words,*(tree_cut_words_data.words_weight));
        //printf("%s\n");
    }




    return 0;
}



/*
 * result_code 1:pull word success
 * result_code 0:pull word fail
 * */
int pull_word(char *origin_search_words, char *result_words[], int8_t *result_code) {
    /*不用pthread_join，咱们用管道通信，读到某个值再返回
    pthread_t tree_cut_words_thread_ids[MAX_CUT_WORD_THREAD_NUMBER];
    //总共开了多少个分词线程
    int tree_cut_words_thread_ids_count = 0;
    */
    int tree_cut_words_thread_ids_count = 0;
    struct TREE_CUT_WORDS_DATA tree_cut_words_args[MAX_CUT_WORD_THREAD_NUMBER];

    //定义父级树分词结果
    struct TREE_CUT_WORDS_DATA * parent_tree_cut_words_data = ( struct TREE_CUT_WORDS_DATA * ) malloc(sizeof(struct TREE_CUT_WORDS_DATA)) ;

    init_tree_cut_words_data(parent_tree_cut_words_data);
    strcpy(parent_tree_cut_words_data->remainder_words, "iphoneioioioioioiaaaaa飞行员A9墨镜");

    //新开一个线程进行分词，每一个子分词线程都继承父级线程分词的结果
    int pthread_create_result;
    pthread_t parent_thread_id;
    pthread_create_result = pthread_create(&parent_thread_id, NULL,
                                           (void *) _tree_cut_words, parent_tree_cut_words_data);

    return 0;
};

void handle(int connfd) {

    size_t n;
    int8_t result_code;
    char search_words[MAXLINE];
    memset(search_words, 0, MAXLINE);
    char *result_words[10];
    for (; ;) {
        n = read(connfd, search_words, MAXLINE);


        if (n < 0) {
            if (errno != EINTR) {
                perror("read error");
                break;
            }
        }
        if (n == 0) {
            //connfd is closed by client
            close(connfd);
            printf("client exit\n");
            break;
        }
        //client exit
        if (strncmp("exit", search_words, 4) == 0) {
            close(connfd);
            printf("client exit\n");
            break;
        }

        printf("client input %s\n", search_words);
        pull_word(search_words, result_words, &result_code);

        char respond_word[5] = "ok\n";
        write(connfd, respond_word, n); //write maybe fail,here don't process failed error
    }
}

int main(int argc, char **argv) {

    char * search_words = (char *) malloc(100);
    int8_t result_code;
    memset(search_words,0,100);
    char *result_words[10];
    pull_word(search_words, result_words, &result_code);
    sleep(100);
    return 0;
    int listenfd, connfd;
    int serverPort = 9999;
    int listenq = 1024;
    pid_t childpid;
    char buf[MAXLINE];
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

    printf("echo server startup,listen on port:%d\n", serverPort);
    for (; ;) {
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);
        if (connfd < 0) {
            perror("accept error");
            continue;
        }

        sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
        printf(buf, "");
        childpid = fork();
        if (childpid == 0) { /* child process */
            close(listenfd);    /* close listening socket */
            handle(connfd);   /* process the request */
            exit(0);
        } else if (childpid > 0) {
            close(connfd);          /* parent closes connected socket */
        } else {
            perror("fork error");
        }
    }
}
