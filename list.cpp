#include "list.h"

#define RETURN_ERROR(err_code) do {                                               \
    printf("%s:%d error (%d) %s\n", __FILE__,  __LINE__, err_code, __FUNCTION__); \
    return err_code;                                                              \
} while (0)

#ifdef LIST_DEBUG
    #define LIST_ASSERT(lst_ptr) lst_assert((lst_ptr), __FILE__, __FUNCTION__, __LINE__)
#else
    #define LIST_ASSERT(lst_ptr)  
#endif

const node poison_node = {.prev_mass = 123456789, .value = 123456789, .next_mass = 123456789};

void lst_assert(list_info* const lst, const char* file_name, const char* func_name, const int line);
LIST_MESS lst_verificate(list_info* const lst, const char* file_name, const char* func_name, const int line);
void lst_dump(list_info* const lst, const LIST_MESS result);

LIST_MESS get_free(list_info* const lst);
LIST_MESS fill_poison(list_info* list, size_t id_start);
LIST_MESS get_ind_mass(list_info* lst, size_t ind_lst, size_t* ind_mass_prev, size_t* ind_mass_next);

// assert списка
void lst_assert(list_info* const lst, const char* file_name, const char* func_name, const int line) {
    LIST_MESS result = lst_verificate(lst, file_name, func_name, line);
    lst_dump(lst, result);
    //assert(result == LIST_MESS_OK);
}


// проверка списка
LIST_MESS lst_verificate(list_info* const lst, const char* file_name, const char* func_name, const int line) {
    if (lst == NULL) RETURN_ERROR(LIST_MESS_NULLPTR);
    else {
        lst->error.file = file_name;
        lst->error.func = func_name;
        lst->error.line = line;
    }

    size_t ind_mass_next = 0;
    for (size_t i = 0; i < lst->num_elems; i++) {
        if (ind_mass_next >= lst->capacity) RETURN_ERROR(LIST_MESS_ERROR_LEN);
        ind_mass_next = lst->buffer[ind_mass_next].next_mass;
    }
    if (ind_mass_next != 0) RETURN_ERROR(LIST_MESS_ERROR_LEN);

    return LIST_MESS_OK;
}


// вывод информации в файл
void lst_dump(list_info* const lst, const LIST_MESS result)
{
    char filename_html[LINE_LEN] = FILENAME_HTML;
    char filename_png[LINE_LEN];
    
    FILE* log_html = fopen(filename_html, "a+");

    fprintf(log_html, "<pre>\n");
    fprintf (log_html, "======================================\n");
    if (result == LIST_MESS_OK)
    {
        fprintf (log_html, "            VERIFICATE OK\n");       
    }
    else
    {
        fprintf (log_html, "            VERIFICATE ERROR\n");
    }
    fprintf (log_html, "======================================\n");
    fprintf (log_html, "            INFORMATION:\n");
    fprintf (log_html, "File name:     %s\n", lst->error.file);
    fprintf (log_html, "Function name: %s\n", lst->error.func);
    fprintf (log_html, "Line number:   %d\n", lst->error.line);
    fprintf (log_html, "---------------------------------------\n");
    fprintf (log_html, "    num_elems: %zu\n", lst->num_elems);
    fprintf (log_html, "    capacity:  %zu\n", lst->capacity);
    fprintf (log_html, "    buffer:  %p\n", lst->buffer);
    fprintf (log_html, "---------------------------------------\n");

    list_get_graph(lst, filename_png);
    fprintf (log_html, "<img src=\"%s\" alt=\"%s\">\n", filename_png, filename_png);

    fprintf (log_html, "======================================\n\n\n");
    fprintf(log_html, "</pre>");
    fclose(log_html);
    return;
}


// создание списка
LIST_MESS list_ctor(list_info* list, const size_t capacity) {
    system("del /Q Graphviz\\*.gv");
    system("del /Q Graphviz\\*.svg");

    list->capacity = capacity;
    list->buffer   = (node*)malloc(list->capacity * sizeof(node));
    if (list->buffer == NULL) RETURN_ERROR(LIST_MESS_MEM_ALLOC);
    fill_poison(list, 0);
    list->buffer[0].value = 0;
    list->buffer[0].prev_mass = 0;
    list->buffer[0].next_mass = 0;
    list->num_elems = 1;
    list->ind_mass_free  = 1;

    LIST_ASSERT(list);

    return LIST_MESS_OK;
}

