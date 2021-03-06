#ifndef _AL_AST_H__
#define _AL_AST_H__

#include "alma.h"

/* Pushing a value */
AAstNode *ast_valnode(unsigned int location, AValue *val);

/* Calling a word */
AAstNode *ast_wordnode(unsigned int location, ASymbol *sym);

/* A sequence of words inside parentheses - we have a pointer to the first one */
AAstNode *ast_parennode(unsigned int location, AWordSeqNode *content);

/* A node representing a "let" introducing a scope. */
AAstNode *ast_letnode(unsigned int location, ADeclSeqNode *decls, AWordSeqNode *words);

/* A node representing an import declaration. */
ADeclNode *ast_importdeclnode(unsigned int location, int just_string, const char *module,
        ASymbol *as, ANameSeqNode *names);

/* A node representing a function declaration. */
ADeclNode *ast_funcdeclnode(unsigned int location, ASymbol *sym, AWordSeqNode *body);

/* Create a new node representing name sequence. */
ANameNode *ast_namenode(unsigned int location, ASymbol *symbol);

/* Create a new node representing a name binding. */
AAstNode *ast_bindnode(unsigned int location, ANameSeqNode *names, AWordSeqNode *words);

/* Create a new node representing a declaration sequence. */
ADeclSeqNode *ast_declseq_new(void);

/* Create a new node representing a name sequence. */
ANameSeqNode *ast_nameseq_new(void);

/* Append a new declaration to an ADeclSeqNode. */
void ast_declseq_append(ADeclSeqNode *seq, ADeclNode *node);

/* Create a new node representing a word/value sequence. */
AWordSeqNode *ast_wordseq_new(void);

/* Prepend a new node to the beginning of an AWordSeqNode. */
void ast_wordseq_prepend(AWordSeqNode *seq, AAstNode *node);

/* Prepend an AWordSeqNode to another AWordSeqNode. */
void ast_wordseq_preconcat(AWordSeqNode * restrict after, AWordSeqNode * restrict before);

/* Append a new node to the end of an AWordSeqNode. */
void ast_wordseq_append(AWordSeqNode *seq, AAstNode *node);

/* Concatenate two AWordSeqNodes together. Doesn't free the second one! */
void ast_wordseq_concat(AWordSeqNode *seq1, AWordSeqNode *seq2);

/* Prepend a new node to the beginning of an ANameSeqNode. */
void ast_nameseq_append(ANameSeqNode *seq, ANameNode *node);

/* Pop the last node off the end of an ANameSeqNode, and return it. */
/* This can only really be done once, but that's ok because we
 * only use this function when parsing function headers. */
ANameNode *ast_nameseq_pop(ANameSeqNode *seq);

/* Allocate a new AProtoList. */
AProtoList *ast_protolist_new(void);

/* Append a new word-sequence to an AProtoList. */
void ast_protolist_append(AProtoList *list, AWordSeqNode *node);

/*--- Printing ---*/

/* Print out an AST node. */
void print_ast_node(AAstNode *x);

/* Print out a protolist. */
void print_protolist(AProtoList *pl);

/* Print out an AST sequence. */
void print_wordseq_node(AWordSeqNode *x);

/* Print out a single declaration. */
void print_declaration(ADeclNode *a);

/* Print out a declaration sequence. */
void print_decl_seq(ADeclSeqNode *x);

/* Same as the above but printing to an arbitrary file */
/* (probably stderr) */
void fprint_ast_node(FILE *out, AAstNode *x);
void fprint_protolist(FILE *out, AProtoList *pl);
void fprint_wordseq_node(FILE *out, AWordSeqNode *x);
void fprint_declaration(FILE *out, ADeclNode *a);
void fprint_decl_seq(FILE *out, ADeclSeqNode *x);

/*--- Freeing ---*/

/* Free an AST node. */
void free_ast_node(AAstNode *to_free);

/* Free a protolist. */
void free_protolist(AProtoList *pl);

/* Free a word-sequence node. */
void free_wordseq_node(AWordSeqNode *to_free);

/* Free a name-sequence node. */
void free_nameseq_node(ANameSeqNode *to_free);

/* Free a declaration node COMPLETELY. (Careful!) */
void free_decl_node(ADeclNode *to_free);

/* Free a declaration sequence node COMPLETELY. (Careful!) */
void free_decl_seq(ADeclSeqNode *to_free);

/* Free only the ADeclNodes of an ADeclSeq -- doesn't free the
 * wordseqs, so we can keep those in the User Func Registry. */
void free_decl_seq_top(ADeclSeqNode *to_free);

#endif
