#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include "cJSON.h"

int main(void) {
    struct person {
        char name[22];
        char * address;
    }; 

    struct person * me =  (struct person * ) malloc(sizeof(struct person));
    me->address = (char * ) malloc(100);
    char * address_point = me->address;
    strcpy(me->address,"Guangdong");
    printf("me.address point is %p\n",me->address);
    printf("address_point is %p\n",address_point);
    strcpy(me->name,"loken");
    char * name_point = me->name;
    printf("name_point point is %p\n",name_point);
    printf("me.name point is %p\n",me->name);
    free(me);
    printf("address is %s\n",address_point);
    printf("name_point is %s\n",name_point);


    return 0;
}
