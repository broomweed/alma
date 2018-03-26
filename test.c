#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "alma.h"
#include "ast.h"
#include "ustrings.h"
#include "eval.h"
#include "scope.h"
#include "lib.h"
#include "parse.h"
#include "grammar.tab.h"

#define ALMATESTINTRO(filename) \
    FILE *in = fopen(filename, "r"); \
    ADeclSeqNode *program = NULL; \
    ASymbolTable symtab = NULL; \
    AStack *stack = NULL; \
    AScope *scope = NULL; \
    program = NULL; \
    symtab = NULL; \
    parse_file(in, &program, &symtab);

#define ALMATESTSET() \
    free_stack(stack); \
    free_scope(scope); \
    stack = stack_new(20); \
    scope = scope_new(NULL); \
    lib_init(symtab, scope)

#define ALMATESTCLEAN() \
    free_stack(stack); \
    free_decl_seq(program); \
    free_scope(scope); \
    free_symbol_table(&symtab);

int ustr_check(AUstr *ustr, const char *test) {
    AUstr *tester = parse_string(test, strlen(test));
    int result = ustr_eq(ustr, tester);
    free_ustring(tester);
    return result;
}

START_TEST(test_stack_push) {
    ALMATESTINTRO("tests/simplepush.alma");
    ALMATESTSET();

    ck_assert(program->first != NULL);

    eval_sequence(stack, scope, program->first->node);

    ck_assert_int_eq(stack->size, 5);
    ck_assert(stack_get(stack, 0)->type == str_val);
    ck_assert(ustr_check(stack_get(stack, 0)->data.str, "hello world"));
    ck_assert(stack_get(stack, 1)->type == int_val);
    ck_assert(stack_get(stack, 1)->data.i == 1);
    ck_assert(stack_get(stack, 2)->type == int_val);
    ck_assert(stack_get(stack, 2)->data.i == 2);
    ck_assert(stack_get(stack, 3)->type == int_val);
    ck_assert(stack_get(stack, 3)->data.i == 3);
    ck_assert(stack_get(stack, 4)->type == int_val);
    ck_assert(stack_get(stack, 4)->data.i == 4);

    ALMATESTSET();

    ck_assert(program->first->next != NULL);

    eval_sequence(stack, scope, program->first->next->node);

    ck_assert_int_eq(stack->size, 3);
    ck_assert(stack_get(stack, 0)->type == proto_block);
    ck_assert(stack_get(stack, 1)->type == proto_list);
    ck_assert(stack_get(stack, 2)->type == sym_val);
    ck_assert_str_eq(stack_get(stack, 2)->data.sym->name, "asdf");

    ALMATESTCLEAN();
} END_TEST

START_TEST(test_stack_pop_print) {
    ALMATESTINTRO("tests/simplepop.alma");
    ALMATESTSET();

    eval_sequence(stack, scope, program->first->node);

    ck_assert_int_eq(stack->size, 0);

    ALMATESTCLEAN();
} END_TEST

START_TEST(test_addition) {
    ALMATESTINTRO("tests/basicmath.alma");
    ALMATESTSET();

    eval_sequence(stack, scope, program->first->node);

    ck_assert_int_eq(stack->size, 1);
    ck_assert_int_eq(stack_get(stack, 0)->data.i, 9);

    ALMATESTSET();

    eval_sequence(stack, scope, program->first->node);

    ck_assert_int_eq(stack->size, 1);
    ck_assert_int_eq(stack_get(stack, 0)->data.i, 9);

    ALMATESTSET();

    eval_sequence(stack, scope, program->first->next->node);

    ck_assert_int_eq(stack->size, 1);
    ck_assert_int_eq(stack_get(stack, 0)->data.i, 27);

    ALMATESTCLEAN();
} END_TEST

Suite *simple_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Basics");

    /* core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_stack_push);
    tcase_add_test(tc_core, test_stack_pop_print);
    tcase_add_test(tc_core, test_addition);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = simple_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
