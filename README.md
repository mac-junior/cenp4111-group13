# CENP4111 — Group 13 — T13: RPNCALC

**Course:** Introduction to Compiler Construction (CENP4111)
**Institution:** University of Bamenda — COLTECH
**Instructor:** Pr. ONABID Mathias Akong
**Academic year:** 2025/2026

## Topic T13 — Infix-to-Postfix Translation and Evaluation

A mini-compiler for a small language with variables, arithmetic, assignment, and `print`.
It converts **infix expressions to postfix** using the **shunting-yard algorithm**, then
generates **quadruples** (three-address code) and evaluates them.

**Lead module:** Module 5.2 — Intermediate representations
**Implementation track:** Hand-written scanner → shunting-yard → postfix → quadruples → codegen

## Language
x =  = 10 + y * 2;
print x;

./compiler tests/sample1.rpn