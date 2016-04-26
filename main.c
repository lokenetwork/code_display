#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void change_number_1(int * number){
    *number = 10;
}

void change_number_2(int ** number){
    *number = 10;
}

int main() {
    int main_number = 5;
    change_number_1(&main_number);
    printf("%d\n",main_number);

    int main_number_2 = 45;
    change_number_2(&main_number_2);
    printf("%d\n",main_number_2);



    return (0);
}
