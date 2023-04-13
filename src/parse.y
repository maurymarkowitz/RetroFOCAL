%{
/* Parser for RetroFOCAL
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

#include "retrofocal.h"
#include "statistics.h"

 /* used to track the line number being processed so
    that errors can report it */
static double errline;

void yyerror(const char *message)
{
  fprintf(stderr, "Syntax error at line %g: %s\n", errline, message);
  exit(1);
}

int yylex(void);

static statement_t *make_statement(int t)
{
  statement_t *new = malloc(sizeof(*new));
  new->type = t;
  return new;
}

static expression_t *make_expression(expression_type_t t)
{
  expression_t *new = malloc(sizeof(*new));
  new->type = t;
  return new;
}

static expression_t *make_operator(int arity, int o)
{
  expression_t *new = make_expression(op);
  new->parms.op.opcode = o;
  new->parms.op.arity = arity;
  return new;
}

#define YYDEBUG 1

 /************************************************************************/

%}

 /* Bison declarations */

 //%define parse.error verbose

%union {
  double d;
  int i;
  char *s;
  list_t *l;
  statement_t *statement;
  expression_t *expression;
  variable_t *variable;
}

%type <l> program line statements
%type <l> printlist exprlist
%type <i> printsep e2op term unary_op fn_0 fn_1
%type <expression> expression expression2 expression3 expression4 function factor
%type <statement> statement
%type <variable> variable

 // make sure STRING is before the numbers, so you can test all numbers as > STRING
%token <s> STRING // only used in constants
%token <d> NUMBER // in FOCAL, even line numbers are doubles
%token <s> NUMSTR // this is the unique "0YES" format, which is stored here as a string for debugging purposes
%token <s> FMTSTR // and this is used to store the format specifier, so rounding doesn't remove trailing zeros
%token <s> VARIABLE_NAME
%token <s> FUNCTION_NAME

 /* following the list from the PDP-8 manual, Appendix A */

 // statements
%token ASK
%token COMMENT
%token CONTINUE
%token DO
%token ERASE
%token FOR
%token GO
%token GOTO
%token IF
%token MODIFY
%token QUIT
%token RETURN
%token SET
%token TYPE
%token WRITE

 // functions from FOCAL-69
%token FABS
%token FATN
%token FCOS
%token FEXP
%token FITR
%token FLOG
%token FRAN
%token FSGN
%token FSIN
%token FSQT

// unsupported functions, included for parsing
%token FADC
%token FDIS
%token FDXS

// mentioned in the manual but seen nowhere
%token FCOM // used to share memory between programs
%token FNEW // user-defined function

// new functions from FOCAL-71
%token FIN  // similar to INKEY
%token FOUT // "outkey"

 // used internally
%token VARLIST

%%

/* Grammar rules */
program:
  line
  |
  program '\n'
	|
	program '\n' line
	|
  program error '\n'
	;

line:
  // keep track of the line number as we parse so we can report offending lines
	NUMBER { errline = $1; } statements
	{
    // there are only 3199 possible lines so we make an array that large
    // even though it ends up mostly empty. to convert the X.Y format, we
    // simply multiply by 100 to shift the decimal so that 3.10 is line 310
    // however, due to decimal conversion, 5.10 might end up as 5.099999...
    // and that would trunced to 5.09, so we have to round the result
	  interpreter_state.lines[(int)round($1 * 100)] = $3;
	}       
	;

  /* statements can be separated by semicolons (not colons, as in BASIC) */
statements:
	statement
  {
	  $$ = lst_prepend(NULL, $1);
  }
  |
  statement ';' statements
  {
    $$ = lst_prepend($3, $1);
  }
	;

