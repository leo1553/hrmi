#include "interp.h"

/* ------------------------------------------ */
/* Command Types */
/* ------------------------------------------ */
cmdtype_t command_types[COMMAND_TYPES_COUNT] = {
    {"INBOX", __cmd_handler_inbox},
    {"OUTBOX", __cmd_handler_outbox},
    {"COPYFROM", __cmd_handler_copyfrom},
    {"COPYTO", __cmd_handler_copyto},
    {"ADD", __cmd_handler_add},
    {"SUB", __cmd_handler_sub},
    {"BUMPUP", __cmd_handler_bumpup},
    {"BUMPDN", __cmd_handler_bumpdn},
    {"JUMP", __cmd_handler_jump},
    {"JUMPZ", __cmd_handler_jumpz},
    {"JUMPN", __cmd_handler_jumpn},
    {"LABEL", NULL},
#ifdef USE_CUSTOM_COMMANDS
    {"MUL", __cmd_handler_mul},
    {"DIV", __cmd_handler_div},
    {"MOD", __cmd_handler_mod},
    {"JUMPP", __cmd_handler_jumpp},
    {"JUMPNZ", __cmd_handler_jumpnz},
    {"JUMPEQ", __cmd_handler_jumpeq},
    {"JUMPNEQ", __cmd_handler_jumpneq},
    {"JUMPLT", __cmd_handler_jumplt},
    {"JUMPLTEQ", __cmd_handler_jumplteq},
    {"JUMPGT", __cmd_handler_jumpgt},
    {"JUMPGTEQ", __cmd_handler_jumpgteq}
#else
    {"MUL", NULL},
    {"DIV", NULL},
    {"MOD", NULL},
    {"JUMPP", NULL},
    {"JUMPNZ", NULL},
    {"JUMPEQ", NULL},
    {"JUMPNEQ", NULL},
    {"JUMPLT", NULL},
    {"JUMPLTEQ", NULL},
    {"JUMPGT", NULL},
    {"JUMPGTEQ", NULL}
#endif
};

/* ------------------------------------------ */
/* Command */
/* ------------------------------------------ */
hrmcommand_t *hrmcommand_create(int _type, alist_t *_args, int _line) {
    hrmcommand_t *command = malloc(sizeof(hrmcommand_t));
    if (command == NULL)
        return NULL;
    command->type = _type;
    command->args = _args;
    command->line = _line;
    return command;
}

void hrmcommand_free(hrmcommand_t *_command) {
    if (_command->args != NULL) {
        alist_free(_command->args);
        _command->args = NULL;
    }
    _command->type = 0;
    _command->line = 0;
    free(_command);
}

void hrmcommand_dump(FILE *_out_stream, hrmcommand_t *_command) {
    fprintf(_out_stream, "% 3d %s ", _command->line + 1, command_types[_command->type].name);
    if (_command->args != NULL) {
        int args_count = alist_count(_command->args);
        int i;
        for (i = 0; i < args_count; i++) {
            fprintf(_out_stream, "%s", (char *)alist_get(_command->args, i));
            if (i < args_count - 1)
                fprintf(_out_stream, ", ");
        }
    }
    fprintf(_out_stream, "\n");
}

void __void_hrmcommand_free(void *_command) {
    hrmcommand_free((hrmcommand_t *)_command);
}

/* ------------------------------------------ */
/* Context */
/* ------------------------------------------ */
hrmcontext_t *hrmcontext_create(alist_t *_commands, hashtable_t *_vars, hashtable_t *_labels) {
    if (_commands == NULL)
        return NULL;
    if (_vars == NULL)
        return NULL;
    if (_labels == NULL)
        return NULL;
    hrmcontext_t *context = malloc(sizeof(hrmcontext_t));
    if (context == NULL)
        return NULL;
    context->commands = _commands;
    context->vars = _vars;
    context->labels = _labels;
    return context;
}

