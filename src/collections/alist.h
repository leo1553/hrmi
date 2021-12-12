#ifndef __INCLUDE_ALIST_H
#define __INCLUDE_ALIST_H

#include "../includes.h"
#include "../common/platformhelper.h"

#define ALIST_SAFETY_SLOTS 3

/*
    TODO:
    - Observer callbacks
    - _all methods (methods that interacts with all nodes on the list)
        - remove_all(list, comparer)
        - get_all(list, comparer)
*/

typedef void (*__alist_freer)(void *value);
typedef bool (*__alist_selector)(const void *value, const int index);

typedef struct __alist_root alist_t;
struct __alist_root {
    void **root;
    unsigned int count;
    unsigned int size;

    __alist_freer freer;
};

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
alist_t *alist_create(__alist_freer freer, unsigned int initialSize);
void alist_free(alist_t* list);

/* Insert */
bool alist_add(alist_t *list, void *value);
bool alist_insert(alist_t *list, unsigned int index, void *value);

/* Remove */
bool alist_remove(alist_t *list, void *value);
bool alist_remove_at(alist_t *list, unsigned int index);
bool alist_clear(alist_t *list);

/* Search */
void *alist_get(alist_t *list, unsigned int index);
int alist_index_of(alist_t *list, void *value);
void *alist_find(alist_t *list, __alist_selector selector);

/* Other */
void alist_to_array(alist_t *list, void **array);
void **alist_as_array(alist_t *list);
int alist_count(alist_t *list);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
alist_t *__alist_alloc_list();
bool __alist_assert_size(alist_t *list, unsigned int size);
void __alist_memmove(alist_t *list, unsigned int index, int move);

/* Reset */
alist_t *__alist_reset_list(alist_t* list);

/* Free */
void __alist_free_cell(alist_t *list, unsigned int index);

/* Freers */
void __alist_freer_value(void *value);

#endif
