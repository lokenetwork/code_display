#include <malloc.h>
#include <string.h>

int main() {
    char * name = (char *) malloc(100);
    char * name_point_can_free = name;
    memset(name,0,100);
    printf("name point is %p,value is %s\n",name,name);
    strcpy(name,"loken");
    printf("name point is %p,value is %s\n",name,name);
    name++;
    printf("name point is %p,value is %s\n",name,name);

    //name貌似已经做了偏移运算，不能free掉
    free(name);
    //name_point_can_free可以正常free
    //free(name_point_can_free);
    return 0;
}
