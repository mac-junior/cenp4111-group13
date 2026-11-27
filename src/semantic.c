#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

#define SYM_MAX 128

/* The symbol table: names that have been assigned. */
static char *symbols[SYM_MAX];
static int   sym_count = 0;

static void clear_symbols(void) {
    for (int i = 0; i < sym_count; i++) free(symbols[i]);
    sym_count = 0;
}

static int is_declared(const char *name) {
    for (int i = 0; i < sym_count; i++)
        if (strcmp(symbols[i], name) == 0) return 1;
    return 0;
}

static void declare(const char *name) {
    if (is_declared(name)) return;
    if (sym_count >= SYM_MAX) return;
    symbols[sym_count++] = strdup(name);
}

/* Walk an expression, checking every IDENTIFIER is declared.
 * Returns 1 on success, 0 on failure. */
static int check_expr(ASTNode *node) {
    if (!node) return 1;

    if (node->type == AST_IDENTIFIER) {
        if (!is_declared(node->name)) {
            fprintf(stderr, "Semantic error: use of undeclared variable '%s'\n",
                    node->name);
            return 0;
        }
        return 1;
    }

    if (node->type == AST_BINOP) {
        return check_expr(node->left) && check_expr(node->right);
    }

    if (node->type == AST_NUMBER) {
        return 1;
    }

    return 1;
}

int check_semantics(ASTNode *tree) {
    clear_symbols();

    if (!tree) return 1;

    if (tree->type == AST_ASSIGN) {
        /* Check the right-hand side first (reads) */
        if (!check_expr(tree->right)) return 0;
        /* Then declare the left-hand side */
        declare(tree->name);
        return 1;
    }

    if (tree->type == AST_PRINT) {
        return check_expr(tree->left);
    }

    return 1;
}