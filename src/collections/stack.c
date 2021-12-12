#include "stack.h"

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
stack_t *stack_create(__stack_freer freer) {
	stack_t *stack = __stack_alloc_stack();
	if(stack == NULL)
		return NULL;
		
	stack->freer = freer;
	return stack;
}

void stack_free(stack_t *stack) {
	if(stack == NULL)
		return;
		
	stknode_t *node;
	while(stack->head != NULL) {
		node = stack->head;
		stack->head = node->prev;
		
		__stack_free_node(stack, node);
	}
	
	__stack_reset_stack(stack);
	free(stack);
}

/* Add */
stknode_t *stack_push(stack_t *stack, void *value) {
	if(stack == NULL)
		return NULL;
	
	stknode_t *node = __stack_alloc_node();
	if(node == NULL)
		return NULL;
	
	node->value = value;
	node->prev = stack->head;
	
	stack->head = node;
	stack->count++;
	return node;
}

/* Remove */
bool stack_pop(stack_t *stack, bool free_value) {
	if(stack == NULL)
		return false;
		
	/* Empty stack */
	if(stack->head == NULL)
		return false;
	
	stknode_t *node = stack->head;
	stack->head = node->prev;
	stack->count--;
	
	if(free_value)
		__stack_free_node(stack, node);
	else  {
		__stack_reset_node(node);
		free(node);
	}
	
	return true;
}

/* Search */
bool stack_peek(stack_t *stack, void **value) {
	if(stack == NULL)
		return false;
	if(value == NULL)
		return false;
	
	/* Empty stack */
	if(stack->head == NULL)
		return false;
	
	*value = stack->head->value;
	return true;
}

bool stack_seek(stack_t *stack, int depth, void **value) {
	if(stack == NULL)
		return false;
	if(value == NULL)
		return false;
	
	/* Empty stack */
	if(stack->head == NULL)
		return false;
	if(depth < 0 || depth >= stack->count)
		return false;
		
	stknode_t *current_node = stack->head;
	int index = 0;
	while(index++ != depth) {
		current_node = current_node->prev;
		if(current_node == NULL)
			return false;
	}
	
	*value = current_node->value;
	return true;
}

/* Other */
int stack_count(stack_t *stack) {
	if(stack == NULL)
		return -1;
	
	int count = 0;
	stknode_t *current_node = stack->head;
	while(current_node != NULL) {
		current_node = current_node->prev;
		count++;
	} 
	
	stack->count = count;
	return count;
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

/* Alloc */
stack_t *__stack_alloc_stack() {
	stack_t *stack = malloc(sizeof *stack);
	if(stack == NULL)
		return NULL;
	return __stack_reset_stack(stack);
}

stknode_t *__stack_alloc_node() {
	stknode_t *node = malloc(sizeof *node);
	if(node == NULL)
		return NULL;
	return __stack_reset_node(node);
}

/* Reset */
stack_t *__stack_reset_stack(stack_t *stack) {
	if(stack == NULL)
		return NULL;
	
	stack->head = NULL;
	stack->count = 0;
	stack->freer = NULL;
	return stack;
}

stknode_t *__stack_reset_node(stknode_t *node) {
	if(node == NULL)
		return NULL;
		
	node->prev = NULL;
	node->value = NULL;
	return node;
}

/* Free */
void __stack_free_node(stack_t *stack, stknode_t *node) {
	if(node == NULL)
		return;
	
	if(stack->freer != NULL)
		stack->freer(node->value);
		
	__stack_reset_node(node);
	free(node);
}

/* Freers */
void __stack_freer_value(void *value) {
	if(value != NULL)
		free(value);
}

