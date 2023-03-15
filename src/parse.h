/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     NUMBER = 259,
     VARIABLE_NAME = 260,
     FUNCTION_NAME = 261,
     ASK = 262,
     COMMENT = 263,
     CONTINUE = 264,
     DO = 265,
     ERASE = 266,
     FOR = 267,
     GO = 268,
     GOTO = 269,
     IF = 270,
     MODIFY = 271,
     QUIT = 272,
     RETURN = 273,
     SET = 274,
     TYPE = 275,
     WRITE = 276,
     FABS = 277,
     FATN = 278,
     FCOS = 279,
     FEXP = 280,
     FITR = 281,
     FLOG = 282,
     FRAN = 283,
     FSGN = 284,
     FSIN = 285,
     FSQT = 286,
     FIN = 287,
     FOUT = 288,
     FADC = 289,
     FDIS = 290,
     FDXS = 291,
     VARLIST = 292
   };
#endif
/* Tokens.  */
#define STRING 258
#define NUMBER 259
#define VARIABLE_NAME 260
#define FUNCTION_NAME 261
#define ASK 262
#define COMMENT 263
#define CONTINUE 264
#define DO 265
#define ERASE 266
#define FOR 267
#define GO 268
#define GOTO 269
#define IF 270
#define MODIFY 271
#define QUIT 272
#define RETURN 273
#define SET 274
#define TYPE 275
#define WRITE 276
#define FABS 277
#define FATN 278
#define FCOS 279
#define FEXP 280
#define FITR 281
#define FLOG 282
#define FRAN 283
#define FSGN 284
#define FSIN 285
#define FSQT 286
#define FIN 287
#define FOUT 288
#define FADC 289
#define FDIS 290
#define FDXS 291
#define VARLIST 292




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 75 "src/parse.y"
{
  double d;
  int i;
  char *s;
  list_t *l;
  statement_t *statement;
  expression_t *expression;
  variable_t *variable;
}
/* Line 1529 of yacc.c.  */
#line 133 "parse.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

