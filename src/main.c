#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "ir.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    /* Read the whole file */
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);

    /* Parse */
    init_parser(source);
    ASTNode *tree = parse_program();

    /* --- AST --- */
    printf("=== AST ===\n");
    ast_print(tree, 0);

    /* --- Postfix (of the expression inside the statement) --- */
    printf("\n=== POSTFIX ===\n");
    if (tree->type == AST_ASSIGN)      emit_postfix(tree->right);
    else if (tree->type == AST_PRINT)  emit_postfix(tree->left);

    /* --- Quadruples --- */
    printf("\n=== QUADRUPLES ===\n");
    emit_quadruples(tree);

    free(source);
    return 0;
}