#include "list.h"

int main(int argc, char* argv[]) {
    printf("Start...\n");    
    
    node temp;

    list_info lst = {};
    list_ctor(&lst, 10);    
    temp = lst.buffer[0];
    list_insert(&lst, 1, 5);
    temp = lst.buffer[1];
    list_insert(&lst, 2, 6);
    temp = lst.buffer[2];

    list_insert(&lst, 1, 10);
    temp = lst.buffer[0];
    temp = lst.buffer[1];
    temp = lst.buffer[2];
    temp = lst.buffer[3];

    list_dtor(&lst);

    printf("...End\n");
    return 0;
}