#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum LIST_MESS {
    LIST_MESS_OK = 0,

    LIST_MESS_MEM_ALLOC = 10
};

typedef int node_elem_t;

// один элемент
struct node {
    size_t prev_mass;        // индекс предыдущего элемента
    node_elem_t value;  // значение текущего элемента
    size_t next_mass;        // индекс следующего элемента
};

struct list_info {
    size_t num_elems;       // количество элементов в списке 
    size_t capacity;        // количество мест, выделенных для списка
    node* buffer;           // указатель на начало выделенной для массива памяти
    size_t ind_mass_free;   // указатель на индекс первой свободной ячейки массива
};


LIST_MESS list_ctor(list_info* list, const size_t capacity);
LIST_MESS list_dtor(list_info* list);
LIST_MESS list_insert(list_info* lst, size_t ind_lst, node_elem_t val);

#endif // LIST_H