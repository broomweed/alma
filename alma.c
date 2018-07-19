#include "alma.h"
#include "parse.h"
#include "ast.h"
#include "eval.h"
#include "scope.h"
#include "lib.h"
#include "compile.h"
#include "registry.h"

ACompileAllocation initialize_compilation(ASymbolTable *symtab);
ACompileStatus compile_file(ADeclSeqNode *program, ASymbolTable symtab,
        AScope *scope, AFuncRegistry *reg);
AFunc *finalize_compilation(AScope *scope, ASymbolTable symtab, AFuncRegistry *reg);
int run_main(AFunc *mainfunc);

const char *STDLIB_FILE = "lib/std.alma";

int main (int argc, char **argv) {
    ALMA_PATH = getenv("ALMA_PATH");
    if (!ALMA_PATH) ALMA_PATH = ".";
    int extra_slash = 0;
    if (ALMA_PATH[strlen(ALMA_PATH)-1] != '/') extra_slash = 1;
    char *stdlibpath = malloc(strlen(ALMA_PATH) + strlen(STDLIB_FILE) + 1 + extra_slash);

    strcpy(stdlibpath, ALMA_PATH);
    if (extra_slash) strcat(stdlibpath, "/");
    strcat(stdlibpath, STDLIB_FILE);

    ASymbolTable symtab = NULL;

    ACompileAllocation comp = initialize_compilation(&symtab);

    AScope *libscope = comp.scope->parent;

    ACompileStatus stdlib_stat = put_file_into_scope(stdlibpath, &symtab, comp.scope, comp.reg);

    if (stdlib_stat == compile_fail) {
        fprintf(stderr, "Failed to initialize standard library! Aborting.\n");
        exit(1);
    }

    if (argc == 2) {
        ACompileStatus file_stat = put_file_into_scope(argv[1], &symtab, comp.scope, comp.reg);

        if (file_stat == compile_fail) {
            exit(1);
        }

        AFunc *mainfunc = finalize_compilation(comp.scope, symtab, comp.reg);

        if (file_stat == compile_success) {
            run_main(mainfunc);
        }
    } else if (argc == 1) {
        interact(&symtab, comp.scope, comp.reg);
        exit(0);
    } else {
        fprintf(stderr, "Please supply one file name.\n");
        return 0;
    }

    free_lib_scope(libscope);
    free_registry(comp.reg);
    free_symbol_table(&symtab);

    return 0;
}

ACompileAllocation initialize_compilation(ASymbolTable *symtab) {
    AScope *lib_scope = scope_new(NULL);

    AFuncRegistry *reg = registry_new(20);
    lib_init(symtab, lib_scope, 0);

    AScope *real_scope = scope_new(lib_scope);

    ACompileAllocation result = { compile_success, reg, real_scope };
    return result;
}

/* Finalizes compilation; frees compilation data structures and returns
 * a pointer to the main function. */
AFunc *finalize_compilation(AScope *scope, ASymbolTable symtab, AFuncRegistry *reg) {
    AFunc *mainfunc = scope_find_func(scope, symtab, "main");

    if (mainfunc == NULL) {
        fprintf(stderr, "error: cannot find ‘main’ function\n");
    }

    free_scope(scope);

    return mainfunc;
}

int run_main(AFunc *mainfunc) {
    AStack *stack = stack_new(20);

    if (mainfunc == NULL) {
        free_stack(stack);
        return 1;
    }

    /* Call main. */
    eval_word(stack, NULL, mainfunc);

    free_stack(stack);

    return 0;
}
