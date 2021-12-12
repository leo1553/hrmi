#ifndef __INCLUDE_HASHTABLE_H
#define __INCLUDE_HASHTABLE_H

/*
	HashTable baseada no System.Collections.Generic.Dictionary do C#.
	Fonte: https://referencesource.microsoft.com/#mscorlib/system/collections/generic/dictionary.cs
*/

#include "../common/platformhelper.h"
#include "../common/utils.h"
#include "../includes.h"

typedef int (*__hashtable_hash)(const void *key);
typedef void (*__hastable_freer)(void *key, void *value);
typedef int (*__hashtable_comparer)(const void *a, const void *b);
typedef bool (*__hashtable_selector)(const void *key, const void *value);

typedef struct __hashtable_entry htentry_t;
struct __hashtable_entry {
    int hash_code;
    int next;
    void *key;
    void *value;
};

typedef struct __hastable_root hashtable_t;
struct __hastable_root {
    htentry_t *entries;
    int *buckets;

    unsigned int size;
    unsigned int count;

    int __free_list;
    int __free_count;

    __hashtable_hash hash;
    __hashtable_comparer key_comparer;
    __hastable_freer freer;
};

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
hashtable_t *hashtable_create(__hashtable_hash hash, __hashtable_comparer key_comparer, __hastable_freer freer, unsigned int capacity);
void hashtable_free(hashtable_t *hashtable);

/* Insert */
bool __hashtable_insert(hashtable_t *hashtable, void *key, void *value, bool is_add, bool free_past_key, bool free_past_value);
bool hashtable_add(hashtable_t *hashtable, void *key, void *value);
bool hashtable_set(hashtable_t *hashtable, void *key, void *value, bool free_past_key, bool free_past_value);

/* Remove */
bool hashtable_remove(hashtable_t *hashtable, void *key);
void hashtable_clear(hashtable_t *hashtable);

/* Search */
int __hashtable_find_entry(hashtable_t *hashtable, void *key);
void *hashtable_get(hashtable_t *hashtable, void *key);
void *hashtable_find(hashtable_t *hashtable, void *key, __hashtable_selector selector);

/* Other */
int hashtable_count(hashtable_t *hashtable);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
hashtable_t *__hashtable_alloc_hashtable();
bool __hashtable_resize(hashtable_t *hashtable, unsigned int size);

/* Reset */
hashtable_t *__hashtable_reset_hashtable(hashtable_t *hashtable);
void __hashtable_reset_entry(htentry_t *entry);

/* Comparers */
bool __hashtable_compare_keys(hashtable_t *hashtable, void *key1, void *key2, int hash1, int hash2);
int __hashtable_comparer_int(const void *a, const void *b);
int __hashtable_comparer_str(const void *a, const void *b);
int __hashtable_comparer_str_i(const void *a, const void *b);

/* Freers */
void __hashtable_freer_key(void *key, void *value);
void __hashtable_freer_value(void *key, void *value);
void __hashtable_freer_both(void *key, void *value);

#endif
