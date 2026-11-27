#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

/* ------------------------------------------------------------------ */
/*  Postfix (Reverse Polish Notation) via post-order traversal         */
/* ------------------------------------------------------------------ */

static const char *op_symbol(ASTOp op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
    }
    return "?";
}

/* Walk the tree left, right, root -> postfix order. */
static void postfix_walk(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            printf("%d ", node->number);
            break;
        case AST_IDENTIFIER:
            printf("%s ", node->name);
            break;
        case AST_BINOP:
            postfix_walk(node->left);
            postfix_walk(node->right);
            printf("%s ", op_symbol(node->op));
            break;
        default:
            /* postfix only handles expressions */
            break;
    }
}

void emit_postfix(ASTNode *expr) {
    postfix_walk(expr);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/*  Three-address code as quadruples                                   */
/* ------------------------------------------------------------------ */

/* Global temp counter: t1, t2, t3, ... */
static int temp_count = 0;

/* Emit a quadruple line:  (op, arg1, arg2, result)  */
static void emit_quad(const char *op, const char *arg1,
                      const char *arg2, const char *result) {
    printf("(%-3s, %-6s, %-6s, %-6s)\n",
           op, arg1 ? arg1 : "-", arg2 ? arg2 : "-",
           result ? result : "-");
}

/* Recursively generate TAC for an expression.
 * Returns a malloc'd string naming the result (either a variable or a temp). */
static char *gen_expr(ASTNode *node) {
    if (!node) return NULL;

    if (node->type == AST_NUMBER) {
        char *buf = malloc(32);
        snprintf(buf, 32, "%d", node->number);
        return buf;
    }

    if (node->type == AST_IDENTIFIER) {
        return strdup(node->name);
    }

    if (node->type == AST_BINOP) {
        char *left  = gen_expr(node->left);
        char *right = gen_expr(node->right);

        char *result = malloc(16);
        snprintf(result, 16, "t%d", ++temp_count);

        emit_quad(op_symbol(node->op), left, right, result);

        free(left);
        free(right);
        return result;
    }

    return NULL;
}

int emit_quadruples(ASTNode *stmt) {
    temp_count = 0;

    if (!stmt) return 0;

    if (stmt->type == AST_ASSIGN) {
        char *value = gen_expr(stmt->right);
        emit_quad("=", value, NULL, stmt->name);
        free(value);
        return temp_count + 1;
    }

    if (stmt->type == AST_PRINT) {
        char *value = gen_expr(stmt->left);
        emit_quad("print", value, NULL, NULL);
        free(value);
        return temp_count + 1;
    }

    return 0;
}