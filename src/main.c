#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    /* Read the whole file into memory */
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);

    /* Parse and print the tree */
    init_parser(source);
    ASTNode *tree = parse_program();
    ast_print(tree, 0);

    free(source);
    return 0;
}