void hrmcontext_free(hrmcontext_t *_context) {
    if (_context->commands != NULL) {
        alist_free(_context->commands);
        _context->commands = NULL;
    }
    if (_context->vars != NULL) {
        hashtable_free(_context->vars);
        _context->vars = NULL;
    }
    if (_context->labels != NULL) {
        hashtable_free(_context->labels);
        _context->labels = NULL;
    }
    free(_context);
}

void hrmcontext_dump_vars(FILE *_out_stream, hrmcontext_t *_context) {
    fprintf(_out_stream, "Variables:\n");
    __hashtable_dump(_out_stream, _context->vars);
}

void hrmcontext_dump_labels(FILE *_out_stream, hrmcontext_t *_context) {
    fprintf(_out_stream, "Labels:\n");
    __hashtable_dump(_out_stream, _context->labels);
}

void hrmcontext_dump_commands(FILE *_out_stream, hrmcontext_t *_context) {
    fprintf(_out_stream, "Commands:\n");
    int i;
    for (i = 0; i < _context->commands->count; i++) {
        hrmcommand_t *command = alist_get(_context->commands, i);
        hrmcommand_dump(_out_stream, command);
    }
}

/* ------------------------------------------ */
/* Scope */
/* ------------------------------------------ */
hrmscope_t *hrmscope_create(hrmcontext_t *_context, alist_t *_inputs, alist_t *_outputs) {
    if (_context == NULL)
        return NULL;
    if (_inputs == NULL)
        return NULL;
    hrmscope_t *scope = malloc(sizeof(hrmscope_t));
    if (scope == NULL)
        return NULL;
    scope->context = _context;
    scope->inputs = _inputs;
    scope->outputs = _outputs;
    scope->vars = create_vars_table();
    scope->outbox = create_common_alist();
    scope->current_inbox = 0;
    scope->current_line = 0;
    scope->in_hand = 0;
    scope->has_in_hand = false;
    scope->steps = 0;

    /* Copy from context vars */
    int i;
    for (i = 0; i < _context->vars->size; i++) {
        htentry_t entry = _context->vars->entries[i];
        if (entry.hash_code >= 0)
            hashtable_add(scope->vars, strdup(entry.key), alloc_int(*(int *)entry.value));
    }
    return scope;
}

void hrmscope_free(hrmscope_t *_scope, bool _free_context) {
    if (_scope->inputs != NULL) {
        alist_free(_scope->inputs);
        _scope->inputs = NULL;
    }
    if (_scope->outputs != NULL) {
        alist_free(_scope->outputs);
        _scope->outputs = NULL;
    }
    if (_scope->vars != NULL) {
        hashtable_free(_scope->vars);
        _scope->vars = NULL;
    }
    if (_scope->outbox != NULL) {
        alist_free(_scope->outbox);
        _scope->outbox = NULL;
    }
    if (_free_context && _scope->context != NULL)
        hrmcontext_free(_scope->context);
    _scope->context = NULL;
    _scope->current_inbox = 0;
    _scope->current_line = 0;
    _scope->in_hand = 0;
    _scope->has_in_hand = false;
    _scope->steps = 0;
    free(_scope);
}

void hrmscope_execute(FILE *_out_stream, hrmscope_t *_scope) {
    if (_scope == NULL)
        return;

    alist_t *commands = _scope->context->commands;
    int commands_count = alist_count(commands);
    if (commands_count == 0) {
        fprintf(stderr, "Error: no commands\n");
        return;
    }

    hrmcommand_t *command;
    do {
        if (_scope->current_line >= commands_count) {
            fprintf(stderr, "Error: line %d out of range\n", _scope->current_line + 1);
            return;
        }
        command = alist_get(commands, _scope->current_line);
        if (command_types[command->type].handler == NULL) {
            fprintf(stderr, "Error: command %s not implemented\n", command_types[command->type].name);
            return;
        }
        _scope->current_line++;
        _scope->steps++;
        hrmcommand_dump(_out_stream, command);
    } while (command_types[command->type].handler(_scope, command));
}

