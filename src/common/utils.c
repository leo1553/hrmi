#include "utils.h"

int    max_i(int a, int b)          { return a > b ? a : b; }
float  max_f(float a, float b)      { return a > b ? a : b; }
size_t max_size(size_t a, size_t b) { return a > b ? a : b; }
int    min_i(int a, int b)          { return a < b ? a : b; }
float  min_f(float a, float b)      { return a < b ? a : b; }
size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

/* Fonte: https://referencesource.microsoft.com/#mscorlib/system/collections/hashtable.cs,a791862b539edbee */
bool is_prime(const unsigned int candidate) {
	if((candidate & 1) != 0) {
		int limit = (int)sqrt(candidate);
		int divisor;
		for(divisor = 3; divisor <= limit; divisor += 2) {
			if((candidate % divisor) == 0)
				return false;
		}
		return true;
	}
    return candidate == 2;
}

/* Fonte: https://referencesource.microsoft.com/#mscorlib/system/collections/hashtable.cs,a791862b539edbee */
int get_prime(const unsigned int min) {
    unsigned int i;
        
	unsigned int primes[] = {
        3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
        1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
        17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
        187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
        1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369 };
    int primesc = sizeof(primes) / sizeof(primes[0]);
    	
	/* Procurar primo ja calculado */
	for(i = 0; i < primesc; i++) {
        unsigned int prime = primes[i];
        if(prime >= min) 
			return prime;
    }
    
    /* Procurar primo brute-force */
    for(i = (min | 1); i < INT_MAX; i += 2) {
        if(is_prime(i) && ((i - 1) % HASHTABLE_HASHPRIME != 0))
            return i;
	}
    return min;
}

/* Fonte: http://www.cse.yorku.ca/~oz/hash.html */
int hash_djb2(const size_t arrayc, const void *array) {
	unsigned long hash = 5381;
	unsigned char *_array = (unsigned char*)array;
	unsigned char byte;
	
	int i;
	for(i = 0; i < arrayc; i++) {
	    unsigned char byte = _array[i];
	    hash = ((hash << 5) + hash) + byte; /* hash * 33 + c */
	}        
    return hash;
}

/* Fonte: http://www.cse.yorku.ca/~oz/hash.html */
int hash_sdbm(const size_t arrayc, const void *array) {
	unsigned long hash = 0;
	unsigned char *_array = (unsigned char*)array;
	unsigned char byte;
	
	int i;
	for(i = 0; i < arrayc; i++) {
	    unsigned char byte = _array[i];
	    hash = byte + (hash << 6) + (hash << 16) - hash;
	}        
    return hash;
}

/* Fonte: http://www.cse.yorku.ca/~oz/hash.html */
int hash_lose_lose(const size_t arrayc, const void *array) {
	unsigned long hash = 0;
	unsigned char *_array = (unsigned char*)array;
	unsigned char byte;
	
	int i;
	for(i = 0; i < arrayc; i++) {
	    unsigned char byte = _array[i];
	    hash += byte;
	}        
    return hash;
}

void *memdup(void *source, size_t size) {
	void *dup = malloc(size);
	if(dup == NULL)
		return NULL;
	
	memcpy(dup, source, size);
	return dup;
}

int *alloc_int(int n) {
	int *ptr = malloc(sizeof *ptr);
	if(ptr == NULL)
		return NULL;
	
	*ptr = n;
	return ptr;
}
unsigned int *alloc_uint(unsigned int n) {
	unsigned int *ptr = malloc(sizeof *ptr);
	if(ptr == NULL)
		return NULL;
	
	*ptr = n;
	return ptr;
}

