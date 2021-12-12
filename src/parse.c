#include "parse.h"

ast_t *yyast;

/* ------------------------------------------ */
/* File Parsing */
/* ------------------------------------------ */
int hrm_ast_from_file(char *_path, ast_t **_ast) {
    if (_path == NULL)
        return -1;
    if (_ast == NULL)
        return -1;
    yyin = fopen(_path, "r");
    int flag = yyparse();
    fclose(yyin);
    if (flag == 0) {
        *_ast = yyast;
        return 0;
    }
    return flag;
}

int hrm_from_file(char *_path, hrmcontext_t **_context, alist_t **_scopes) {
    if (_path == NULL)
        return -1;
    if (_context == NULL)
        return -1;
    ast_t *ast;
    if (hrm_ast_from_file(_path, &ast) != 0)
        return -2;

    astnprogram_t *program = (astnprogram_t *)ast->root->data;
    astnode_t *lines = program->lines;
    astnlist_t *first_item = (astnlist_t *)lines->data;
    astnline_t *first_line = (astnline_t *)first_item->value->data;

    hrmcontext_t *context;
    if (hrmcontext_from_ast(ast, &context) != 0) {
        ast_free(ast);
        return -3;
    }
    alist_t *scopes = NULL;
    if (hrmscopes_from_ast_and_context(ast, context, &scopes) != 0) {
        ast_free(ast);
        hrmcontext_free(context);
        return -4;
    }
    ast_free(ast);
    *_context = context;
    *_scopes = scopes;
    return 0;
}

/* ------------------------------------------ */
/* AST Parsing */
/* ------------------------------------------ */
int hrmcontext_vars_from_ast(ast_t *_ast, hashtable_t **_vars) {
    if (_ast == NULL)
        return -1;
    if (_vars == NULL)
        return -1;
    astnode_t *vars_list = __ast_vars_list(_ast);
    hashtable_t *vars = __from_var_ast_list_to_hashtable(vars_list);
    if (vars == NULL)
        return -2;
    *_vars = vars;
    return 0;
}

int hrmcontext_commands_and_labels_from_ast(ast_t *_ast, hrmcmdlbl_t **_commands_labels) {
    if (_ast == NULL)
        return -1;
    if (_commands_labels == NULL)
        return -1;
    astnode_t *commands_list = __ast_commands_list(_ast);
    hrmcmdlbl_t *commands_labels = __from_line_ast_list_to_command_and_labels(commands_list);
    if (commands_labels == NULL)
        return -2;
    *_commands_labels = commands_labels;
    return 0;
}

int hrmcontext_from_ast(ast_t *_ast, hrmcontext_t **_context) {
    if (_ast == NULL)
        return -1;
    if (_context == NULL)
        return -1;
    hashtable_t *vars;
    if (hrmcontext_vars_from_ast(_ast, &vars) != 0)
        return -2;

    hrmcmdlbl_t *commands_labels;
    if (hrmcontext_commands_and_labels_from_ast(_ast, &commands_labels) != 0) {
        hashtable_free(vars);
        return -3;
    }

    hrmcontext_t *context = hrmcontext_create(commands_labels->commands, vars, commands_labels->labels);
    if (context == NULL) {
        hashtable_free(vars);
        alist_free(commands_labels->commands);
        hashtable_free(commands_labels->labels);
        free(commands_labels);
        return -4;
    }
    *_context = context;
    free(commands_labels);
    return 0;
}

int hrmscope_inputs_from_ast(ast_t *_ast, alist_t **_all_inputs) {
    if (_ast == NULL)
        return -1;
    if (_all_inputs == NULL)
        return -1;
    astnode_t *inputs_list = __ast_inputs_list(_ast);
    alist_t *all_inputs = __from_list_ast_list_to_int_alist(inputs_list);
    if (all_inputs == NULL)
        return -2;
    *_all_inputs = all_inputs;
    return 0;
}

int hrmscope_outputs_from_ast(ast_t *_ast, alist_t **_all_outputs) {
    if (_ast == NULL)
        return -1;
    if (_all_outputs == NULL)
        return -1;
    astnode_t *outputs_list = __ast_outputs_list(_ast);
    alist_t *all_outputs = __from_list_ast_list_to_int_alist(outputs_list);
    if (outputs_list == NULL)
        return -2;
    *_all_outputs = all_outputs;
    return 0;
}

