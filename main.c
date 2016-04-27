/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>

//分词结果的最大长度
#define WORD_RESULT_LEN 200
//可处理的最大的客户端发送的搜索词长度
#define MAX_SEARCH_WORDS_LEN 200

//线程分词的结果结构体，不是最终的结果。
struct TREE_CUT_WORDS_DATA {
    //父级的分词结果
    char *cut_word_result;

    //父级还没处理，剩余的搜索词
    char *remainder_words;

    //父级的分词权重
    int *words_weight;

};

//初始化分词结果结构体
void init_tree_cut_words_data(struct TREE_CUT_WORDS_DATA *cut_words_data) {
    (*cut_words_data).cut_word_result = (char *) malloc(WORD_RESULT_LEN);
    memset((*cut_words_data).cut_word_result, 0, WORD_RESULT_LEN);
    (*cut_words_data).words_weight = (int *) malloc(sizeof(int));
    memset((*cut_words_data).words_weight, 0, sizeof(int));
    (*cut_words_data).remainder_words = (char *) malloc(MAX_SEARCH_WORDS_LEN);
    memset((*cut_words_data).remainder_words, 0, MAX_SEARCH_WORDS_LEN);
}


int _tree_cut_words(struct TREE_CUT_WORDS_DATA *parent_cut_words_data) {
    //copy一份父级的分词结果到子进程，不要直接用指针指向父进程的分词结果
    struct TREE_CUT_WORDS_DATA tree_cut_words_data;
    init_tree_cut_words_data(&tree_cut_words_data);

    char copy_type[20];
    memset(copy_type,0,20);
    strcpy(copy_type,"struc对拷贝");
    //struc对拷贝是最优雅的写法
    if( strcmp(copy_type,"memcpy拷贝") ){
        printf("memcpy拷贝 starting\n");
        memcpy(&tree_cut_words_data,parent_cut_words_data,sizeof(tree_cut_words_data));
    }else if( strcmp(copy_type,"一个一个值拷贝") ){
        printf("一个一个值拷贝 starting\n");
        strcpy(tree_cut_words_data.cut_word_result, (*parent_cut_words_data).cut_word_result);
        strcpy(tree_cut_words_data.remainder_words, (*parent_cut_words_data).remainder_words);
        *(tree_cut_words_data.words_weight) = *((*parent_cut_words_data).words_weight);
    }else if( strcmp(copy_type,"struc对拷贝") ){
        tree_cut_words_data = *parent_cut_words_data;
    }



    printf("tree_cut_words_data cut_word_result is %s, remainder_words is %s,words_weight is %d\n",
           tree_cut_words_data.cut_word_result, tree_cut_words_data.remainder_words,
           *tree_cut_words_data.words_weight);

    //sleep 2秒，等待父线程把数据改了，看赋值有没影响
    sleep(2);
    printf("tree_cut_words_data cut_word_result is %s, remainder_words is %s,words_weight is %d\n",
           tree_cut_words_data.cut_word_result, tree_cut_words_data.remainder_words,
           *tree_cut_words_data.words_weight);

    return 0;
}

int main(void) {
    //定义父级树分词结果
    struct TREE_CUT_WORDS_DATA parent_tree_cut_words_data;
    init_tree_cut_words_data(&parent_tree_cut_words_data);
    strcpy(parent_tree_cut_words_data.cut_word_result, "iphone6s");
    strcpy(parent_tree_cut_words_data.remainder_words, "飞行员墨镜");
    *(parent_tree_cut_words_data.words_weight) = 10;

    int pthread_create_result;
    pthread_t parent_thread_id;
    pthread_create_result = pthread_create(&parent_thread_id, NULL,
                                           (void *) _tree_cut_words, &parent_tree_cut_words_data);

    //父线程把数据改了,看子线程赋值有没影响
    init_tree_cut_words_data(&parent_tree_cut_words_data);
    strcpy(parent_tree_cut_words_data.cut_word_result, "xiaomi");
    strcpy(parent_tree_cut_words_data.remainder_words, "好用切丝器");

    sleep(20);
    return (0);
}
