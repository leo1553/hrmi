#include "includes.h"
#include "interp.h"
#include "parse.h"

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        return 1;
    }

    hrmcontext_t *context;
    alist_t *scopes;

    int parse_status = hrm_from_file(argv[1], &context, &scopes);
    if (parse_status != 0) {
        printf("Could not parse file, error code: %d\n", parse_status);
        return 1;
    }
    int scopes_count = alist_count(scopes);

    hrmcontext_dump_commands(stdout, context);
    hrmcontext_dump_vars(stdout, context);
    hrmcontext_dump_labels(stdout, context);
    printf("\n");

    int i;
    for (i = 0; i < scopes_count; i++) {
        hrmscope_t *scope = alist_get(scopes, i);
        printf("\n");
        hrmscope_dump_inputs(stdout, scope);
        hrmscope_dump_outputs(stdout, scope);
        hrmscope_dump_vars(stdout, scope);

        printf("\nExecution:\n");
        hrmscope_execute(stdout, scope);

        printf("\n");
        hrmscope_dump_outbox(stdout, scope);
        printf("Steps: %d\n", scope->steps);
        printf("Commands: %d\n", context->commands->count);
    }
    hrmcontext_free(context);
    alist_free(scopes);
    return 0;
}
