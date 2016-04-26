#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void change_number_2(int ** number) {
    *number = 40;
}

int main() {
    int main_number_2 = 45;
    printf("%d\n", main_number_2);
    int * main_number_2_point = &main_number_2;
    change_number_2(main_number_2_point);
    //为什么用&main_number_2_point不成功
    //change_number_2(&main_number_2_point);
    printf("%d\n", main_number_2);
    return (0);
}