int hrmscopes_from_ast_and_context(ast_t *_ast, hrmcontext_t *_context, alist_t **_scopes) {
    if (_ast == NULL)
        return -1;
    if (_context == NULL)
        return -1;
    if (_scopes == NULL)
        return -1;

    alist_t *all_inputs;
    if (hrmscope_inputs_from_ast(_ast, &all_inputs) != 0)
        return -2;

    alist_t *all_outputs = NULL;
    hrmscope_outputs_from_ast(_ast, &all_outputs);

    int all_inputs_size = alist_count(all_inputs);
    if (all_outputs != NULL) {
        int all_outputs_size = alist_count(all_outputs);
        if (all_inputs_size != all_outputs_size) {
            alist_free(all_inputs);
            alist_free(all_outputs);
            return -4;
        }
    }

    alist_t *scopes = create_scopes_alist();
    if (scopes == NULL) {
        alist_free(all_inputs);
        if (all_outputs != NULL)
            alist_free(all_outputs);
        return -5;
    }

    int i;
    for (i = 0; i < all_inputs_size; i++) {
        if (!alist_add(scopes, hrmscope_create(_context, alist_get(all_inputs, i), alist_get(all_outputs, i)))) {
            alist_free(all_inputs);
            if (all_outputs != NULL)
                alist_free(all_outputs);
            alist_free(scopes);
            return -6;
        }
    }
    *_scopes = scopes;
    alist_free(all_inputs);
    if (all_outputs != NULL)
        alist_free(all_outputs);
    return 0;
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */
astnprogram_t *__ast_program(ast_t *_ast) {
    if (_ast == NULL)
        return NULL;
    return (astnprogram_t *)_ast->root->data;
}

astnheader_t *__ast_header(ast_t *_ast) {
    return (astnheader_t *)__ast_program(_ast)->header->data;
}

astnode_t *__ast_inputs_list(ast_t *_ast) {
    return __ast_header(_ast)->input;
}

astnode_t *__ast_outputs_list(ast_t *_ast) {
    return __ast_header(_ast)->output;
}

astnode_t *__ast_vars_list(ast_t *_ast) {
    return __ast_header(_ast)->vars;
}

astnode_t *__ast_commands_list(ast_t *_ast) {
    return __ast_program(_ast)->lines;
}

alist_t *__from_text_ast_list_to_int_alist(astnode_t *_list) {
    if (_list == NULL)
        return NULL;
    alist_t *int_list = create_common_alist();
    void *result = __ast_list_iterate(_list, int_list, __ast_list_iterate_add_int);
    if (result == NULL) {
        alist_free(int_list);
        return NULL;
    }
    return int_list;
}

alist_t *__from_text_ast_list_to_string_alist(astnode_t *_list) {
    if (_list == NULL)
        return NULL;
    alist_t *string_list = create_common_alist();
    if (string_list == NULL)
        return NULL;
    void *result = __ast_list_iterate(_list, string_list, __ast_list_iterate_add_string);
    if (result == NULL) {
        alist_free(string_list);
        return NULL;
    }
    return string_list;
}

hashtable_t *__from_var_ast_list_to_hashtable(astnode_t *_list) {
    if (_list == NULL)
        return NULL;
    hashtable_t *hashtable = create_vars_table();
    void *result = __ast_list_iterate(_list, hashtable, __ast_list_iterate_add_var);
    if (result == NULL) {
        hashtable_free(hashtable);
        return NULL;
    }
    return hashtable;
}

hrmcmdlbl_t *__from_line_ast_list_to_command_and_labels(astnode_t *_list) {
    if (_list == NULL)
        return NULL;
    hrmcmdlbl_t *commands_labels = malloc(sizeof(hrmcmdlbl_t));
    if (commands_labels == NULL)
        return NULL;
    commands_labels->commands = create_commands_alist();
    commands_labels->labels = create_vars_table();
    void *result = __ast_list_iterate(_list, commands_labels, __ast_list_iterate_add_command);
    if (result == NULL) {
        alist_free(commands_labels->commands);
        hashtable_free(commands_labels->labels);
        free(commands_labels);
        return NULL;
    }
    return commands_labels;
}

alist_t *__from_list_ast_list_to_int_alist(astnode_t *_list) {
    if (_list == NULL)
        return NULL;
    alist_t *list_list = alist_create(NULL, 8);
    void *result = __ast_list_iterate(_list, list_list, __ast_list_iterate_add_int_alist);
    if (result == NULL) {
        alist_free(list_list);
        return NULL;
    }
    return list_list;
}

void *__ast_list_iterate(astnode_t *_list_node, void *_data, __ast_list_iterator _iterator) {
    if (_list_node == NULL)
        return NULL;
    if (_iterator == NULL)
        return NULL;

    astnode_t *l;
    astnlist_t *list_item;
    for (l = _list_node; l != NULL; l = list_item->next) {
        list_item = (astnlist_t *)l->data;
        if (_iterator(_data, list_item->value) != 0)
            return NULL;
    }
    return _data;
}

int __ast_list_iterate_add_int(void *_data, astnode_t *_node) {
    alist_t *list = (alist_t *)_data;
    astntext_t *value = (astntext_t *)_node->data;
    return alist_add(list, alloc_int(atoi(value->text))) ? 0 : -1;
}

int __ast_list_iterate_add_string(void *_data, astnode_t *_node) {
    alist_t *list = (alist_t *)_data;
    astntext_t *value = (astntext_t *)_node->data;
    return alist_add(list, strdup(value->text)) ? 0 : -1;
}

int __ast_list_iterate_add_var(void *_data, astnode_t *_node) {
    hashtable_t *hashtable = (hashtable_t *)_data;
    astnvar_t *value = (astnvar_t *)_node->data;
    return hashtable_add(hashtable, strdup(value->name), alloc_int(atoi(value->value))) ? 0 : -1;
}

int __ast_list_iterate_add_command(void *_data, astnode_t *_node) {
    hrmcmdlbl_t *commands_labels = (hrmcmdlbl_t *)_data;
    astnline_t *value = (astnline_t *)_node->data;
    int line_number = alist_count(commands_labels->commands);
    if (value->line_type == COMMAND_TYPE_LABEL) {
        astnlist_t *parameter_list = (astnlist_t *)(value->params->data);
        astntext_t *parameter_value = (astntext_t *)(parameter_list->value->data);
        return hashtable_add(commands_labels->labels, strdup(parameter_value->text), alloc_int(line_number)) ? 0 : -1;
    } else {
        alist_t *args = NULL;
        if (value->params != NULL) {
            args = __from_text_ast_list_to_string_alist(value->params);
            if (args == NULL)
                return -1;
        }
        return alist_add(commands_labels->commands, hrmcommand_create(value->line_type, args, line_number)) ? 0 : -1;
    }
}

int __ast_list_iterate_add_int_alist(void *_data, astnode_t *_node) {
    alist_t *list = (alist_t *)_data;
    return alist_add(list, __from_text_ast_list_to_int_alist(_node)) ? 0 : -1;
}
