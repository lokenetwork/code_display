#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
typedef struct _hashmap_element{
    int age;
} hashmap_element;
typedef struct _hashmap_map{
    hashmap_element *data;
} hashmap_map;


int main(){

    hashmap_map * array = (hashmap_map *) malloc(1024);
    for (int index = 0; index < 10; ++index) {
        array->data[index].age = 10;
    }
    printf("end here!\n");
    sleep(100);


}
