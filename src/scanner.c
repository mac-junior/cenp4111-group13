#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

/* Where we are in the input string */
static const char *input;
static int pos = 0;

/* Peek at the current character without moving */
static char peek(void) { return input[pos]; }

/* Move forward one character */
static void advance(void) { if (input[pos] != '\0') pos++; }

/* Skip spaces, tabs, newlines */
static void skip_whitespace(void) {
    while (peek() == ' ' || peek() == '\t' || peek() == '\n')
        advance();
}

/* Copy n characters starting at 'start' into a fresh C string */
static char *copy_text(int start, int n) {
    char *s = malloc(n + 1);
    strncpy(s, input + start, n);
    s[n] = '\0';
    return s;
}

/* The main function: return the NEXT token */
Token next_token(void) {
    skip_whitespace();

    Token t;
    t.lexeme = NULL;

    char c = peek();

    /* End of input */
    if (c == '\0') {
        t.type = TOKEN_EOF;
        return t;
    }

    /* Identifier or keyword: starts with a letter */
    if (isalpha(c)) {
        int start = pos;
        while (isalnum(peek())) advance();
        t.lexeme = copy_text(start, pos - start);
        if (strcmp(t.lexeme, "print") == 0)
            t.type = TOKEN_PRINT;
        else
            t.type = TOKEN_IDENTIFIER;
        return t;
    }

    /* Number: starts with a digit */
    if (isdigit(c)) {
        int start = pos;
        while (isdigit(peek())) advance();
        t.lexeme = copy_text(start, pos - start);
        t.type = TOKEN_NUMBER;
        return t;
    }

    /* Single-character tokens */
    advance();
    t.lexeme = copy_text(pos - 1, 1);

    switch (c) {
        case '+': t.type = TOKEN_PLUS;      break;
        case '-': t.type = TOKEN_MINUS;     break;
        case '*': t.type = TOKEN_TIMES;     break;
        case '/': t.type = TOKEN_DIVIDE;    break;
        case '=': t.type = TOKEN_ASSIGN;    break;
        case ';': t.type = TOKEN_SEMICOLON; break;
        case '(': t.type = TOKEN_LPAREN;    break;
        case ')': t.type = TOKEN_RPAREN;    break;
        default:  t.type = TOKEN_UNKNOWN;   break;
    }
    return t;
}

/* Set up the scanner with a string */
void init_scanner(const char *source) {
    input = source;
    pos = 0;
}