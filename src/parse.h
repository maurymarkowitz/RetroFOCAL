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
     VARIABLE_NAME = 261,
     FUNCTION_NAME = 262,
     ASK = 263,
     COMMENT = 264,
     CONTINUE = 265,
     DO = 266,
     ERASE = 267,
     FOR = 268,
     GO = 269,
     GOTO = 270,
     IF = 271,
     MODIFY = 272,
     QUIT = 273,
     RETURN = 274,
     SET = 275,
     TYPE = 276,
     WRITE = 277,
     FABS = 278,
     FATN = 279,
     FCOS = 280,
     FEXP = 281,
     FITR = 282,
     FLOG = 283,
     FRAN = 284,
     FSGN = 285,
     FSIN = 286,
     FSQT = 287,
     FADC = 288,
     FDIS = 289,
     FDXS = 290,
     FCOM = 291,
     FNEW = 292,
     FIN = 293,
     FOUT = 294,
     VARLIST = 295
   };
#endif
/* Tokens.  */
#define STRING 258
#define NUMBER 259
#define NUMSTR 260
#define VARIABLE_NAME 261
#define FUNCTION_NAME 262
#define ASK 263
#define COMMENT 264
#define CONTINUE 265
#define DO 266
#define ERASE 267
#define FOR 268
#define GO 269
#define GOTO 270
#define IF 271
#define MODIFY 272
#define QUIT 273
#define RETURN 274
#define SET 275
#define TYPE 276
#define WRITE 277
#define FABS 278
#define FATN 279
#define FCOS 280
#define FEXP 281
#define FITR 282
#define FLOG 283
#define FRAN 284
#define FSGN 285
#define FSIN 286
#define FSQT 287
#define FADC 288
#define FDIS 289
#define FDXS 290
#define FCOM 291
#define FNEW 292
#define FIN 293
#define FOUT 294
#define VARLIST 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 75 "/Volumes/Bigger/Users/maury/Desktop/RetroFOCAL/src/parse.y"
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
#line 139 "/Volumes/Bigger/Users/maury/Desktop/RetroFOCAL/obj/Intermediates.noindex/RetroFOCAL.build/Debug/retrofocal.build/DerivedSources/y.tab.h"
    YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

