#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ir.h"
#include "optimiser.h"

static int is_number(const char *s) {
    if (!s || !*s) return 0;
    if (*s == '-') s++;
    if (!*s) return 0;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

static void set_field(char **field, const char *value) {
    free(*field);
    *field = value ? strdup(value) : NULL;
}

/* ------------------------------------------------------------------ */
/*  1. Constant folding + propagation                                  */
/*  After folding, remember which temps hold constants, and substitute */
/*  those constants into later quads.                                  */
/* ------------------------------------------------------------------ */

static int constant_folding(void) {
    int changed = 0;

    /* Map: temp name -> its integer value (or "not a constant") */
    /* We use a small array of (name, value) pairs. */
    char *names[MAX_QUADS];
    int   values[MAX_QUADS];
    int   known = 0;

    for (int i = 0; i < quad_count; i++) {
        Quad *q = &quads[i];

        /* Substitute known constants into arg1 and arg2 */
        if (q->arg1 && !is_number(q->arg1)) {
            for (int k = 0; k < known; k++) {
                if (strcmp(names[k], q->arg1) == 0) {
                    char buf[32];
                    snprintf(buf, 32, "%d", values[k]);
                    set_field(&q->arg1, buf);
                    changed++;
                    break;
                }
            }
        }
        if (q->arg2 && !is_number(q->arg2)) {
            for (int k = 0; k < known; k++) {
                if (strcmp(names[k], q->arg2) == 0) {
                    char buf[32];
                    snprintf(buf, 32, "%d", values[k]);
                    set_field(&q->arg2, buf);
                    changed++;
                    break;
                }
            }
        }

        /* If both args are now numbers and op is arithmetic, fold */
        if (q->arg1 && q->arg2 &&
            is_number(q->arg1) && is_number(q->arg2)) {
            int a = atoi(q->arg1);
            int b = atoi(q->arg2);
            int value = 0;
            int can = 1;

            if      (strcmp(q->op, "+") == 0) value = a + b;
            else if (strcmp(q->op, "-") == 0) value = a - b;
            else if (strcmp(q->op, "*") == 0) value = a * b;
            else if (strcmp(q->op, "/") == 0) {
                if (b == 0) can = 0;
                else value = a / b;
            } else can = 0;

            if (can) {
                char buf[32];
                snprintf(buf, 32, "%d", value);
                set_field(&q->op,   "=");
                set_field(&q->arg1, buf);
                set_field(&q->arg2, NULL);
                changed++;
            }
        }

        /* If this quad is (=, <number>, -, temp), record temp's value */
        if (strcmp(q->op, "=") == 0 &&
            q->arg1 && is_number(q->arg1) && !q->arg2 && q->result) {
            /* Is it a temp (starts with 't')? */
            if (q->result[0] == 't') {
                names[known] = q->result;
                values[known] = atoi(q->arg1);
                known++;
            }
        }
    }
    return changed;
}

/* ------------------------------------------------------------------ */
/*  2. Dead-code elimination                                           */
/* ------------------------------------------------------------------ */

static int is_used_after(int index, const char *name) {
    if (!name) return 0;
    for (int i = index + 1; i < quad_count; i++) {
        Quad *q = &quads[i];
        if (q->arg1 && strcmp(q->arg1, name) == 0) return 1;
        if (q->arg2 && strcmp(q->arg2, name) == 0) return 1;
    }
    return 0;
}

static int dead_code_elimination(void) {
    int removed = 0;
    int i = 0;
    while (i < quad_count) {
        Quad *q = &quads[i];

        /* Never remove final assignments to variables or print */
        if (strcmp(q->op, "=") == 0 && q->result && q->result[0] != 't') {
            i++; continue;
        }
        if (strcmp(q->op, "print") == 0) {
            i++; continue;
        }

        /* Remove arithmetic or temp-definitions whose result is unused */
        if (q->result && !is_used_after(i, q->result)) {
            free(q->op); free(q->arg1); free(q->arg2); free(q->result);
            for (int j = i; j < quad_count - 1; j++)
                quads[j] = quads[j + 1];
            quad_count--;
            removed++;
        } else {
            i++;
        }
    }
    return removed;
}

/* ------------------------------------------------------------------ */
/*  Public entry point                                                 */
/* ------------------------------------------------------------------ */

void optimise_quads(void) {
    printf("Before optimisation: %d quadruples\n", quad_count);

    int changed;
    do {
        changed = 0;
        changed += constant_folding();
        changed += dead_code_elimination();
    } while (changed > 0);

    printf("After optimisation:  %d quadruples\n", quad_count);
}