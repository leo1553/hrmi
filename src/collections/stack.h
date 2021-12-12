#ifndef __INCLUDE_STACK_H
#define __INCLUDE_STACK_H

#include "../includes.h"

typedef void (*__stack_freer)(void *value);

typedef struct __stack_node stknode_t;
struct __stack_node {
	stknode_t *prev;
	void *value;
};

typedef struct __stack_root stack_t;
struct __stack_root {
	stknode_t *head;
	int count;
	
	__stack_freer freer;
};

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
stack_t *stack_create(__stack_freer freer);
void stack_free(stack_t *stack);

/* Add */
stknode_t *stack_push(stack_t *stack, void *value);

/* Remove */
bool stack_pop(stack_t *stack, bool free_value);

/* Search */
bool stack_peek(stack_t *stack, void **value);
bool stack_seek(stack_t *stack, int depth, void **value);

/* Other */
int stack_count(stack_t *stack);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
stack_t *__stack_alloc_stack();
stknode_t *__stack_alloc_node();

/* Reset */
stack_t *__stack_reset_stack(stack_t *stack);
stknode_t *__stack_reset_node(stknode_t *node);

/* Free */
void __stack_free_node(stack_t *stack, stknode_t *node);

/* Freers */
void __stack_freer_value(void *value);

#endif