// заполнение ядовитыми значениями
LIST_MESS fill_poison(list_info* list, size_t id_start) {
    for (size_t i = id_start; i < list->capacity; i++) {
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

// удаление элемента
LIST_MESS list_del(list_info* lst, size_t ind_lst) {
    assert(lst != NULL);
    size_t ind_mass_prev;
    size_t ind_mass;
    if (ind_lst > 0) {
        if (ind_lst > lst->num_elems) RETURN_ERROR(LIST_MESS_INCORR_IND);

        get_ind_mass(lst, ind_lst, &ind_mass_prev, &ind_mass);
        // корректировка предыдущего элемента
        lst->buffer[lst->buffer[ind_mass].prev_mass].next_mass = lst->buffer[ind_mass].next_mass;
        // корректировка следующего элемента
        lst->buffer[lst->buffer[ind_mass].next_mass].prev_mass = lst->buffer[ind_mass].prev_mass;
        // удаление текущего элемента
        lst->buffer[ind_mass] = poison_node;

        //поиск первого свободного элемента
        get_free(lst);
        lst->num_elems -= 1;
    }

    LIST_ASSERT(lst);

    return LIST_MESS_OK;
}


// вставка элемента
LIST_MESS list_insert(list_info* lst, size_t ind_lst, node_elem_t val) {
    assert(lst != NULL);
    LIST_MESS res;
    size_t ind_mass_prev;
    size_t ind_mass_next;
    
    if (lst->num_elems == 1) {
        lst->buffer[lst->ind_mass_free].value = val;
        lst->buffer[lst->ind_mass_free].next_mass = 0;
        lst->buffer[lst->ind_mass_free].prev_mass = 0;
        lst->buffer[0].prev_mass = lst->ind_mass_free;
        lst->buffer[0].next_mass = lst->ind_mass_free;
        lst->ind_mass_free = 2;
    } else {
        if (ind_lst > lst->num_elems + 1) ind_lst = lst->num_elems + 1;
        res = get_ind_mass(lst, ind_lst, &ind_mass_prev, &ind_mass_next);
        if (res != LIST_MESS_OK) RETURN_ERROR(res);

        // вставка элемента
        lst->buffer[lst->ind_mass_free].prev_mass = ind_mass_prev;
        lst->buffer[lst->ind_mass_free].value = val;
        lst->buffer[lst->ind_mass_free].next_mass = ind_mass_next;
        // изменение предыдущего элемента
        lst->buffer[ind_mass_prev].next_mass = lst->ind_mass_free;
        // формирование некорректного узла
        /*
        if (ind_lst == 4) {
            lst->buffer[ind_mass_prev].next_mass = 90;
        }
        */
        // изменение следующего элемента
        lst->buffer[ind_mass_next].prev_mass = lst->ind_mass_free;
    }


    //поиск первого свободного элемента
    lst->num_elems += 1;
    get_free(lst);

    LIST_ASSERT(lst);

    return LIST_MESS_OK;
}

// поиск индекса нового свободного элемента в массиве
LIST_MESS get_free(list_info* const lst) {
    lst->ind_mass_free = poison_node.prev_mass;
    if (lst->capacity == lst->num_elems) {
        node* temp = (node*)realloc(lst->buffer, 2 * lst->capacity * sizeof(node));
        if (temp == NULL) RETURN_ERROR(LIST_MESS_MEM_ALLOC);
        lst->buffer = temp;
        lst->capacity = 2 * lst->capacity;
        lst->ind_mass_free = lst->num_elems;
        fill_poison(lst, lst->ind_mass_free);
    } else {
        for (size_t i = 1; i < lst->capacity; i++) {
            if (lst->buffer[i].value == poison_node.value) {
                lst->ind_mass_free = i;
                break;
            }
        }
    }

    if (lst->ind_mass_free == poison_node.prev_mass) RETURN_ERROR(LIST_MESS_INCORR_IND);

    return LIST_MESS_OK;
}


LIST_MESS get_ind_mass(list_info* lst, size_t ind_lst, size_t* ind_mass_prev, size_t* ind_mass_next) {
    assert(lst != NULL);
    size_t ind_mass = 0;
    size_t ind_lst_temp = 0;
    while (ind_lst_temp < ind_lst) {
        if (ind_mass > lst->capacity) RETURN_ERROR(LIST_MESS_ERROR_LEN);
        ind_mass = lst->buffer[ind_mass].next_mass;
        ind_lst_temp++;
    }

    if (ind_mass > lst->capacity) RETURN_ERROR(LIST_MESS_ERROR_LEN);
    *ind_mass_prev = lst->buffer[ind_mass].prev_mass;
    *ind_mass_next = ind_mass;

    return LIST_MESS_OK;
}

LIST_MESS list_get_graph(list_info* lst, char* filename_png) {
    FILE* file_gv;
    char filename_gv[LINE_LEN] = "./Graphviz/";
    struct timespec time_now;
    struct tm time_info;
    char time_str[LINE_LEN];

    clock_gettime(CLOCK_REALTIME, &time_now);
    localtime_s(&time_info, &time_now.tv_sec);
    size_t res = strftime(time_str, LINE_LEN, "%Y%m%d_%H%M%S", &time_info);
    if (res == 0) RETURN_ERROR(LIST_MESS_GET_TIME);

    char sec[LINE_LEN] = {};
    char cmd[LINE_LEN] = {};

    sprintf(sec, "_%05ld", time_now.tv_nsec/10000);
    strcat_s(time_str, sec);


    strcat_s(filename_gv, time_str);
    strcat_s(filename_gv, ".gv");

    strcpy_s(filename_png, LINE_LEN, "./Graphviz/");
    strcat_s(filename_png, LINE_LEN, time_str);
    strcat_s(filename_png, LINE_LEN, ".svg");

    mkdir("Graphviz");
    // построение описание графа для graphviz
    fopen_s(&file_gv, filename_gv, "w");
    fprintf(file_gv, "digraph G {\n");
    fprintf(file_gv, "rankdir = \"LR\"\n");
    fprintf(file_gv, "node [shape=Mrecord, style=filled, fillcolor = \"#77C3D9\", fontname=\"Arial\", fontsize=10];\n");
    fprintf(file_gv, "edge [color = \"#A6340D\", weight = 2];\n");

    char color[LINE_LEN] = {};
    // узлы массива
    for (size_t i = 0; i < lst->capacity; i++) {
        if ((i == lst->buffer[0].next_mass) && (i != 0)) {
            strcpy_s(color, "#77E0AE");
            fprintf(file_gv, "{rank = same;\n");
            fprintf(file_gv, "  node_head [shape = \"invhouse\", label = \"HEAD\", fillcolor=\"%s\"];\n", 
                color);
            fprintf(file_gv, "  node%zu [label = \"Index = %zu | Value = %d | {Prev = %zu | Next = %zu}\", fillcolor=\"%s\"];\n",
                i, 
                i, 
                lst->buffer[i].value, 
                lst->buffer[i].prev_mass,
                lst->buffer[i].next_mass,
                color);
            fprintf(file_gv, "}\n");
        } else if ((i == lst->buffer[0].prev_mass) && (i != 0)) {
            strcpy_s(color, "#FFC488");
            fprintf(file_gv, "{rank = same;\n");
            fprintf(file_gv, "  node_tail [shape = \"invhouse\", label = \"TAIL\", fillcolor=\"%s\"];\n", 
                color);
            fprintf(file_gv, "  node%zu [label = \"Index = %zu | Value = %d | {Prev = %zu | Next = %zu}\", fillcolor=\"%s\"];\n",
                i, 
                i, 
                lst->buffer[i].value, 
                lst->buffer[i].prev_mass,
                lst->buffer[i].next_mass,
                color);
            fprintf(file_gv, "}\n");
        } else if (i == lst->ind_mass_free) {
            strcpy_s(color, "#FFA688");
            fprintf(file_gv, "{rank = same;\n");
            fprintf(file_gv, "  node_firstfree [shape = \"invhouse\", label = \"FIRST FREE\", fillcolor=\"%s\"];\n", 
                color);
            fprintf(file_gv, "  node%zu [label = \"Index = %zu | Value = %d | {Prev = %zu | Next = %zu}\", fillcolor=\"%s\"];\n",
                i, 
                i, 
                lst->buffer[i].value, 
                lst->buffer[i].prev_mass,
                lst->buffer[i].next_mass,
                color);
            fprintf(file_gv, "}\n");
        } else {
            if (i == 0) strcpy_s(color, "#A8A8A8");
            else if (lst->buffer[i].value == poison_node.value) strcpy_s(color, "#D7D7D7");
            else strcpy_s(color, "#77C3D9");
            fprintf(file_gv, "node%zu [label = \"Index = %zu | Value = %d | {Prev = %zu | Next = %zu}\", fillcolor=\"%s\"];\n",
                i, 
                i, 
                lst->buffer[i].value, 
                lst->buffer[i].prev_mass,
                lst->buffer[i].next_mass,
                color);
        }
    }

    // связи списка
    size_t ind_lst_temp = 0;
    size_t ind_mass_temp = 0;
    size_t ind_prev;
    int weight;
    while (ind_lst_temp < (*lst).num_elems) {
        if ((*lst).buffer[ind_mass_temp].next_mass == 0) weight = 1;
        else weight = 2;
        fprintf(file_gv, "node%zu -> node%zu [weight = %d, dir = \"both\"];\n", 
            ind_mass_temp, 
            lst->buffer[ind_mass_temp].next_mass,
            weight);
        ind_prev = ind_mass_temp;
        ind_mass_temp = (*lst).buffer[ind_mass_temp].next_mass;
        ind_lst_temp++;
    }

    // связи свободных узлов
    for (size_t i = lst->ind_mass_free; i < lst->capacity; i++) {
        if (lst->buffer[i].value == poison_node.value) {
            if (i == lst->ind_mass_free) {
                fprintf(file_gv, "node%zu -> node%zu [style = invis];\n",
                ind_prev, 
                i);
                ind_prev = i;
            } else {
                fprintf(file_gv, "node%zu -> node%zu;\n", 
                ind_prev, 
                i);
                ind_prev = i;
            }
        }
    }

    fprintf(file_gv, "}\n");
    fclose(file_gv);

    // преобразование описания графа в картинку
    sprintf(cmd, "dot -Tsvg %s -o %s", filename_gv, filename_png);
    system(cmd);

    return LIST_MESS_OK;
} 
