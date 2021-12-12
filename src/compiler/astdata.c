#include "astdata.h"

/* header node */
void ast_header_node_free(astnode_t *node, astnheader_t *data) {
}

void ast_header_node_dump(FILE *file, astnode_t *node, astnheader_t *data, int depth) {
}

/* line node */
void ast_line_node_free(astnode_t *node, astnline_t *data) {
}

void ast_line_node_dump(FILE *file, astnode_t *node, astnline_t *data, int depth) {
    char str[32];
    sprintf(str, "line_type = %d", data->line_type);
    __ast_dump_gviz_node(file, str);
}

/* list node */
void ast_list_node_free(astnode_t *node, astnlist_t *data) {
}

void ast_list_node_dump(FILE *file, astnode_t *node, astnlist_t *data, int depth) {
}

/* text node */
void ast_text_node_free(astnode_t *node, astntext_t *data) {
    if (data->text != NULL)
        free(data->text);
}

void ast_text_node_dump(FILE *file, astnode_t *node, astntext_t *data, int depth) {
    if (data->text == NULL)
        __ast_dump_gviz_node(file, "NULL");
    else {
        int len = strlen(data->text);
        char *buffer = malloc(sizeof *buffer * (len + 16));
        sprintf(buffer, "\\\"%s\\\"", data->text);

        __ast_dump_gviz_node(file, buffer);

        free(buffer);
    }
}

/* program node */
void ast_program_node_free(astnode_t *node, astnprogram_t *data) {
}

void ast_program_node_dump(FILE *file, astnode_t *node, astnprogram_t *data, int depth) {
}

/* var node */
void ast_var_node_free(astnode_t *node, astnvar_t *data) {
    if (data->name != NULL)
        free(data->name);
    if (data->value != NULL)
        free(data->value);
}

void ast_var_node_dump(FILE *file, astnode_t *node, astnvar_t *data, int depth) {
}
