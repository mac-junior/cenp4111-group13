#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "token.h"

/* The current token we're looking at */
static Token current;

/* Advance to the next token */
static void advance(void) {
    current = next_token();
}

/* If the current token is the given type, consume it and return 1.
 * Otherwise return 0. */
static int match(TokenType t) {
    if (current.type == t) {
        advance();
        return 1;
    }
    return 0;
}

/* Same as match, but on failure print an error and exit. */
static void expect(TokenType t, const char *what) {
    if (!match(t)) {
        fprintf(stderr, "Syntax error: expected %s\n", what);
        exit(1);
    }
}

/* Forward declarations (mutual recursion) */
static ASTNode *parse_expression(void);
static ASTNode *parse_term(void);
static ASTNode *parse_factor(void);
static ASTNode *parse_statement(void);

/* factor := NUMBER | IDENTIFIER | '(' expression ')' */
static ASTNode *parse_factor(void) {
    if (current.type == TOKEN_NUMBER) {
        int value = atoi(current.lexeme);
        advance();
        return ast_number(value);
    }
    if (current.type == TOKEN_IDENTIFIER) {
        char *name = strdup(current.lexeme);
        advance();
        return ast_identifier(name);
    }
    if (match(TOKEN_LPAREN)) {
        ASTNode *inner = parse_expression();
        expect(TOKEN_RPAREN, "')'");
        return inner;
    }
    fprintf(stderr, "Syntax error: unexpected token in expression\n");
    exit(1);
}

/* term := factor (('*' | '/') factor)* */
static ASTNode *parse_term(void) {
    ASTNode *left = parse_factor();
    while (current.type == TOKEN_TIMES || current.type == TOKEN_DIVIDE) {
        ASTOp op = (current.type == TOKEN_TIMES) ? OP_MUL : OP_DIV;
        advance();
        ASTNode *right = parse_factor();
        left = ast_binop(op, left, right);
    }
    return left;
}

/* expression := term (('+' | '-') term)* */
static ASTNode *parse_expression(void) {
    ASTNode *left = parse_term();
    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS) {
        ASTOp op = (current.type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
        advance();
        ASTNode *right = parse_term();
        left = ast_binop(op, left, right);
    }
    return left;
}

/* statement := IDENTIFIER '=' expression ';'
 *            | 'print' expression ';'
 */
static ASTNode *parse_statement(void) {
    if (current.type == TOKEN_IDENTIFIER) {
        char *name = strdup(current.lexeme);
        advance();
        expect(TOKEN_ASSIGN, "'='");
        ASTNode *value = parse_expression();
        expect(TOKEN_SEMICOLON, "';'");
        return ast_assign(name, value);
    }
    if (current.type == TOKEN_PRINT) {
        advance();
        ASTNode *value = parse_expression();
        expect(TOKEN_SEMICOLON, "';'");
        return ast_print_stmt(value);
    }
    fprintf(stderr, "Syntax error: expected statement\n");
    exit(1);
}

/* program := statement EOF */
ASTNode *parse_program(void) {
    ASTNode *stmt = parse_statement();
    expect(TOKEN_EOF, "end of input");
    return stmt;
}

void init_parser(const char *source) {
    init_scanner(source);
    advance();  /* load the first token */
}