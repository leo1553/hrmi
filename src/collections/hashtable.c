#include "hashtable.h"

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
hashtable_t *hashtable_create(__hashtable_hash hash, __hashtable_comparer key_comparer, __hastable_freer freer, unsigned int capacity) {
    if (hash == NULL)
        return NULL;
    if (key_comparer == NULL)
        return NULL;

    hashtable_t *hashtable = __hashtable_alloc_hashtable();
    if (hashtable == NULL)
        return NULL;

    int size = get_prime(capacity);
    hashtable->entries = malloc(sizeof *hashtable->entries * size);
    hashtable->buckets = malloc(sizeof *hashtable->buckets * size);
    hashtable->size = size;
    hashtable->hash = hash;
    hashtable->key_comparer = key_comparer;
    hashtable->freer = freer;

    int i;
    for (i = 0; i < size; i++) {
        hashtable->buckets[i] = -1;
        __hashtable_reset_entry(&hashtable->entries[i]);
    }

    return hashtable;
}

void hashtable_free(hashtable_t *hashtable) {
    if (hashtable == NULL)
        return;

    int i;
    for (i = 0; i < hashtable->size; i++) {
        if (hashtable->freer != NULL)
            hashtable->freer(hashtable->entries[i].key, hashtable->entries[i].value);

        __hashtable_reset_entry(&hashtable->entries[i]);
    }

    free(hashtable->entries);
    free(hashtable->buckets);

    __hashtable_reset_hashtable(hashtable);
    free(hashtable);
}

/* Insert */
bool __hashtable_insert(hashtable_t *hashtable, void *key, void *value, bool is_add, bool free_past_key, bool free_past_value) {
    if (hashtable == NULL)
        return false;
    if (key == NULL)
        return false;

    int i;

    int hash_code = hashtable->hash(key) & 0x7FFFFFFF;
    int target_bucket = hash_code % hashtable->size;

    /* Alterar se ja existir */
    for (i = hashtable->buckets[target_bucket]; i >= 0; i = hashtable->entries[i].next) {
        if (__hashtable_compare_keys(hashtable, hashtable->entries[i].key, key, hashtable->entries[i].hash_code, hash_code)) {
            if (is_add)
                return false;
            if (free_past_key && hashtable->freer != NULL)
                hashtable->freer(hashtable->entries[i].key, NULL);
            if (free_past_value && hashtable->freer != NULL)
                hashtable->freer(NULL, hashtable->entries[i].value);
            hashtable->entries[i].key = key;
            hashtable->entries[i].value = value;
            return true;
        }
    }

    /* Inserir caso nao exista */
    int index;
    if (hashtable->__free_count > 0) {
        index = hashtable->__free_list;
        hashtable->__free_list = hashtable->entries[index].next;
        hashtable->__free_count--;
    } else {
        if (hashtable->count == hashtable->size) {
            if (!__hashtable_resize(hashtable, hashtable->size * 2))
                return false;
            target_bucket = hash_code % hashtable->size;
        }
        index = hashtable->count;
        hashtable->count++;
    }

    hashtable->entries[index].hash_code = hash_code;
    hashtable->entries[index].next = hashtable->buckets[target_bucket];
    hashtable->entries[index].key = key;
    hashtable->entries[index].value = value;
    hashtable->buckets[target_bucket] = index;
    return true;
}

bool hashtable_add(hashtable_t *hashtable, void *key, void *value) {
    if (hashtable == NULL)
        return false;
    if (key == NULL)
        return false;

    return __hashtable_insert(hashtable, key, value, true, false, false);
}

bool hashtable_set(hashtable_t *hashtable, void *key, void *value, bool free_past_key, bool free_past_value) {
    if (hashtable == NULL)
        return false;
    if (key == NULL)
        return false;

    return __hashtable_insert(hashtable, key, value, false, free_past_value, free_past_key);
}

/* Remove */
bool hashtable_remove(hashtable_t *hashtable, void *key) {
    if (hashtable == NULL)
        return false;
    if (key == NULL)
        return false;

    int hash_code = hashtable->hash(key) & 0x7FFFFFFF;
    int bucket = hash_code % hashtable->size;
    int last = -1;

    int i;
    for (i = hashtable->buckets[bucket]; i >= 0; last = i, i = hashtable->entries[i].next) {
        if (__hashtable_compare_keys(hashtable, hashtable->entries[i].key, key, hashtable->entries[i].hash_code, hash_code)) {
            if (last < 0) {
                hashtable->buckets[bucket] = hashtable->entries[i].next;
            } else {
                hashtable->entries[last].next = hashtable->entries[i].next;
            }

            if (hashtable->freer != NULL)
                hashtable->freer(hashtable->entries[i].key, hashtable->entries[i].value);

            __hashtable_reset_entry(&hashtable->entries[i]);
            hashtable->entries[i].next = hashtable->__free_list;
            hashtable->__free_list = i;
            hashtable->__free_count++;
            return true;
        }
    }
    return false;
}

