/* Interpreter (header) for RetroFOCAL
 Copyright (C) 2023 Maury Markowitz

 Based on RetroBASIC
 Copyright (C) 2020 Maury Markowitz

 Based on gnbasic
 Copyright (C) 1998 James Bowman

This file is part of RetroFOCAL.

RetroFOCAL is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RetroFOCAL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RetroFOCAL; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef __RETROFOCAL_H__
#define __RETROFOCAL_H__

#include "stdhdr.h"

/**
 * @file retrofocal.h
 * @author Maury Markowitz
 * @brief Core interpreter code
 *
 * This is the core of the RetroFOCAL interpreter. It performs all of the
 * underlying FOCAL functionality including parsing the original file using
 * lex/yacc, cleaning up the resulting tokenized code, and then running it.
 */

/* consts used during parsing the source */
#define MAXLINE 3199          // 1.01 through 31.99

/* internal state variables used for I/O and other tasks */
extern bool run_program;      // default to running the program, not just parsing it
extern bool print_stats;      // when the program finishes running, should we print statistics?
extern bool write_stats;      // ... or write them to a file?

extern int tab_columns;       // based on PET BASIC, which is a good enough target
extern bool trace_lines;
extern bool ask_colon;        // print a colon before each ASK input (like BASIC's ?)
extern bool type_equals;      // print an equals before each TYPE output?
extern bool upper_case;       // force ASK inputs to upper case
extern double random_seed;    // reset with RANDOMIZE, if -1 then auto-seeds

extern char *source_file;
extern char *input_file;
extern char *print_file;
extern char *stats_file;

/* variable **references** */
/* this is used to record a reference to a variable in the code,
   not it's value. So this might be A or A$ or A(1,2).
   The current value is held in a separate variable_storage_t
   in the variable_values list of the interpreter_state.
 */
typedef struct {
  char *name;
  list_t *subscripts;      // subscripts, list of expressions
} variable_t;

/* either_t is used within variable_value_t for the actual data */
typedef union {
  char *string;
  double number;
} either_t;

/* variable_storage_t holds the *value* of a variable in memory, it is a variable_t */
typedef struct {
  int type;             /* NUMBER, STRING */
  list_t *actual_dimensions;    // actual dimensions, even if auto-DIMmed
  either_t *value;      // actual value(s), malloced
} variable_storage_t;

/* expressions */
typedef enum {
  number, string, numstr, variable, op
} expression_type_t;

typedef struct expression_struct {
  expression_type_t type;
  union {
    double number;
    char *string;
    variable_t *variable;   // also used for user-defined function names and parameters
    struct {
      int arity;
      int opcode;
      struct expression_struct *p[3]; // arity can be up to 3 in BASIC
    } op;
  } parms;
} expression_t;

/* printlists */
/* print lists are different from other lists in BASIC because they
 have three possible separators, nulls, commas and semis
 */
typedef struct {
  expression_t *expression;
  int separator;      /* ';' ',' or 0 */
  char *format;      /* %x.y */
} printitem_t;

/* Every statement in the program gets a statement entry. the most
 basic forms are simply a type, which contains the token value. These
 are used for things like QUIT, which doesn't have any paramaters.
 Other statements can store additional parameters in the params union.
 */
typedef struct statement_struct {
  int type;
  union {
    struct {
      variable_t *variable;
      expression_t *begin, *end, *step;
    } _for;
    double _do;
    double go;
    struct {
      expression_t *condition;
      double less_line;
      double zero_line;
      double more_line;
    } _if;
    list_t *input;
    list_t *print;
    struct {
      variable_t *variable;
      expression_t *expression;
    } set;
    char *rem;
  } parms;
} statement_t;

/* runtime stacks
 * used for tracking DO, FOR, etc.
 *
 * due to the way FOCAL does a NEXT or RETURN at the end of
 * lines, we need to track the original line number, which is
 * not needed in BASIC. We could do this using line_for_statement
 * on the head or returnpoint, but in the case of loops we're going
 * to be doing this a lot so we'll store it.
 *
 * In the case of a DO, we also need to know the original target
 * line, because DO can automatically RETURN at the end of a group
 * or individual line, so we record target_line.
 */
typedef struct {
  int type;
  double original_line; // need to know what line the DO or FOR was on, so record it to avoid searching for it
  double target_line;   // in the case of a DO, we also need to know where it was going, so we can see if it's at the end of that group
  list_t *head;
  list_t *returnpoint;
  variable_t *index_variable;
  double begin, end, step;
} stackentry_t;

/* this is the main state for the interpreter, largely consisting of the lines of
 code, a pointer to the first line for easy lookup, a pointer to the current
 statement, a list of variables and their values, and the runtime stack for
 GOSUB and FOR/NEXT */
typedef struct {
  list_t *lines[MAXLINE];         // the lines in the program, stored as an array of statement lists
  int first_line_index;		        // index of the first line in the lines array, this is *100 the FOCAL line, thus the name
  list_t *current_statement;      // currently executing statement
  list_t *next_statement;         // next statement to run, might change for GOTO and such
  list_t *variable_values;		    // name/value pairs used to store variable values
  list_t *stack;	                // runtime stack
  int cursor_column;              // current column of the output cursor
  char *format;                   // FOCAL uses a single print format
  int running_state;              // is the program running (1), paused/stopped (0), or setting up a function (-1)
} interpreterstate_t;

/* and here's the link to an instance of interpstate_t defined in the c side */
extern interpreterstate_t interpreter_state;

/* the only piece of the interpreter the parser needs to know about is the variable table */
void insert_variable(const variable_t *variable);

/* perform post-parse setup */
void interpreter_post_parse(void);

/* the interpreter entry point */
void interpreter_run(void);

#endif
