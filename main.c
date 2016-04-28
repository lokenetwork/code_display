/* example.c*/
#include <printf.h>
#include <sys/param.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
//父线程传参給子线程的结构体
struct TREE_CUT_WORDS_DATA {
    char *cut_word_result;
};
void thread(struct TREE_CUT_WORDS_DATA *parent_cut_words_data) {
    //原来free掉结构体指针并不会free掉结构体里面的char *指针元素
    //free(parent_cut_words_data->cut_word_result);
    free(parent_cut_words_data);
}

int main(void) {


    struct TREE_CUT_WORDS_DATA * parent_cut_words_data = (struct TREE_CUT_WORDS_DATA *) malloc(sizeof(struct TREE_CUT_WORDS_DATA));
    memset(parent_cut_words_data,0,sizeof(struct TREE_CUT_WORDS_DATA));
    parent_cut_words_data->cut_word_result = (char *) malloc(10);
    memset(parent_cut_words_data->cut_word_result,0,10);
    char * name = (char *) malloc(10);
    memset(name,0,10);
    strcpy(name,"loken");
    strcpy(parent_cut_words_data->cut_word_result,name);
    char * back_name = parent_cut_words_data->cut_word_result;
    printf("back_name point is %p\n",back_name);
    printf("parent_cut_words_data->cut_word_result point is %p\n",parent_cut_words_data->cut_word_result);


    pthread_t thread_id_1;
    int ret = pthread_create(&thread_id_1, NULL, (void *) thread, parent_cut_words_data);
    if (ret != 0) {
        printf("pthread_create error.\n");
    }
    sleep(2);

    printf("back_name is %s\n",back_name);

    printf("This is the main .\n");
    return (0);
}
