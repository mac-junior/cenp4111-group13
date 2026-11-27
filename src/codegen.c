#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"
#include "codegen.h"

/* ------------------------------------------------------------------ */
/*  Bytecode instruction set (IN_ prefix to avoid clash with ast.h)    */
/* ------------------------------------------------------------------ */

typedef enum {
    IN_PUSH, IN_LOAD, IN_STORE,
    IN_ADD, IN_SUB, IN_MUL, IN_DIV,
    IN_PRINT, IN_HALT
} InstrOp;

typedef struct {
    InstrOp op;
    int     value;
    char    name[32];
} Instr;

#define MAX_INSTR 2048

static Instr code[MAX_INSTR];
static int   code_count = 0;

static void emit(InstrOp op, int value, const char *name) {
    if (code_count >= MAX_INSTR) {
        fprintf(stderr, "Too many instructions\n");
        exit(1);
    }
    Instr *i = &code[code_count++];
    i->op = op;
    i->value = value;
    if (name) {
        strncpy(i->name, name, 31);
        i->name[31] = '\0';
    } else {
        i->name[0] = '\0';
    }
}

/* ------------------------------------------------------------------ */
/*  IR -> bytecode                                                     */
/* ------------------------------------------------------------------ */

static void emit_operand(const char *arg) {
    if (!arg) return;
    int is_num = 1;
    const char *p = arg;
    if (*p == '-') p++;
    if (!*p) is_num = 0;
    while (*p && is_num) {
        if (*p < '0' || *p > '9') is_num = 0;
        else p++;
    }

    if (is_num) {
        emit(IN_PUSH, atoi(arg), NULL);
    } else {
        emit(IN_LOAD, 0, arg);
    }
}

void generate_bytecode(void) {
    code_count = 0;

    for (int i = 0; i < quad_count; i++) {
        Quad *q = &quads[i];

        if (strcmp(q->op, "print") == 0) {
            emit_operand(q->arg1);
            emit(IN_PRINT, 0, NULL);
            continue;
        }

        if (strcmp(q->op, "=") == 0) {
            emit_operand(q->arg1);
            emit(IN_STORE, 0, q->result);
            continue;
        }

        emit_operand(q->arg1);
        emit_operand(q->arg2);

        if      (strcmp(q->op, "+") == 0) emit(IN_ADD, 0, NULL);
        else if (strcmp(q->op, "-") == 0) emit(IN_SUB, 0, NULL);
        else if (strcmp(q->op, "*") == 0) emit(IN_MUL, 0, NULL);
        else if (strcmp(q->op, "/") == 0) emit(IN_DIV, 0, NULL);

        emit(IN_STORE, 0, q->result);
    }

    emit(IN_HALT, 0, NULL);
}

/* ------------------------------------------------------------------ */
/*  Print the bytecode                                                 */
/* ------------------------------------------------------------------ */

void print_bytecode(void) {
    printf("\n=== BYTECODE ===\n");
    for (int i = 0; i < code_count; i++) {
        Instr *in = &code[i];
        switch (in->op) {
            case IN_PUSH:  printf("%3d: PUSH  %d\n", i, in->value); break;
            case IN_LOAD:  printf("%3d: LOAD  %s\n", i, in->name);  break;
            case IN_STORE: printf("%3d: STORE %s\n", i, in->name);  break;
            case IN_ADD:   printf("%3d: ADD\n", i);                 break;
            case IN_SUB:   printf("%3d: SUB\n", i);                 break;
            case IN_MUL:   printf("%3d: MUL\n", i);                 break;
            case IN_DIV:   printf("%3d: DIV\n", i);                 break;
            case IN_PRINT: printf("%3d: PRINT\n", i);               break;
            case IN_HALT:  printf("%3d: HALT\n", i);                break;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Interpreter                                                        */
/* ------------------------------------------------------------------ */

#define STACK_MAX 256
#define VAR_MAX   64

typedef struct { char name[32]; int value; } Var;

static int  stack[STACK_MAX];
static int  sp = 0;
static Var  vars[VAR_MAX];
static int  var_count = 0;

static void push(int v) { stack[sp++] = v; }
static int  pop(void)   { return stack[--sp]; }

static int get_var(const char *name) {
    for (int i = 0; i < var_count; i++)
        if (strcmp(vars[i].name, name) == 0) return vars[i].value;
    return 0;
}

static void set_var(const char *name, int value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = value;
            return;
        }
    }
    strncpy(vars[var_count].name, name, 31);
    vars[var_count].name[31] = '\0';
    vars[var_count].value = value;
    var_count++;
}

void run_bytecode(void) {
    sp = 0;
    var_count = 0;

    printf("\n=== EXECUTION ===\n");
    for (int i = 0; i < code_count; i++) {
        Instr *in = &code[i];
        switch (in->op) {
            case IN_PUSH:  push(in->value); break;
            case IN_LOAD:  push(get_var(in->name)); break;
            case IN_STORE: set_var(in->name, pop()); break;
            case IN_ADD:   { int b = pop(), a = pop(); push(a + b); break; }
            case IN_SUB:   { int b = pop(), a = pop(); push(a - b); break; }
            case IN_MUL:   { int b = pop(), a = pop(); push(a * b); break; }
            case IN_DIV:   { int b = pop(), a = pop(); push(a / b); break; }
            case IN_PRINT: printf("%d\n", pop()); break;
            case IN_HALT:  goto done;
        }
    }
done:
    printf("\n=== FINAL VARIABLES ===\n");
    for (int i = 0; i < var_count; i++)
        printf("%s = %d\n", vars[i].name, vars[i].value);
}