#ifndef AST_H
#define AST_H

/* Node types for our Abstract Syntax Tree (AST) */
typedef enum {
    AST_NUMBER,      /* leaf: 42 */
    AST_IDENTIFIER,  /* leaf: x */
    AST_BINOP,       /* internal: left op right */
    AST_ASSIGN,      /* internal: name = right */
    AST_PRINT        /* internal: one child */
} ASTNodeType;

/* Operators for AST_BINOP */
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV
} ASTOp;

/* A single node in the tree */
typedef struct ASTNode {
    ASTNodeType type;

    int number;         /* for AST_NUMBER */
    char *name;         /* for AST_IDENTIFIER / AST_ASSIGN */
    ASTOp op;           /* for AST_BINOP */

    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

/* Constructors (implemented in ast.c) */
ASTNode *ast_number(int value);
ASTNode *ast_identifier(const char *name);
ASTNode *ast_binop(ASTOp op, ASTNode *left, ASTNode *right);
ASTNode *ast_assign(const char *name, ASTNode *value);
ASTNode *ast_print_stmt(ASTNode *value);

/* Debug: print the tree */
void ast_print(ASTNode *node, int indent);

#endif