void hrmscope_dump_inputs(FILE *_out_stream, hrmscope_t *_scope) {
    fprintf(_out_stream, "Inbox:\n");
    __int_alist_dump(_out_stream, _scope->inputs);
}

void hrmscope_dump_outputs(FILE *_out_stream, hrmscope_t *_scope) {
    fprintf(_out_stream, "Output:\n");
    if (_scope->outputs == NULL)
        fprintf(_out_stream, "<empty>\n");
    else
        __int_alist_dump(_out_stream, _scope->outputs);
}

void hrmscope_dump_outbox(FILE *_out_stream, hrmscope_t *_scope) {
    fprintf(_out_stream, "Outbox:\n");
    __int_alist_dump(_out_stream, _scope->outbox);
}

void hrmscope_dump_vars(FILE *_out_stream, hrmscope_t *_scope) {
    fprintf(_out_stream, "Variables:\n");
    __hashtable_dump(_out_stream, _scope->vars);
}

void hrmscope_dump_scope(FILE *_out_stream, hrmscope_t *_scope) {
    fprintf(_out_stream, "current_inbox: %d\n", _scope->current_inbox);
    fprintf(_out_stream, "current_line: %d\n", _scope->current_line);
    fprintf(_out_stream, "in_hand: %d\n", _scope->in_hand);
    fprintf(_out_stream, "has_in_hand: %d\n", _scope->has_in_hand);
    fprintf(_out_stream, "steps: %d\n", _scope->steps);
}

void __void_hrmscope_free(void *_scope) {
    hrmscope_free((hrmscope_t *)_scope, false);
}

/* ------------------------------------------ */
/* Utils */
/* ------------------------------------------ */
hashtable_t *create_vars_table() {
    return hashtable_create(__hash_string, __hashtable_comparer_str, __hashtable_freer_both, 8);
}

alist_t *create_common_alist() {
    return alist_create(__alist_freer_value, 8);
}

alist_t *create_commands_alist() {
    return alist_create(__void_hrmcommand_free, 8);
}

alist_t *create_scopes_alist() {
    return alist_create(__void_hrmscope_free, 8);
}

int __hash_string(const void *_key) {
    int hash = 0;
    int c;
    char *str = (char *)_key;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

void __hashtable_dump(FILE *_out_stream, hashtable_t *_table) {
    htentry_t entry;
    int i;
    for (i = 0; i < hashtable_count(_table); i++) {
        entry = _table->entries[i];
        fprintf(_out_stream, "%s: %d\n", entry.key, *(int *)entry.value);
    }
}

void __int_alist_dump(FILE *_out_stream, alist_t *_alist) {
    int i;
    for (i = 0; i < alist_count(_alist); i++) {
        fprintf(_out_stream, "%d", *(int *)alist_get(_alist, i));
        if (i < alist_count(_alist) - 1)
            fprintf(_out_stream, ", ");
    }
    fprintf(_out_stream, "\n");
}

/* ------------------------------------------ */
/* Command Type Handlers */
/* ------------------------------------------ */
bool __cmd_handler_inbox(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (_scope->current_inbox >= alist_count(_scope->inputs)) {
        if (_scope->outputs != NULL)
            fprintf(stderr, "Error: inbox overflow\n");
        return false;
    }
    _scope->in_hand = *(int *)alist_get(_scope->inputs, _scope->current_inbox);
    _scope->has_in_hand = true;
    _scope->current_inbox++;
    return true;
}

bool __cmd_handler_outbox(hrmscope_t *_scope, hrmcommand_t *_command) {
    alist_add(_scope->outbox, alloc_int(_scope->in_hand));
    _scope->has_in_hand = false;
    if (_scope->outputs != NULL) {
        int outbox_count = alist_count(_scope->outbox);
        if (*(int *)alist_get(_scope->outputs, outbox_count - 1) != _scope->in_hand) {
            fprintf(stderr, "Error: invalid outbox, expected %d, got %d\n",
                    *(int *)alist_get(_scope->outputs, outbox_count - 1), _scope->in_hand);
            return false;
        } else if (_scope->outputs->count == outbox_count)
            return false;
    }
    return true;
}

bool __cmd_handler_copyfrom(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand = *var_value;
    _scope->has_in_hand = true;
    return true;
}

bool __cmd_handler_copyto(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    hashtable_set(_scope->vars, strdup(var_name), alloc_int(_scope->in_hand), true, true);
    return true;
}

bool __cmd_handler_add(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand += *var_value;
    return true;
}

bool __cmd_handler_sub(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand -= *var_value;
    return true;
}

bool __cmd_handler_bumpup(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    *var_value = (*var_value) + 1;
    _scope->in_hand = *var_value;
    _scope->has_in_hand = true;
    return true;
}

bool __cmd_handler_bumpdn(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    *var_value = (*var_value) - 1;
    _scope->in_hand = *var_value;
    _scope->has_in_hand = true;
    return true;
}

bool __cmd_handler_jump(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *label_name = alist_get(_command->args, 0);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    _scope->current_line = *label_value;
    return true;
}

bool __cmd_handler_jumpz(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *label_name = alist_get(_command->args, 0);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand == 0)
        _scope->current_line = *label_value;
    return true;
}

