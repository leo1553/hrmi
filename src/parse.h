#ifndef __INCLUDE_PARSE_H
#define __INCLUDE_PARSE_H

#include "common/utils.h"
#include "compiler/ast.h"
#include "compiler/scanner.yy.h"
#include "includes.h"
#include "interp.h"

extern ast_t *yyast;

typedef struct __hrm_commands_labels hrmcmdlbl_t;
typedef int (*__ast_list_iterator)(void *_data, astnode_t *_node);

/* ------------------------------------------ */
/* File Parsing */
/* ------------------------------------------ */
int hrm_ast_from_file(char *_path, ast_t **_ast);
int hrm_from_file(char *_path, hrmcontext_t **_context, alist_t **_scopes);

/* ------------------------------------------ */
/* AST Parsing */
/* ------------------------------------------ */
struct __hrm_commands_labels {
    alist_t *commands;
    hashtable_t *labels;
};

int hrmcontext_vars_from_ast(ast_t *_ast, hashtable_t **_vars);
int hrmcontext_commands_and_labels_from_ast(ast_t *_ast, hrmcmdlbl_t **_commands_labels);
int hrmcontext_from_ast(ast_t *_ast, hrmcontext_t **_context);

int hrmscope_inputs_from_ast(ast_t *_ast, alist_t **_all_inputs);
int hrmscope_outputs_from_ast(ast_t *_ast, alist_t **_all_outputs);
int hrmscopes_from_ast_and_context(ast_t *_ast, hrmcontext_t *_context, alist_t **_scopes);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */
astnprogram_t *__ast_program(ast_t *_ast);
astnheader_t *__ast_header(ast_t *_ast);
astnode_t *__ast_inputs_list(ast_t *_ast);
astnode_t *__ast_outputs_list(ast_t *_ast);
astnode_t *__ast_vars_list(ast_t *_ast);
astnode_t *__ast_commands_list(ast_t *_ast);

alist_t *__from_text_ast_list_to_int_alist(astnode_t *_list);
alist_t *__from_text_ast_list_to_string_alist(astnode_t *_list);
hashtable_t *__from_var_ast_list_to_hashtable(astnode_t *_list);
hrmcmdlbl_t *__from_line_ast_list_to_command_and_labels(astnode_t *_list);
alist_t *__from_list_ast_list_to_int_alist(astnode_t *_list);

void *__ast_list_iterate(astnode_t *_list_node, void *_data, __ast_list_iterator _iterator);
int __ast_list_iterate_add_int(void *_data, astnode_t *_node);
int __ast_list_iterate_add_string(void *_data, astnode_t *_node);
int __ast_list_iterate_add_var(void *_data, astnode_t *_node);
int __ast_list_iterate_add_command(void *_data, astnode_t *_node);
int __ast_list_iterate_add_int_alist(void *_data, astnode_t *_node);

#endif
