#include "list.h"

int main() {
    printf("Start...\n");    
    remove(FILENAME_HTML);

    list_info lst = {};
    list_ctor(&lst, 2);
    //list_get_graph(&lst, filename_png);    
    list_insert(&lst, 1, 5);
    //list_get_graph(&lst, filename_png);   
    list_insert(&lst, 2, 6);
    //list_get_graph(&lst, filename_png);    
    list_insert(&lst, 3, 10);
    list_insert(&lst, 4, 15);
    list_insert(&lst, 5, 12);
    //list_get_graph(&lst, filename_png);    
    list_dtor(&lst);

    printf("...End\n");
    return 0;
}