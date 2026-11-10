#include "list.h"

int main(int argc, char* argv[]) {
    printf("Start...\n");    
    remove(FILENAME_HTML);
    node temp;

    list_info lst = {};
    char filename_png[LINE_LEN];
    list_ctor(&lst, 2);
    //list_get_graph(&lst, filename_png);    
    list_insert(&lst, 1, 5);
    //list_get_graph(&lst, filename_png);   
    list_insert(&lst, 2, 6);
    //list_get_graph(&lst, filename_png);    
    list_insert(&lst, 1, 10);
    list_insert(&lst, 3, 10);
    list_insert(&lst, 4, 15);
    list_del(&lst, 1); 
    list_insert(&lst, 4, 12);
    //list_get_graph(&lst, filename_png);    
    list_dtor(&lst);

    printf("...End\n");
    return 0;
}