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
     NUMSTR = 260,
     FMTSTR = 261,
     VARIABLE_NAME = 262,
     FUNCTION_NAME = 263,
     ASK = 264,
     COMMENT = 265,
     CONTINUE = 266,
     DO = 267,
     ERASE = 268,
     FOR = 269,
     GO = 270,
     GOTO = 271,
     IF = 272,
     MODIFY = 273,
     QUIT = 274,
     RETURN = 275,
     SET = 276,
     TYPE = 277,
     WRITE = 278,
     LIBRARY = 279,
     CALL = 280,
     SAVE = 281,
     RUN = 282,
     ALL = 283,
     FABS = 284,
     FATN = 285,
     FCOS = 286,
     FEXP = 287,
     FITR = 288,
     FLOG = 289,
     FRAN = 290,
     FSGN = 291,
     FSIN = 292,
     FSQT = 293,
     FADC = 294,
     FDIS = 295,
     FDXS = 296,
     FCOM = 297,
     FNEW = 298,
     FIN = 299,
     FOUT = 300,
     VARLIST = 301
   };
#endif
/* Tokens.  */
#define STRING 258
#define NUMBER 259
#define NUMSTR 260
#define FMTSTR 261
#define VARIABLE_NAME 262
#define FUNCTION_NAME 263
#define ASK 264
#define COMMENT 265
#define CONTINUE 266
#define DO 267
#define ERASE 268
#define FOR 269
#define GO 270
#define GOTO 271
#define IF 272
#define MODIFY 273
#define QUIT 274
#define RETURN 275
#define SET 276
#define TYPE 277
#define WRITE 278
#define LIBRARY 279
#define CALL 280
#define SAVE 281
#define RUN 282
#define ALL 283
#define FABS 284
#define FATN 285
#define FCOS 286
#define FEXP 287
#define FITR 288
#define FLOG 289
#define FRAN 290
#define FSGN 291
#define FSIN 292
#define FSQT 293
#define FADC 294
#define FDIS 295
#define FDXS 296
#define FCOM 297
#define FNEW 298
#define FIN 299
#define FOUT 300
#define VARLIST 301




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
#line 151 "parse.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

