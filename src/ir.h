#ifndef IR_H
#define IR_H

#include "ast.h"

/* One quadruple: (op, arg1, arg2, result) */
typedef struct {
    char *op;
    char *arg1;
    char *arg2;
    char *result;
} Quad;

#define MAX_QUADS 1024

/* The global quadruple list */
extern Quad quads[MAX_QUADS];
extern int  quad_count;

/* Postfix (unchanged) */
void emit_postfix(ASTNode *expr);

/* Build quadruples from a statement (stores in quads[], returns count) */
int emit_quadruples(ASTNode *stmt);

/* Print the current quadruple list */
void print_quads(void);

/* Free all memory used by the quad list */
void free_quads(void);

#endif