statement:
	/* can be empty, in which case we remove it */
  // NOTE: should we? or should we insert a placeholder?
  {
	  $$ = NULL;
	}
  |
  ASK printlist
  {
    statement_t *new = make_statement(ASK);
    new->parms.input = $2;
    $$ = new;
  }
  |
  COMMENT /* the PDP-8 documentation separately lists CONTINUE as a COMMENT */
  {
    statement_t *new = make_statement(COMMENT);
    new->parms.rem = yylval.s;
    $$ = new;
  }
  |
  DO expression /* PDP-8 manual shows "DO ALL", but it is not explained, same as GO? */
  {
    statement_t *new = make_statement(DO);
    new->parms._do = $2;
    $$ = new;
      
    /* static analyzer */
    linenum_do_totals++;
    linenum_constants_total++;
    if ($2->parms.number) {
      if ($2->parms.number == errline) {
        linenum_same_line++;
      } else if ($2->parms.number > errline) {
          linenum_forwards++;
      } else {
          linenum_backwards++;
      }
    }
  }
  |
  ERASE /* ERASE can also be followed by a line or group number to erase those lines. this code only handles clearing out variable values */
  {
    statement_t *new = make_statement(ERASE);
    $$ = new;
  }
	|
	FOR variable '=' expression ',' expression
	{
	  statement_t *new = make_statement(FOR);
	  new->parms._for.variable = $2;
	  new->parms._for.begin = $4;
	  new->parms._for.end = $6;
	  new->parms._for.step = NULL;
	  $$ = new;
    
    /* static analyser */
    for_loops_total++;
    for_loops_step_1++;
	}
	|
	FOR variable '=' expression ',' expression ',' expression /* note the FORTRAN-like syntax with the step in the middle */
	{
	  statement_t *new = make_statement(FOR);
	  new->parms._for.variable = $2;
	  new->parms._for.begin = $4;
	  new->parms._for.step = $6;
    new->parms._for.end = $8;
	  $$ = new;
	
    /* static analyser - consider anything with a STEP special even if it is a 1 */
    for_loops_total++;
  }
  |
  GO /* same as BASIC's RUN */
  {
    statement_t *new = make_statement(GOTO);
    $$ = new;
  }
  |
  GOTO expression /* essentially identical to above, but in the documentation they never put line numbers on a GO */
  {
    statement_t *new = make_statement(GOTO);
    new->parms._do = $2;
    $$ = new;
    
    /* static analyzer */
    linenum_go_totals++;
    linenum_constants_total++;
    if ($2->parms.number) {
      if ($2->parms.number == errline) {
        linenum_same_line++;
      } else if ($2->parms.number > errline) {
          linenum_forwards++;
      } else {
          linenum_backwards++;
      }
    }
  }
	|
	IF '(' expression ')' NUMBER /* IF requires parens, like C */
  {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = $3;
    new->parms._if.less_line = $5;
    $$ = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total++;
    if ($5 == errline) {
      linenum_same_line++;
    } else if ($5 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  }
  |
  IF '(' expression ')' NUMBER ',' NUMBER
  {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = $3;
    new->parms._if.less_line = $5;
    new->parms._if.zero_line = $7;
    $$ = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total++;
    if ($5 == errline) {
      linenum_same_line++;
    } else if ($5 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ($7 == errline) {
      linenum_same_line++;
    } else if ($7 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  }
  |
  IF '(' expression ')' NUMBER ',' NUMBER ',' NUMBER
  {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = $3;
    new->parms._if.less_line = $5;
    new->parms._if.zero_line = $7;
    new->parms._if.more_line = $9;
    $$ = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total++;
    if ($5 == errline) {
      linenum_same_line++;
    } else if ($5 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ($7 == errline) {
      linenum_same_line++;
    } else if ($7 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ($9 == errline) {
      linenum_same_line++;
    } else if ($9 > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  }
  |
  QUIT
  {
    statement_t *new = make_statement(QUIT);
    $$ = new;
  }
	|
	RETURN
	{
	  statement_t *new = make_statement(RETURN);
	  $$ = new;
	}
  |
  SET variable '=' expression /* explicit SET, no implicit version like in BASIC */
  {
    statement_t *new = make_statement(SET);
    new->parms.set.variable = $2;
    new->parms.set.expression = $4;
    $$ = new;
    
    /* static analyser - see if we are setting a value to 0 or 1 */
    if (new->parms.set.expression->type == number) {
      if ((int)new->parms.set.expression->parms.number == 0) {
          assign_zero++;
      } else if ((int)new->parms.set.expression->parms.number == 1
                 && (int)new->parms.set.expression->parms.number == new->parms.set.expression->parms.number) {
        assign_one++;
      } else {
        assign_other++;
      }
    }
  }
  |
  TYPE printlist /* unlike BASIC, the formatter can be anywhere in the line, and there can be more than one */
  {
    statement_t *new = make_statement(TYPE);
    new->parms.print = $2;
    $$ = new;
  }
	|
	TYPE '$' /* lists out all the variables and their values */
	{
	  statement_t *new = make_statement(VARLIST);
	  $$ = new;
	}
	;

 /* expressions in FOCAL are very simple, and there are no comparison
  or binary operators, so it's basically just a five-function calculator
  that can be easily implemented in yacc
  */
 
// %left '='
// %left '+' '-'
// %left '/'
// %left '*' // multiply is higher than divide
// %left '^' // multiply is higher than divide
// %left U-
 
 //expression:
//  expression '+' expression {$$ = $1 + $3;} |
//  expression '-' expression {$$ = $1 - $3;} |
//  expression '*' expression {$$ = $1 * $3;} |
//  expression '/' expression {$$ = $1 / $3;} |
//  '-' term %prec NEG {$$ = -$2; } |
//  expression '^' expression {$$ = $1 ^ $3;} |
//  open_bracket expression close_bracket {$$ = $2;} |
//  FUNCTION {$$ = $2;} |
//  NUMBER {$$ = $1;}
//  ;

expression: expression2;

expression2:
	expression3
	|
	expression2 e2op expression3
	{
	  expression_t *new = make_operator(2, $2);
	  new->parms.op.p[0] = $1;
	  new->parms.op.p[1] = $3;
	  $$ = new;
    
    /* static analyser - if it's + or - 1, record it as an increment */
    if (new->parms.op.p[1]->type == number
        && (int)new->parms.op.p[1]->parms.number == 1
        && (int)new->parms.op.p[1]->parms.number == new->parms.op.p[1]->parms.number) {
      if (new->parms.op.opcode == '+') {
        increments++;
      } else if (new->parms.op.opcode == '-') {
        decrements++;
      }
    }
	}
	;

e2op:   '+' { $$ = '+'; } |
        '-' { $$ = '-'; }
        ;

expression3:
	expression4
	|
	expression3 term expression4
	{
	  expression_t *new = make_operator(2, $2);
	  new->parms.op.p[0] = $1;
	  new->parms.op.p[1] = $3;
	  $$ = new;
	}
	;

term:   '*' { $$ = '*'; } |
        '/' { $$ = '/'; } |
        '^' { $$ = '^'; } ;

expression4:
	function
	|
	unary_op function
	{
	  expression_t *new = make_operator(1, $1);
	  new->parms.op.p[0] = $2;
	  $$ = new;
	}
	;

unary_op:
  '-' { $$ = '-'; }
  ;

function:
	factor
  |
  /* functions with no parameters, like TIME */
  fn_0
  {
    expression_t *new = make_operator(0, $1);
    $$ = new;
  }
  |
  /* functions with optional parameters, which we store but ignore */
  fn_0 "()"
  {
    expression_t *new = make_operator(0, $1);
    $$ = new;
  }
  |
  fn_0 "[]"
  {
    expression_t *new = make_operator(0, $1);
    $$ = new;
  }
  |
  fn_0 "<>"
  {
    expression_t *new = make_operator(0, $1);
    $$ = new;
  }
  |
  /* it is not clear this is allowed, but we'll keep it for now */
  fn_0 '(' expression ')'
  {
    expression_t *new = make_operator(0, $1);
    new->parms.op.p[0] = $3;
    $$ = new;
  }
  |
  fn_0 '[' expression ']'
  {
    expression_t *new = make_operator(0, $1);
    new->parms.op.p[0] = $3;
    $$ = new;
  }
  |
  fn_0 '<' expression '>'
  {
    expression_t *new = make_operator(0, $1);
    new->parms.op.p[0] = $3;
    $$ = new;
  }
	|
	fn_1 '(' expression ')'
  {
    expression_t *new = make_operator(1, $1);
    new->parms.op.p[0] = $3;
    $$ = new;
  }
  |
  fn_1 '[' expression ']'
  {
    expression_t *new = make_operator(1, $1);
    new->parms.op.p[0] = $3;
    $$ = new;
  }
  |
  fn_1 '<' expression '>'
	{
	  expression_t *new = make_operator(1, $1);
	  new->parms.op.p[0] = $3;
	  $$ = new;
	}
	;
  
 /* arity-0 functions */
fn_0:
  FRAN { $$ = FRAN; } |
  FIN  { $$ = FIN; }
  ;

 /* arity-1 functions */
fn_1:
  FABS { $$ = FABS; } |
  FADC { $$ = FADC; } |
  FATN { $$ = FATN; } |
  FCOM { $$ = FCOM; } |
	FCOS { $$ = FCOS; } |
  FEXP { $$ = FEXP; } |
  FDIS { $$ = FDIS; } |
  FDXS { $$ = FDXS; } |
  FITR { $$ = FITR; } |
  FLOG { $$ = FLOG; } |
  FNEW { $$ = FNEW; } |
  FSQT { $$ = FSQT; } |
  FSGN { $$ = FSGN; } |
  FSIN { $$ = FSIN; } |
  FOUT { $$ = FOUT; }
  
 /* ultimately all expressions end up here in factor, which is either a
    constant value, a variable value, or a parened expression. in
    all cases, the result of the expression is a value. most books on
    language design call this level a "factor", so we'll do the same. */
factor:
  NUMBER
	{
	  expression_t *new = make_expression(number);
	  new->parms.number = $1;
	  $$ = new;
    
    /* static analyzer code */
    /* numbers here are not *all* of numbers in a program, line numbers
       (for instance) don't end up here. This *is* what we want for the
       analyzer.
       This means we have to track line numbers separately, like in the
       case of GOTO etc. This will, however, capture all the numbers found
       in expressions, PRINT statements, user formulas, POKEs, etc.
     */
    numeric_constants_total++;
    
    /* basic sizes for ints */
    double num = new->parms.number;
    if (floorf(num) == num) {
      // count decimal digits
      if (num == 0) {
        numeric_constants_zero++;
      } else if (num == 1) {
        numeric_constants_one++;
      }
    }
    /* everything else is a float */
    else {
        numeric_constants_float++;
    }
	}
  |
  NUMSTR
  {
    expression_t *new = make_expression(numstr);
    new->parms.string = $1;
    $$ = new;
  }
	|
	STRING
	{
	  expression_t *new = make_expression(string);
	  new->parms.string = $1;
	  $$ = new;
      
    /* static analyzer code */
    size_t len = strlen($1);
    string_constants_total++;
    if (len > string_constants_max) string_constants_max = (int)len;
  }
  |
  variable
  {
    expression_t *new = make_expression(variable);
    new->parms.variable = $1;
    $$ = new;
  }
  |
  '(' expression ')'
  {
    $$ = $2;
  }
  |
  '[' expression ']'
  {
    $$ = $2;
  }
  |
  '<' expression '>'
  {
    $$ = $2;
  }
	;

variable:
  VARIABLE_NAME
  {
	  variable_t *new = malloc(sizeof(*new));
	  new->name = $1;
	  new->subscripts = NULL;
    $$ = new;
    
    /* add it to the interpreter's variable list for the analyizer*/
    insert_variable(new);
	}
	|
  VARIABLE_NAME '(' exprlist ')' // this assumes only () is allowed for subscripts, not <> or [], and only one-d arrays
  {
    variable_t *new = malloc(sizeof(*new));
    new->name = $1;
    new->subscripts = $3;
    $$ = new;

    insert_variable(new);
  }

printlist:
  expression
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = $1;
    new->separator = 0;
    $$ = lst_prepend(NULL, new);
  }
  |
  printlist expression
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = $2;
    new->separator = 0;
    $$ = lst_append($1, new);
  }
  // this is common in FOCAL, you might see TYPE !!! to add some vertical space
  |
  printsep
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->separator = $1;
    $$ = lst_prepend(NULL, new);
  }
  |
  printlist printsep
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->separator = $2;
    $$ = lst_append($1, new);
  }
  // the formatters are annoying because they are typed in as a number
  // lacking trailing zeros, so 10.4 means 10 width, four decimals. The
  // problem is when you read that back and try to figure out if 10.4 means
  // 10.40 or 10.04. adding to the annoyance is that the format string looks
  // just like a number, so the scanner has to look for the % as well as the
  // digits to match, which leads to...
  |
  FMTSTR
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = $1;
    $$ = lst_append(NULL, new);
  }
  |
  printlist FMTSTR
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = $2;
    $$ = lst_append($1, new);
  }
  // we shouldn't need these, the scanner should match a null here
  |
  '%'
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = "-1";
    $$ = lst_prepend(NULL, new);
  }
  |
  printlist '%'
  {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = "-1";
    $$ = lst_prepend($1, new);
  }
  ;
  
  /* FOCAL uses a number of special characters, not just separators */
printsep:
  ','
  {
	  $$ = ','; // this is the main separator, comparable to semicolon in BASIC
	}
  |
	'!'
  {
	  $$ = '!'; // inserts a cr/lf
	}
  |
  '#'
  {
    $$ = '#'; // inserts a cr
  }
  |
  ':'
  {
    $$ = ':'; // inserts a tab
  }
  ;
	
// /* used in places that take a list of expressions, like function call parameters */
// FOCAL doesn't use this, the only place would be in 2-D arrays which we don't support
exprlist:
	expression
	{
	  $$ = lst_prepend(NULL, $1);
	}
	|
	exprlist ',' expression
	{
	  $$ = lst_append($1, $3);
	}
	;

%%
