#include "compile.h"

/* Mutate an AWordSeqNode by replacing compile-time-resolvable words
 * by their corresponding AFunc*s found in scope. */
static
ACompileStatus compile_wordseq(AScope *scope, AWordSeqNode *seq) {
    if (seq == NULL) return compile_success;
    unsigned int errors = 0;

    AAstNode *current = seq->first;
    while (current != NULL) {
        if (current->type == value_node) {
            // for now we just assume these are ok
        } else if (current->type == func_node) {
            fprintf(stderr, "internal error: node already compiled\n");
            errors ++;
        } else if (current->type == paren_node) {
            fprintf(stderr, "internal error: paren_node found in compilation stage\n");
            errors ++;
        } else if (current->type == word_node) {
            /* Find the function bound to its name. */
            AFunc *f = scope_lookup(scope, current->data.sym);
            if (f == NULL) {
                fprintf(stderr, "error: unknown word ‘%s’ at line %d.\n",
                        current->data.sym->name, current->linenum);
                errors ++;
            } else {
                current->type = func_node;
                current->data.func = f;
            }
        } else {
            fprintf(stderr, "Don't yet know how to compile node type %d\n", current->type);
        }
        current = current->next;
    }

    if (errors > 0) {
        return compile_fail;
    }

    return compile_success;
}

/* Mutate an ADeclSeqNode by replacing compile-time-resolvable
 * symbol references with references to AFunc*'s. */
ACompileStatus compile(AScope *scope, ADeclSeqNode *program) {
    if (program == NULL) return compile_success;
    unsigned int errors = 0;
    ADeclNode *current;

    /*-- PASS 1: check names being defined --*/
    current = program->first;
    /* (We do this in a separate pass so that functions being defined can
     * refer to functions later without fear. */
    while (current != NULL) {
        /* Mark that the function will be compiled later. */
        ACompileStatus stat = scope_placehold(scope, current->sym, current->linenum);

        if (stat == compile_fail) {
            errors ++;
        } else if (stat != compile_success) {
            /* in the future, i will probably add another status and
             * forget to check for it here. future proofing */
            fprintf(stderr, "internal error: unrecognized compile status %d in pass 1.\n", stat);
            errors ++;
        }

        current = current->next;
    }

    if (errors != 0) {
        /* If we accidentally defined two functions with the same name in the
         * same scope, bail out now. */
        fprintf(stderr, "Compilation aborted.\n");
        return compile_fail;
    }

    /*-- PASS 2: compile symbols we can resolve, convert to func ptrs --*/
    current = program->first;
    while (current != NULL) {
        ACompileStatus stat = compile_wordseq(scope, current->node);

        /* ... check for errors ... */
        if (stat == compile_fail) {
            fprintf(stderr, "Failed to compile word ‘%s’.\n", current->sym->name);
            errors ++;
            current = current->next;
            continue;
        } else if (stat != compile_success) {
            fprintf(stderr, "internal error: unrecognized compile status %d in pass 2.\n", stat);
            current = current->next;
            errors ++;
            continue;
        }

        // This is also where we'll eventually typecheck stuff before registering it.
        // .. Or will we do that in a third pass? Hmm.

        stat = scope_user_register(scope, current->sym, const_func, current->node);

        if (stat == compile_fail) {
            fprintf(stderr, "Failed to compile word ‘%s’.\n", current->sym->name);
            errors ++;
        } else if (stat != compile_success) {
            fprintf(stderr, "internal error: unrecognized compile status %d in pass 2.\n", stat);
        }

        current = current->next;
    }

    if (errors != 0) {
        fprintf(stderr, "Compilation aborted.\n");
        return compile_fail;
    }

    return compile_success;
}
