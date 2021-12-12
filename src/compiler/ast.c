#include "ast.h"

asttype_t ast_node_types[AST_TYPES_COUNT] = {
    {AST_TYPE_UNDEFINED, NULL, NULL},
    {AST_TYPE_HEADER, __ast_header_node_free, __ast_header_node_dump},
    {AST_TYPE_LIST, __ast_list_node_free, __ast_list_node_dump},
    {AST_TYPE_TEXT, __ast_text_node_free, __ast_text_node_dump},
    {AST_TYPE_LINE, __ast_line_node_free, __ast_line_node_dump},
    {AST_TYPE_PROGRAM, __ast_program_node_free, __ast_program_node_dump},
    {AST_TYPE_VAR, __ast_var_node_free, __ast_var_node_dump}};

unsigned int __ast_dump_aux = 0;
stack_t *__ast_dump_stack = NULL;

/* ------------------------------------------ */
/* Core */
/* ------------------------------------------ */

ast_t *ast_create() {
    ast_t *ast = __ast_alloc_ast();
    if (ast == NULL)
        return NULL;

    return ast;
}

void ast_free(ast_t *ast) {
    if (ast == NULL)
        return;

    if (ast->root != NULL)
        ast_free_node(ast->root);

    __ast_reset_ast(ast);
    free(ast);
}

void ast_dump(ast_t *ast, FILE *file) {
    if (ast == NULL)
        return;
    if (__ast_dump_stack == NULL)
        stack_free(__ast_dump_stack);

    __ast_dump_stack = stack_create(__stack_freer_value);
    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_aux));

    fprintf(file, "digraph G {\n");
    fprintf(file, "    rankdir = TB;\n");
    fprintf(file, "    N%d [style = invis, shape = point];\n", __ast_dump_aux);

    ast_dump_node(file, ast->root);

    fprintf(file, "}\n");

    stack_free(__ast_dump_stack);
    __ast_dump_stack = NULL;
}

astnode_t *ast_create_node(asttypes_t type) {
    astnode_t *node = malloc(sizeof *node);
    if (node == NULL)
        return NULL;

    node->type = &ast_node_types[type];
    return node;
}

void ast_free_node(astnode_t *node) {
    if (node == NULL)
        return;

    node->type->freer(node);
    __ast_reset_node(node);
    free(node);
}

