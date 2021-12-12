#ifndef __INCLUDE_INTERP_H
#define __INCLUDE_INTERP_H

#include "collections/alist.h"
#include "collections/hashtable.h"
#include "includes.h"

#define USE_CUSTOM_COMMANDS
#define COMMAND_TYPES_COUNT 23

typedef enum __command_types cmdtypes_t;
typedef struct __command_type cmdtype_t;
typedef struct __hrm_command hrmcommand_t;
typedef struct __hrm_context hrmcontext_t;
typedef struct __hrm_scope hrmscope_t;

typedef bool (*__hrm_command_handler)(hrmscope_t *_scope, hrmcommand_t *_command);

/* ------------------------------------------ */
/* Command Types */
/* ------------------------------------------ */
enum __command_types {
    COMMAND_TYPE_INBOX,
    COMMAND_TYPE_OUTBOX,
    COMMAND_TYPE_COPYFROM,
    COMMAND_TYPE_COPYTO,
    COMMAND_TYPE_ADD,
    COMMAND_TYPE_SUB,
    COMMAND_TYPE_BUMPUP,
    COMMAND_TYPE_BUMPDN,
    COMMAND_TYPE_JUMP,
    COMMAND_TYPE_JUMPZ,
    COMMAND_TYPE_JUMPN,
    COMMAND_TYPE_LABEL,

    /* custom */
    COMMAND_TYPE_MUL,
    COMMAND_TYPE_DIV,
    COMMAND_TYPE_MOD,
    COMMAND_TYPE_JUMPP,
    COMMAND_TYPE_JUMPNZ,
    COMMAND_TYPE_JUMPEQ,
    COMMAND_TYPE_JUMPNEQ,
    COMMAND_TYPE_JUMPLT,
    COMMAND_TYPE_JUMPLTEQ,
    COMMAND_TYPE_JUMPGT,
    COMMAND_TYPE_JUMPGTEQ
};

struct __command_type {
    char *name;
    __hrm_command_handler handler;
};

extern cmdtype_t command_types[COMMAND_TYPES_COUNT];

/* ------------------------------------------ */
/* Command */
/* ------------------------------------------ */
struct __hrm_command {
    cmdtypes_t type;
    alist_t *args;
    int line;
};

hrmcommand_t *hrmcommand_create(int _type, alist_t *_args, int _line);
void hrmcommand_free(hrmcommand_t *_command);

void hrmcommand_dump(FILE *_out_stream, hrmcommand_t *_command);

void __void_hrmcommand_free(void *_command);

/* ------------------------------------------ */
/* Context */
/* ------------------------------------------ */
struct __hrm_context {
    alist_t *commands;
    hashtable_t *vars;
    hashtable_t *labels;
};

hrmcontext_t *hrmcontext_create(alist_t *_commands, hashtable_t *_vars, hashtable_t *_labels);
void hrmcontext_free(hrmcontext_t *_context);

void hrmcontext_dump_commands(FILE *_out_stream, hrmcontext_t *_context);
void hrmcontext_dump_vars(FILE *_out_stream, hrmcontext_t *_context);
void hrmcontext_dump_labels(FILE *_out_stream, hrmcontext_t *_context);

/* ------------------------------------------ */
/* Scope */
/* ------------------------------------------ */
struct __hrm_scope {
    hrmcontext_t *context;
    alist_t *inputs;
    alist_t *outputs;
    hashtable_t *vars;
    alist_t *outbox;
    int current_inbox;
    int current_line;
    int in_hand;
    bool has_in_hand;
    int steps;
};

hrmscope_t *hrmscope_create(hrmcontext_t *_context, alist_t *_inputs, alist_t *_outputs);
void hrmscope_free(hrmscope_t *_scope, bool _free_context);

void hrmscope_execute(FILE *_out_stream, hrmscope_t *_scope);

void hrmscope_dump_inputs(FILE *_out_stream, hrmscope_t *_scope);
void hrmscope_dump_outputs(FILE *_out_stream, hrmscope_t *_scope);
void hrmscope_dump_vars(FILE *_out_stream, hrmscope_t *_scope);
void hrmscope_dump_outbox(FILE *_out_stream, hrmscope_t *_scope);
void hrmscope_dump_scope(FILE *_out_stream, hrmscope_t *_scope);

void __void_hrmscope_free(void *_scope);

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */
hashtable_t *create_vars_table();
alist_t *create_common_alist();
alist_t *create_commands_alist();
alist_t *create_scopes_alist();

int __hash_string(const void *key);
void __hashtable_dump(FILE *_out_stream, hashtable_t *_table);
void __int_alist_dump(FILE *_out_stream, alist_t *_alist);

/* ------------------------------------------ */
/* Command Type Handlers */
/* ------------------------------------------ */
bool __cmd_handler_inbox(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_outbox(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_copyfrom(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_copyto(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_add(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_sub(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_bumpup(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_bumpdn(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jump(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpz(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpn(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_mul(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_div(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_mod(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpp(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpnz(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpeq(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpneq(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumplt(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumplteq(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpgt(hrmscope_t *_scope, hrmcommand_t *_command);
bool __cmd_handler_jumpgteq(hrmscope_t *_scope, hrmcommand_t *_command);

bool __validate_hand(hrmscope_t *_scope);
bool __read_var_value(hrmscope_t *_scope, char *_var_name, int **_var_value);
bool __read_label_value(hrmscope_t *_scope, char *_label_name, int **_label_value);

#endif