void hashtable_clear(hashtable_t *hashtable) {
    if (hashtable == NULL)
        return;

    int i;
    for (i = 0; i < hashtable->size; i++) {
        if (hashtable->freer != NULL && hashtable->entries[i].key != NULL) {
            hashtable->freer(hashtable->entries[i].key, hashtable->entries[i].value);
        }

        __hashtable_reset_entry(&hashtable->entries[i]);
        hashtable->buckets[i] = -1;
    }

    hashtable->count = 0;
    hashtable->__free_count = 0;
    hashtable->__free_list = -1;
}

/* Search */
int __hashtable_find_entry(hashtable_t *hashtable, void *key) {
    if (hashtable == NULL)
        return -1;
    if (key == NULL)
        return -1;

    int i;
    int hash_code = hashtable->hash(key) & 0x7FFFFFFF;
    for (i = hashtable->buckets[hash_code % hashtable->size]; i >= 0; i = hashtable->entries[i].next) {
        if (__hashtable_compare_keys(hashtable, hashtable->entries[i].key, key, hashtable->entries[i].hash_code, hash_code))
            return i;
    }
    return -1;
}

void *hashtable_get(hashtable_t *hashtable, void *key) {
    if (hashtable == NULL)
        return NULL;
    if (key == NULL)
        return NULL;

    int i = __hashtable_find_entry(hashtable, key);
    if (i == -1)
        return NULL;
    return hashtable->entries[i].value;
}

void *hashtable_find(hashtable_t *hashtable, void *key, __hashtable_selector selector) {
    if (hashtable == NULL)
        return NULL;
    if (key == NULL)
        return NULL;
    if (selector == NULL)
        return NULL;

    int i;
    int hash_code = hashtable->hash(key) & 0x7FFFFFFF;
    for (i = hashtable->buckets[hash_code % hashtable->size]; i >= 0; i = hashtable->entries[i].next) {
        if (__hashtable_compare_keys(hashtable, hashtable->entries[i].key, key, hashtable->entries[i].hash_code, hash_code) && selector(hashtable->entries[i].key, hashtable->entries[i].value))
            return hashtable->entries[i].value;
    }
    return NULL;
}

/* Other */
int hashtable_count(hashtable_t *hashtable) {
    return hashtable->count - hashtable->__free_count;
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
hashtable_t *__hashtable_alloc_hashtable() {
    hashtable_t *hashtable = malloc(sizeof *hashtable);
    if (hashtable == NULL)
        return NULL;
    return __hashtable_reset_hashtable(hashtable);
}
bool __hashtable_resize(hashtable_t *hashtable, unsigned int size) {
    if (hashtable == NULL)
        return false;
    if (size < hashtable->size)
        return false;

    int *new_buckets = malloc(sizeof *new_buckets * size);
    if (new_buckets == NULL)
        return false;
    int i;
    for (i = 0; i < size; i++)
        new_buckets[i] = -1;

    htentry_t *new_entries = realloc(hashtable->entries, sizeof *hashtable->entries * size);
    if (new_entries == NULL)
        return false;

    for (i = 0; i < hashtable->count; i++) {
        if (i < hashtable->count && new_entries[i].hash_code >= 0) {
            int bucket = new_entries[i].hash_code % size;
            new_entries[i].next = new_buckets[bucket];
            new_buckets[bucket] = i;
        } else {
            __hashtable_reset_entry(&new_entries[i]);
        }
    }

    free(hashtable->buckets);

    hashtable->buckets = new_buckets;
    hashtable->entries = new_entries;
    hashtable->size = size;
    return true;
}

/* Reset */
hashtable_t *__hashtable_reset_hashtable(hashtable_t *hashtable) {
    if (hashtable == NULL)
        return NULL;
    hashtable->entries = NULL;
    hashtable->buckets = NULL;
    hashtable->size = 0;
    hashtable->count = 0;
    hashtable->__free_count = 0;
    hashtable->__free_list = -1;
    hashtable->hash = NULL;
    hashtable->key_comparer = NULL;
    hashtable->freer = NULL;
    return hashtable;
}
void __hashtable_reset_entry(htentry_t *entry) {
    if (entry == NULL)
        return;
    entry->hash_code = -1;
    entry->next = -1;
    entry->key = NULL;
    entry->value = NULL;
}

/* Comparers */
bool __hashtable_compare_keys(hashtable_t *hashtable, void *key1, void *key2, int hash1, int hash2) {
    return hash1 == hash2 && hashtable->key_comparer(key1, key2) == 0;
}
int __hashtable_comparer_int(const void *a, const void *b) {
    return *((const int *)b) - *((const int *)a);
}
int __hashtable_comparer_str(const void *a, const void *b) {
    return strcmp((const char *)b, (const char *)a);
}
int __hashtable_comparer_str_i(const void *a, const void *b) {
    return strcmpi((const char *)b, (const char *)a);
}

/* Freers */
void __hashtable_freer_key(void *key, void *value) {
    if (key != NULL)
        free(key);
}
void __hashtable_freer_value(void *key, void *value) {
    if (value != NULL)
        free(value);
}
void __hashtable_freer_both(void *key, void *value) {
    __hashtable_freer_key(key, value);
    __hashtable_freer_value(key, value);
}
