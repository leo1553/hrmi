#ifndef __INCLUDE_AST_H
#define __INCLUDE_AST_H

#include "../includes.h"
#include "../common/utils.h"
#include "../collections/stack.h"

#define AST_TYPES_COUNT 7

typedef enum __ast_node_types asttypes_t;
typedef struct __ast_node_type asttype_t;
typedef struct __ast_node astnode_t;
typedef struct __ast_root ast_t;
typedef struct __astnheader_t astnheader_t;
typedef struct __astnlist_t astnlist_t;
typedef struct __astntext_t astntext_t;
typedef struct __astnline_t astnline_t;
typedef struct __astnprogram_t astnprogram_t;
typedef struct __astnvar_t astnvar_t;

typedef void (*__ast_node_freer)(astnode_t *node);
typedef void (*__ast_node_dumper)(FILE *file, astnode_t *node, int depth);

enum __ast_node_types {
	AST_TYPE_UNDEFINED,
	AST_TYPE_HEADER,
	AST_TYPE_LIST,
	AST_TYPE_TEXT,
	AST_TYPE_LINE,
	AST_TYPE_PROGRAM,
	AST_TYPE_VAR
};

struct __ast_node_type {
	asttypes_t index;
	__ast_node_freer freer;
	__ast_node_dumper dumper;
};
struct __ast_node {
	asttype_t *type;
	void *data;
};
struct __ast_root {
	astnode_t *root;
};

extern asttype_t ast_node_types[AST_TYPES_COUNT];

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

/* Alloc */
ast_t *ast_create();
void ast_free(ast_t *ast);
void ast_dump(ast_t *ast, FILE *file);

astnode_t *ast_create_node(asttypes_t type);
void ast_free_node(astnode_t *node);

void ast_dump_node(FILE *file, astnode_t *node);
void __ast_dump_node(FILE *file, astnode_t *node, int depth);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

void __ast_dump_depth(FILE *file, int depth);
int __ast_dump_gviz_node(FILE *file, char *str);

/* Alloc */
ast_t *__ast_alloc_ast();
astnode_t *__ast_alloc_node();

/* Reset */
ast_t *__ast_reset_ast(ast_t *ast);
astnode_t *__ast_reset_node(astnode_t *node);

/* ------------------------------------------ */
/* Nodes */
/* ------------------------------------------ */
#include "astdata.h"

/* header node */
struct __astnheader_t {
	astnode_t* input;
	astnode_t* output;
	astnode_t* vars;
};
astnode_t *ast_create_header_node(astnode_t* _input, astnode_t* _output, astnode_t* _vars);
astnheader_t *__ast_reset_header_node_data(astnheader_t *data);
void __ast_header_node_free(astnode_t *node);
void __ast_header_node_dump(FILE *file, astnode_t *node, int depth);

/* list node */
struct __astnlist_t {
	astnode_t* next;
	astnode_t* value;
};
astnode_t *ast_create_list_node(astnode_t* _next, astnode_t* _value);
astnlist_t *__ast_reset_list_node_data(astnlist_t *data);
void __ast_list_node_free(astnode_t *node);
void __ast_list_node_dump(FILE *file, astnode_t *node, int depth);

/* text node */
struct __astntext_t {
	char* text;
};
astnode_t *ast_create_text_node(char* _text);
astntext_t *__ast_reset_text_node_data(astntext_t *data);
void __ast_text_node_free(astnode_t *node);
void __ast_text_node_dump(FILE *file, astnode_t *node, int depth);

/* line node */
struct __astnline_t {
	int line_type;
	astnode_t* params;
};
astnode_t *ast_create_line_node(int _line_type, astnode_t* _params);
astnline_t *__ast_reset_line_node_data(astnline_t *data);
void __ast_line_node_free(astnode_t *node);
void __ast_line_node_dump(FILE *file, astnode_t *node, int depth);

/* program node */
struct __astnprogram_t {
	astnode_t* header;
	astnode_t* lines;
};
astnode_t *ast_create_program_node(astnode_t* _header, astnode_t* _lines);
astnprogram_t *__ast_reset_program_node_data(astnprogram_t *data);
void __ast_program_node_free(astnode_t *node);
void __ast_program_node_dump(FILE *file, astnode_t *node, int depth);

/* var node */
struct __astnvar_t {
	char* name;
	char* value;
};
astnode_t *ast_create_var_node(char* _name, char* _value);
astnvar_t *__ast_reset_var_node_data(astnvar_t *data);
void __ast_var_node_free(astnode_t *node);
void __ast_var_node_dump(FILE *file, astnode_t *node, int depth);

#endif

