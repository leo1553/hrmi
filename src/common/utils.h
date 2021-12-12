#ifndef __INCLUDE_UTILS_H
#define __INCLUDE_UTILS_H

#include "../includes.h"

#define HASHTABLE_HASHPRIME 101

int max_i(int a, int b);
float max_f(float a, float b);
size_t max_size(size_t a, size_t b);
int min_i(int a, int b);
float min_f(float a, float b);
size_t min_size(size_t a, size_t b);

bool is_prime(const unsigned int candidate);
int get_prime(const unsigned int min);

int hash_djb2(const size_t arrayc, const void *array);
int hash_sdbm(const size_t arrayc, const void *array);
int hash_lose_lose(const size_t arrayc, const void *array);

void *memdup(void *source, size_t size);

int *alloc_int(int n);
unsigned int *alloc_uint(unsigned int n);

#endif
