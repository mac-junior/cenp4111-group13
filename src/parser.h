#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* Parse the whole input and return the root of the AST.
 * Returns NULL on syntax error. */
ASTNode *parse_program(void);

/* Initialise the parser with a source string.
 * (Calls init_scanner internally.) */
void init_parser(const char *source);

#endif