void ast_dump_node(FILE *file, astnode_t *node) {
    if (file == NULL)
        return;

    __ast_dump_node(file, node, 0);
}
void __ast_dump_node(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;

    if (node == NULL) {
        __ast_dump_gviz_node(file, "NULL");
        return;
    }

    asttype_t *type = node->type;
    if (type->dumper != NULL)
        type->dumper(file, node, depth);
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */

void __ast_dump_depth(FILE *file, int depth) {
    char buffer[32];
    sprintf(buffer, "%%-%ds", depth * 4);
    fprintf(file, buffer, "");
}

int __ast_dump_gviz_node(FILE *file, char *str) {
    unsigned int to = ++__ast_dump_aux;

    void *from_ptr;
    stack_peek(__ast_dump_stack, &from_ptr);
    unsigned int from = *(unsigned int *)from_ptr;

    if (str == NULL)
        fprintf(file, "    N%u -> N%u;\n", from, to);
    else {
        fprintf(file, "    N%u [label=\"%s\"];\n", to, str);
        fprintf(file, "    N%u -> N%u;\n", from, to);
    }
    return to;
}

/* Alloc */
ast_t *__ast_alloc_ast() {
    ast_t *ast = malloc(sizeof *ast);
    if (ast == NULL)
        return NULL;
    return __ast_reset_ast(ast);
}

astnode_t *__ast_alloc_node() {
    astnode_t *node = malloc(sizeof *node);
    if (node == NULL)
        return NULL;
    return __ast_reset_node(node);
}

/* Reset */
ast_t *__ast_reset_ast(ast_t *ast) {
    if (ast == NULL)
        return NULL;

    ast->root = NULL;
    return ast;
}

astnode_t *__ast_reset_node(astnode_t *node) {
    if (node == NULL)
        return NULL;

    node->type = NULL;
    node->data = NULL;
    return node;
}

/* ------------------------------------------ */
/* Nodes */
/* ------------------------------------------ */
/* header node */
astnode_t *ast_create_header_node(astnode_t *_input, astnode_t *_output, astnode_t *_vars) {
    astnode_t *node = ast_create_node(AST_TYPE_HEADER);
    if (node == NULL)
        return NULL;

    astnheader_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_header_node_data(data);

    data->input = _input;
    data->output = _output;
    data->vars = _vars;
    return node;
}
astnheader_t *__ast_reset_header_node_data(astnheader_t *data) {
    if (data == NULL)
        return NULL;

    data->input = NULL;
    data->output = NULL;
    data->vars = NULL;

    return data;
}
void __ast_header_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astnheader_t *data = (astnheader_t *)node->data;
    if (data != NULL) {
        if (data->input != NULL)
            ast_free_node(data->input);
        if (data->output != NULL)
            ast_free_node(data->output);
        if (data->vars != NULL)
            ast_free_node(data->vars);
    }

#ifdef AST_HEADER_NODE_FREE
    ast_header_node_free(node, data);
#endif

    __ast_reset_header_node_data(data);
    free(data);
}
void __ast_header_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "HEADER")));

    astnheader_t *data = (astnheader_t *)node->data;
    if (data != NULL) {
        if (data->input != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "input")));
            __ast_dump_node(file, data->input, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
        if (data->output != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "output")));
            __ast_dump_node(file, data->output, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
        if (data->vars != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "vars")));
            __ast_dump_node(file, data->vars, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
    }

#ifdef AST_HEADER_NODE_DUMP
    ast_header_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
/* list node */
astnode_t *ast_create_list_node(astnode_t *_next, astnode_t *_value) {
    astnode_t *node = ast_create_node(AST_TYPE_LIST);
    if (node == NULL)
        return NULL;

    astnlist_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_list_node_data(data);

    data->next = _next;
    data->value = _value;
    return node;
}
astnlist_t *__ast_reset_list_node_data(astnlist_t *data) {
    if (data == NULL)
        return NULL;

    data->next = NULL;
    data->value = NULL;

    return data;
}
void __ast_list_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astnlist_t *data = (astnlist_t *)node->data;
    if (data != NULL) {
        if (data->next != NULL)
            ast_free_node(data->next);
        if (data->value != NULL)
            ast_free_node(data->value);
    }

#ifdef AST_LIST_NODE_FREE
    ast_list_node_free(node, data);
#endif

    __ast_reset_list_node_data(data);
    free(data);
}
void __ast_list_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "LIST")));

    astnlist_t *data = (astnlist_t *)node->data;
    if (data != NULL) {
        if (data->next != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "next")));
            __ast_dump_node(file, data->next, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
        if (data->value != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "value")));
            __ast_dump_node(file, data->value, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
    }

#ifdef AST_LIST_NODE_DUMP
    ast_list_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
/* text node */
astnode_t *ast_create_text_node(char *_text) {
    astnode_t *node = ast_create_node(AST_TYPE_TEXT);
    if (node == NULL)
        return NULL;

    astntext_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_text_node_data(data);

    data->text = _text;
    return node;
}
astntext_t *__ast_reset_text_node_data(astntext_t *data) {
    if (data == NULL)
        return NULL;

    data->text = NULL;

    return data;
}
void __ast_text_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astntext_t *data = (astntext_t *)node->data;
#ifdef AST_TEXT_NODE_FREE
    ast_text_node_free(node, data);
#endif

    __ast_reset_text_node_data(data);
    free(data);
}
void __ast_text_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "TEXT")));

#ifdef AST_TEXT_NODE_DUMP
    astntext_t *data = (astntext_t *)node->data;
    ast_text_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
