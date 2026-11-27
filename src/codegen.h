#ifndef CODEGEN_H
#define CODEGEN_H

/* Generate stack-machine bytecode from the current quads[] array.
 * Prints each instruction as it is emitted. */
void generate_bytecode(void);

/* Interpret the bytecode just generated.
 * Prints any PRINT statements and final variable values. */
void run_bytecode(void);

#endif