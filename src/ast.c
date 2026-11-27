#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *ast_number(int value) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = AST_NUMBER;
    n->number = value;
    return n;
}

ASTNode *ast_identifier(const char *name) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = AST_IDENTIFIER;
    n->name = strdup(name);
    return n;
}

ASTNode *ast_binop(ASTOp op, ASTNode *left, ASTNode *right) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = AST_BINOP;
    n->op = op;
    n->left = left;
    n->right = right;
    return n;
}

ASTNode *ast_assign(const char *name, ASTNode *value) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = AST_ASSIGN;
    n->name = strdup(name);
    n->right = value;
    return n;
}

ASTNode *ast_print_stmt(ASTNode *value) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = AST_PRINT;
    n->left = value;
    return n;
}

static const char *op_name(ASTOp op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
    }
    return "?";
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case AST_NUMBER:
            printf("NUMBER(%d)\n", node->number);
            break;
        case AST_IDENTIFIER:
            printf("IDENT(%s)\n", node->name);
            break;
        case AST_BINOP:
            printf("BINOP(%s)\n", op_name(node->op));
            ast_print(node->left,  indent + 1);
            ast_print(node->right, indent + 1);
            break;
        case AST_ASSIGN:
            printf("ASSIGN(%s)\n", node->name);
            ast_print(node->right, indent + 1);
            break;
        case AST_PRINT:
            printf("PRINT\n");
            ast_print(node->left, indent + 1);
            break;
    }
}