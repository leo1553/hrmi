#include "alist.h"

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
alist_t *alist_create(__alist_freer freer, unsigned int initialSize) {
    if(initialSize == 0)
        return NULL;

    alist_t *list = __alist_alloc_list();
    if(list == NULL)
        return NULL;

    list->root = calloc(initialSize, sizeof *list->root);
    if(list->root == NULL)
        return NULL;

    list->freer = freer;
    list->size = initialSize;
    return list;
}

void alist_free(alist_t *list) {
    if(list == NULL)
        return;

    int i;
    for(i = 0; i < list->size; i++) {
    	if(list->root[i] == NULL)
    		break;
    	__alist_free_cell(list, i);
	}
    free(list->root);

    __alist_reset_list(list);
    free(list);
}

/* Insert */
bool alist_add(alist_t *list, void *value) {
    if(list == NULL)
        return false;
    if(!__alist_assert_size(list, list->count + ALIST_SAFETY_SLOTS))
        return false;

    *(list->root + list->count) = value;
    list->count++;
    return true;
}

bool alist_insert(alist_t *list, unsigned int index, void *value) {
    if(list == NULL)
        return false;
    if(!__alist_assert_size(list, list->count + ALIST_SAFETY_SLOTS))
        return false;
       
    /* Insert at the end if index > count
       other implementations may result on different behaviours
       e.g.: returning false or inserting null values until the end. */
    if(index > list->count)
        index = list->count;
    
    __alist_memmove(list, index, +1);
    *(list->root + index) = value;
    list->count++;
    return true;
}

bool alist_remove(alist_t *list, void *value) {
    if(list == NULL)
        return false;

    int i;
    for(i = 0; i < list->count; i++) {
        if(*(list->root + i) == value) {
            __alist_free_cell(list, i);
            __alist_memmove(list, i + 1, -1);
            list->count--;
            *(list->root + list->count) = NULL;
            return true;
        }
    }
    return false;
}

bool alist_remove_at(alist_t *list, unsigned int index) {
    if(list == NULL)
        return false;
    if(index > list->count)
        return false;

    __alist_free_cell(list, index);
    __alist_memmove(list, index + 1, -1);
    list->count--;
    *(list->root + list->count) = NULL;
    return true;
}

bool alist_clear(alist_t *list) {
    if(list == NULL)
        return false;
    
    int i;
    for(i = 0; i < list->count; i++) 
        __alist_free_cell(list, i);
    list->count = 0;
    return true;
}

/* Search */
void *alist_get(alist_t *list, unsigned int index) {
    if(list == NULL)
        return NULL;
    if(index > list->count)
        return NULL;
    return *(list->root + index);
}

int alist_index_of(alist_t *list, void *value) {
    if(list == NULL)
        return -1;

    int i;
    for(i = 0; i < list->count; i++)
        if(*(list->root + i) == value)
            return i;
    return -1;
}

void *alist_find(alist_t *list, __alist_selector selector) {
    if(list == NULL)
        return NULL;
    if(selector == NULL)
        return NULL;

    int i;
    for(i = 0; i < list->count; i++)
        if(selector(*(list->root + i), i))
            return *(list->root + i);
    return NULL;
}

/* Other */
void alist_to_array(alist_t *list, void **array) {
    if(list == NULL)
        return;
    
    int i;
    for(i = 0; i < list->count; i++)
    	array[i] = list->root[i];
}
void **alist_as_array(alist_t *list) {
    if(list == NULL)
        return NULL;
    
    void **result = malloc(sizeof *result * list->count);
    alist_to_array(list, result);
	return result;
}

int alist_count(alist_t *list) {
    if(list == NULL)
        return -1;
    return list->count;
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
alist_t *__alist_alloc_list() {
    alist_t *list = malloc(sizeof *list);
    if(list == NULL)
        return NULL;
    return __alist_reset_list(list);
}

bool __alist_assert_size(alist_t *list, unsigned int size) {
    if(list == NULL)
        return false;
    if(size < 1)
        return false;

    int newSize = list->size;
    while(newSize < size)
        newSize *= 2;
    
    void **newRoot = realloc(list->root, sizeof *list->root * newSize);
    if(newRoot == NULL) 
    	return false;

    /* Assert null values */
    int i;
    for(i = list->size; i < newSize; i++)
        *(newRoot + i) = NULL;

    /* Assign values */
    list->root = newRoot;
    list->size = newSize;
    return true;
}

void __alist_memmove(alist_t *list, unsigned int index, int move) {
    if(list == NULL)
        return;
    if(move == 0)
        return;
    if(index + move < 0)
        return;

    void **cell = list->root + index;
    int len = list->count - index - move + ALIST_SAFETY_SLOTS;
    memmove(cell + move, cell, sizeof *cell * len);
}

/* Reset */
alist_t *__alist_reset_list(alist_t* list) {
    if(list == NULL)
        return NULL;
    list->root = NULL;
    list->count = 0;
    list->size = 0;
    list->freer = NULL;
    return list;
}

/* Free */
void __alist_free_cell(alist_t *list, unsigned int index) {
    if(list == NULL)
        return;
    if(list->freer != NULL)
        list->freer(list->root[index]);
    list->root[index] = NULL;
}

/* Freers */
void __alist_freer_value(void *value) {
    if(value != NULL)
        free(value);
}
