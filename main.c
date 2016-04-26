#include <stdio.h>
#include <string.h>

/*
char get_name(){

}
 */
int get_age(int type) {
    int age;
    if (type == 1) {
        age = 35;
    } else {
        age = 100;
    }
    return age;
}
char *getStackPoint(int type) {
    char name[1024];
    if( type == 1 ){
        strcpy(name,"loken");
    }else{
        strcpy(name,"nahai");
    }
    printf("name is %s; p is %p \n", &name, &name);
    printf("name is %s; p is %p \n", name, name);
    return &name;
}

int main(void) {
    int main_age;
    printf("main_age point is %p\n", &main_age);
    main_age = get_age(1);
    printf("main_age point is %p\n", &main_age);

    printf("main_age is %d\n", main_age);
    int main_age_2 = get_age(2);
    printf("main_age is %d\n", main_age);



    char * main_name = getStackPoint(1);
    printf("main_name is %s; p is %p \n", main_name, main_name);
    int i;
    for (i = 0; i < 2; i++) {
        getStackPoint(2);
    };
    printf("main_name is %s; p is %p \n", main_name, main_name);

    return 0;
}
