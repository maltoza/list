#include "list.h"

#define RETURN_ERROR(err_code) do {                                               \
    printf("%s error (%d) %s:%d\n", __FILE__, err_code, __FUNCTION__,  __LINE__); \
    return err_code;                                                              \
} while (0)

const node poison_node = {.prev_mass = 123456789, .value = 123456789, .next_mass = 123456789};

LIST_MESS fill_poison(list_info* list);
LIST_MESS get_ind_mass(list_info* lst, size_t ind_lst, size_t* ind_mass_prev, size_t* ind_mass_next);

// создание списка
LIST_MESS list_ctor(list_info* list, const size_t capacity) {
    list->capacity = capacity;
    list->buffer   = (node*)malloc((list->capacity + 1) * sizeof(node));
    if (list->buffer == NULL) RETURN_ERROR(LIST_MESS_MEM_ALLOC);
    fill_poison(list);
    list->num_elems = 0;
    list->ind_mass_free  = 1;
    list->buffer[0].value = 0;

    return LIST_MESS_OK;
}

// заполнение ядовитыми значениями
LIST_MESS fill_poison(list_info* list) {
    for (int i = 0; i <= list->capacity; i++) {
        list->buffer[i] = poison_node;
    }

    return LIST_MESS_OK;
}

// уничтожение списка
LIST_MESS list_dtor(list_info* list) {
    list->ind_mass_free  = 0;
    list->num_elems = 0;
    free(list->buffer);
    list->capacity  = 0;
    list->buffer = NULL;

    return LIST_MESS_OK;
}

// вставка элемента
LIST_MESS list_insert(list_info* lst, size_t ind_lst, node_elem_t val) {
    assert(lst != NULL);
    if (lst->capacity == lst->num_elems) {
        node* temp = (node*)realloc(lst->buffer, 2 * lst->capacity * sizeof(node));
        if (temp == NULL) RETURN_ERROR(LIST_MESS_MEM_ALLOC);
        lst->buffer = temp;
        lst->capacity = 2 * lst->capacity;
        lst->ind_mass_free = lst->num_elems;
    }

    if (lst->num_elems == 0) {
        lst->buffer[lst->ind_mass_free].value = val;
        lst->buffer[lst->ind_mass_free].next_mass = 0;
        lst->buffer[lst->ind_mass_free].prev_mass = 0;
        lst->buffer[0].prev_mass = lst->ind_mass_free;
        lst->buffer[0].next_mass = lst->ind_mass_free;
        lst->ind_mass_free = 2;
    } else {
        size_t ind_mass_prev;
        size_t ind_mass_next;
        get_ind_mass(lst, ind_lst, &ind_mass_prev, &ind_mass_next);

        // вставка элемента
        lst->buffer[lst->ind_mass_free].prev_mass = ind_mass_prev;
        lst->buffer[lst->ind_mass_free].value = val;
        lst->buffer[lst->ind_mass_free].next_mass = ind_mass_next;
        // изменение предыдущего элемента
        lst->buffer[ind_mass_prev].next_mass = lst->ind_mass_free;
        // изменение следующего элемента
        lst->buffer[ind_mass_next].prev_mass = lst->ind_mass_free;

    }

    // получение индекса нового свободного элемента в массиве
    size_t ind_mass = 0;
    while (lst->buffer[ind_mass].value != poison_node.value) {
        ind_mass++;
    }
    lst->ind_mass_free = ind_mass;
    lst->num_elems += 1;

    return LIST_MESS_OK;
}

LIST_MESS get_ind_mass(list_info* lst, size_t ind_lst, size_t* ind_mass_prev, size_t* ind_mass_next) {
    assert(lst != NULL);
    size_t ind_mass = 0;
    size_t ind_lst_temp = 0;
    while (ind_lst_temp < ind_lst) {
        ind_mass = lst->buffer[ind_mass].next_mass;
        ind_lst_temp++;
    }

    *ind_mass_prev = lst->buffer[ind_mass].prev_mass;
    *ind_mass_next = ind_mass;

    return LIST_MESS_OK;
}

