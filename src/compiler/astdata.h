#ifndef __INCLUDE_ASTDATA_H
#define __INCLUDE_ASTDATA_H

#include "ast.h"

/* header node */
#define AST_HEADER_NODE_FREE
void ast_header_node_free(astnode_t *node, astnheader_t *data);

#define AST_HEADER_NODE_DUMP
void ast_header_node_dump(FILE *file, astnode_t *node, astnheader_t *data, int depth);

/* list node */
#define AST_LIST_NODE_FREE
void ast_list_node_free(astnode_t *node, astnlist_t *data);

#define AST_LIST_NODE_DUMP
void ast_list_node_dump(FILE *file, astnode_t *node, astnlist_t *data, int depth);

/* text node */
#define AST_TEXT_NODE_FREE
void ast_text_node_free(astnode_t *node, astntext_t *data);

#define AST_TEXT_NODE_DUMP
void ast_text_node_dump(FILE *file, astnode_t *node, astntext_t *data, int depth);

/* line node */
#define AST_LINE_NODE_FREE
void ast_line_node_free(astnode_t *node, astnline_t *data);

#define AST_LINE_NODE_DUMP
void ast_line_node_dump(FILE *file, astnode_t *node, astnline_t *data, int depth);

/* program node */
#define AST_PROGRAM_NODE_FREE
void ast_program_node_free(astnode_t *node, astnprogram_t *data);

#define AST_PROGRAM_NODE_DUMP
void ast_program_node_dump(FILE *file, astnode_t *node, astnprogram_t *data, int depth);

/* var node */
#define AST_VAR_NODE_FREE
void ast_var_node_free(astnode_t *node, astnvar_t *data);

#define AST_VAR_NODE_DUMP
void ast_var_node_dump(FILE *file, astnode_t *node, astnvar_t *data, int depth);

#endif

