#include <stdio.h>
#include "token.h"

const char *type_name(TokenType t) {
    switch (t) {
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER:     return "NUMBER";
        case TOKEN_PLUS:       return "PLUS";
        case TOKEN_MINUS:      return "MINUS";
        case TOKEN_TIMES:      return "TIMES";
        case TOKEN_DIVIDE:     return "DIVIDE";
        case TOKEN_ASSIGN:     return "ASSIGN";
        case TOKEN_PRINT:      return "PRINT";
        case TOKEN_SEMICOLON:  return "SEMICOLON";
        case TOKEN_LPAREN:     return "LPAREN";
        case TOKEN_RPAREN:     return "RPAREN";
        case TOKEN_EOF:        return "EOF";
        default:               return "UNKNOWN";
    }
}

int main(void) {
    init_scanner("x = 10 + y * 2; print x;");
    Token t;
    do {
        t = next_token();
        printf("%-12s %s\n", type_name(t.type), t.lexeme ? t.lexeme : "");
    } while (t.type != TOKEN_EOF);
    return 0;
}