/* line node */
astnode_t *ast_create_line_node(int _line_type, astnode_t *_params) {
    astnode_t *node = ast_create_node(AST_TYPE_LINE);
    if (node == NULL)
        return NULL;

    astnline_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_line_node_data(data);

    data->line_type = _line_type;
    data->params = _params;
    return node;
}
astnline_t *__ast_reset_line_node_data(astnline_t *data) {
    if (data == NULL)
        return NULL;

    data->line_type = 0;
    data->params = NULL;

    return data;
}
void __ast_line_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astnline_t *data = (astnline_t *)node->data;
    if (data != NULL) {
        if (data->params != NULL)
            ast_free_node(data->params);
    }

#ifdef AST_LINE_NODE_FREE
    ast_line_node_free(node, data);
#endif

    __ast_reset_line_node_data(data);
    free(data);
}
void __ast_line_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "LINE")));

    astnline_t *data = (astnline_t *)node->data;
    if (data != NULL) {
        if (data->params != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "params")));
            __ast_dump_node(file, data->params, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
    }

#ifdef AST_LINE_NODE_DUMP
    ast_line_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
/* program node */
astnode_t *ast_create_program_node(astnode_t *_header, astnode_t *_lines) {
    astnode_t *node = ast_create_node(AST_TYPE_PROGRAM);
    if (node == NULL)
        return NULL;

    astnprogram_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_program_node_data(data);

    data->header = _header;
    data->lines = _lines;
    return node;
}
astnprogram_t *__ast_reset_program_node_data(astnprogram_t *data) {
    if (data == NULL)
        return NULL;

    data->header = NULL;
    data->lines = NULL;

    return data;
}
void __ast_program_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astnprogram_t *data = (astnprogram_t *)node->data;
    if (data != NULL) {
        if (data->header != NULL)
            ast_free_node(data->header);
        if (data->lines != NULL)
            ast_free_node(data->lines);
    }

#ifdef AST_PROGRAM_NODE_FREE
    ast_program_node_free(node, data);
#endif

    __ast_reset_program_node_data(data);
    free(data);
}
void __ast_program_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "PROGRAM")));

    astnprogram_t *data = (astnprogram_t *)node->data;
    if (data != NULL) {
        if (data->header != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "header")));
            __ast_dump_node(file, data->header, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
        if (data->lines != NULL) {
            stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "lines")));
            __ast_dump_node(file, data->lines, depth + 1);
            stack_pop(__ast_dump_stack, true);
        }
    }

#ifdef AST_PROGRAM_NODE_DUMP
    ast_program_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
/* var node */
astnode_t *ast_create_var_node(char *_name, char *_value) {
    astnode_t *node = ast_create_node(AST_TYPE_VAR);
    if (node == NULL)
        return NULL;

    astnvar_t *data = malloc(sizeof *data);
    if (node == NULL) {
        ast_free_node(node);
        return NULL;
    }
    node->data = __ast_reset_var_node_data(data);

    data->name = _name;
    data->value = _value;
    return node;
}
astnvar_t *__ast_reset_var_node_data(astnvar_t *data) {
    if (data == NULL)
        return NULL;

    data->name = NULL;
    data->value = NULL;

    return data;
}
void __ast_var_node_free(astnode_t *node) {
    if (node == NULL)
        return;

    astnvar_t *data = (astnvar_t *)node->data;
#ifdef AST_VAR_NODE_FREE
    ast_var_node_free(node, data);
#endif

    __ast_reset_var_node_data(data);
    free(data);
}
void __ast_var_node_dump(FILE *file, astnode_t *node, int depth) {
    if (file == NULL)
        return;
    if (node == NULL)
        return;

    stack_push(__ast_dump_stack, alloc_uint(__ast_dump_gviz_node(file, "VAR")));

#ifdef AST_VAR_NODE_DUMP
    astnvar_t *data = (astnvar_t *)node->data;
    ast_var_node_dump(file, node, data, depth);
#endif

    stack_pop(__ast_dump_stack, true);
}
