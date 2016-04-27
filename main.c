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

//父线程传参給子线程的结构体
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

      //这样copy一个结构体里面的值太慢了
    strcpy(tree_cut_words_data.cut_word_result, (*parent_cut_words_data).cut_word_result);
    strcpy(tree_cut_words_data.remainder_words, (*parent_cut_words_data).remainder_words);
    *(tree_cut_words_data.words_weight) = *((*parent_cut_words_data).words_weight);



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
    strcpy(parent_tree_cut_words_data.remainder_words, "华为A9");
    *(parent_tree_cut_words_data.words_weight) = 10;
    _tree_cut_words(&parent_tree_cut_words_data);
    return (0);
}
