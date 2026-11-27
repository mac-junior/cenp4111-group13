#ifndef IR_H
#define IR_H

#include "ast.h"

/* Print the postfix (Reverse Polish) form of an expression tree. */
void emit_postfix(ASTNode *expr);

/* Print three-address-code quadruples for a statement.
 * Returns the number of quadruples emitted. */
int emit_quadruples(ASTNode *stmt);

#endif