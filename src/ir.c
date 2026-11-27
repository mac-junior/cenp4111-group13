#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

/* ------------------------------------------------------------------ */
/*  Postfix (unchanged logic)                                          */
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

static void postfix_walk(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case AST_NUMBER:     printf("%d ", node->number); break;
        case AST_IDENTIFIER: printf("%s ", node->name);   break;
        case AST_BINOP:
            postfix_walk(node->left);
            postfix_walk(node->right);
            printf("%s ", op_symbol(node->op));
            break;
        default: break;
    }
}

void emit_postfix(ASTNode *expr) {
    postfix_walk(expr);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/*  Quadruple storage                                                  */
/* ------------------------------------------------------------------ */

Quad quads[MAX_QUADS];
int  quad_count = 0;

static int temp_count = 0;

static void emit_quad(const char *op, const char *arg1,
                      const char *arg2, const char *result) {
    if (quad_count >= MAX_QUADS) {
        fprintf(stderr, "Too many quadruples\n");
        exit(1);
    }
    Quad *q = &quads[quad_count++];
    q->op     = strdup(op);
    q->arg1   = arg1   ? strdup(arg1)   : NULL;
    q->arg2   = arg2   ? strdup(arg2)   : NULL;
    q->result = result ? strdup(result) : NULL;
}

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
    quad_count = 0;

    if (!stmt) return 0;

    if (stmt->type == AST_ASSIGN) {
        char *value = gen_expr(stmt->right);
        emit_quad("=", value, NULL, stmt->name);
        free(value);
        return quad_count;
    }
    if (stmt->type == AST_PRINT) {
        char *value = gen_expr(stmt->left);
        emit_quad("print", value, NULL, NULL);
        free(value);
        return quad_count;
    }
    return 0;
}

void print_quads(void) {
    for (int i = 0; i < quad_count; i++) {
        Quad *q = &quads[i];
        printf("(%2d) (%-5s, %-6s, %-6s, %-6s)\n",
               i, q->op,
               q->arg1   ? q->arg1   : "-",
               q->arg2   ? q->arg2   : "-",
               q->result ? q->result : "-");
    }
}

void free_quads(void) {
    for (int i = 0; i < quad_count; i++) {
        free(quads[i].op);
        free(quads[i].arg1);
        free(quads[i].arg2);
        free(quads[i].result);
    }
    quad_count = 0;
}