bool __cmd_handler_jumpn(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *label_name = alist_get(_command->args, 0);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand < 0)
        _scope->current_line = *label_value;
    return true;
}

bool __cmd_handler_mul(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand *= *var_value;
    return true;
}

bool __cmd_handler_div(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand /= *var_value;
    return true;
}

bool __cmd_handler_mod(hrmscope_t *_scope, hrmcommand_t *_command) {
    if (!__validate_hand(_scope))
        return false;
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    _scope->in_hand %= *var_value;
    return true;
}

bool __cmd_handler_jumpp(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *label_name = alist_get(_command->args, 0);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand > 0)
        _scope->current_line = *label_value;
    return true;
}

bool __cmd_handler_jumpnz(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *label_name = alist_get(_command->args, 0);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand != 0)
        _scope->current_line = *label_value;
    return true;
}

bool __cmd_handler_jumpeq(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand == *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __cmd_handler_jumpneq(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand != *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __cmd_handler_jumplt(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand < *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __cmd_handler_jumplteq(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand <= *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __cmd_handler_jumpgt(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand > *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __cmd_handler_jumpgteq(hrmscope_t *_scope, hrmcommand_t *_command) {
    char *var_name = alist_get(_command->args, 0);
    int *var_value;
    if (!__read_var_value(_scope, var_name, &var_value))
        return false;
    char *label_name = alist_get(_command->args, 1);
    int *label_value;
    if (!__read_label_value(_scope, label_name, &label_value))
        return false;
    if (_scope->in_hand >= *var_value)
        _scope->current_line = *label_value;
    return false;
}

bool __validate_hand(hrmscope_t *_scope) {
    if (!_scope->has_in_hand) {
        fprintf(stderr, "Error: no value in hand\n");
        return false;
    }
    return true;
}

bool __read_var_value(hrmscope_t *_scope, char *_var_name, int **_var_value) {
    void *value = hashtable_get(_scope->vars, _var_name);
    if (value == NULL) {
        fprintf(stderr, "Error: variable %s not found\n", _var_name);
        return false;
    }
    *_var_value = (int *)value;
    return true;
}

bool __read_label_value(hrmscope_t *_scope, char *_label_name, int **_label_value) {
    void *value = hashtable_get(_scope->context->labels, _label_name);
    if (value == NULL) {
        fprintf(stderr, "Error: label %s not found\n", _label_name);
        return false;
    }
    *_label_value = (int *)value;
    return true;
}
