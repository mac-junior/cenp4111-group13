#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "ir.h"
#include "optimiser.h"
#include "codegen.h"

/* Prototype — codegen.c defines this, not declared in codegen.h (internal) */
void print_bytecode(void);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);

    init_parser(source);
    ASTNode *tree = parse_program();

    printf("=== AST ===\n");
    ast_print(tree, 0);

    printf("\n=== POSTFIX ===\n");
    if (tree->type == AST_ASSIGN)     emit_postfix(tree->right);
    else if (tree->type == AST_PRINT) emit_postfix(tree->left);

    printf("\n=== QUADRUPLES (before) ===\n");
    emit_quadruples(tree);
    print_quads();

    printf("\n=== OPTIMISATION ===\n");
    optimise_quads();

    printf("\n=== QUADRUPLES (after) ===\n");
    print_quads();

    generate_bytecode();
    print_bytecode();
    run_bytecode();

    free_quads();
    free(source);
    return 0;
}