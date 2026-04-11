/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "src/parse.y"

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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 193 of yacc.c.  */
#line 268 "parse.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 281 "parse.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   473

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  99
/* YYNRULES -- Number of states.  */
#define YYNSTATES  151

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      47,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,    65,    53,    63,     2,     2,
      51,    52,    56,    54,    50,    55,     2,    57,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    48,
      61,    49,    62,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    59,     2,    60,    58,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    12,    16,    17,    21,    23,
      27,    28,    31,    33,    36,    38,    41,    44,    47,    54,
      63,    65,    67,    70,    76,    84,    94,    98,   102,   106,
     108,   110,   115,   118,   121,   123,   126,   128,   130,   134,
     136,   138,   140,   144,   146,   148,   150,   152,   155,   157,
     159,   161,   165,   169,   173,   178,   183,   188,   193,   198,
     203,   205,   207,   209,   211,   213,   215,   217,   219,   221,
     223,   225,   227,   229,   231,   233,   235,   237,   239,   241,
     243,   245,   249,   253,   257,   259,   264,   266,   269,   271,
     274,   276,   279,   281,   284,   286,   288,   290,   292,   294
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      68,     0,    -1,    69,    -1,    68,    47,    -1,    68,    47,
      69,    -1,    68,     1,    47,    -1,    -1,     4,    70,    71,
      -1,    72,    -1,    72,    48,    71,    -1,    -1,     9,    85,
      -1,    10,    -1,    12,     4,    -1,    13,    -1,    13,    28,
      -1,    13,     4,    -1,    18,     4,    -1,    14,    84,    49,
      73,    50,    73,    -1,    14,    84,    49,    73,    50,    73,
      50,    73,    -1,    15,    -1,    16,    -1,    16,     4,    -1,
      17,    51,    73,    52,     4,    -1,    17,    51,    73,    52,
       4,    50,     4,    -1,    17,    51,    73,    52,     4,    50,
       4,    50,     4,    -1,    24,    25,     3,    -1,    24,    26,
       3,    -1,    24,    27,     3,    -1,    19,    -1,    20,    -1,
      21,    84,    49,    73,    -1,    22,    85,    -1,    22,    53,
      -1,    23,    -1,    23,    73,    -1,    74,    -1,    76,    -1,
      74,    75,    76,    -1,    54,    -1,    55,    -1,    78,    -1,
      76,    77,    78,    -1,    56,    -1,    57,    -1,    58,    -1,
      80,    -1,    79,    80,    -1,    55,    -1,    83,    -1,    81,
      -1,    81,    51,    52,    -1,    81,    59,    60,    -1,    81,
      61,    62,    -1,    81,    51,    73,    52,    -1,    81,    59,
      73,    60,    -1,    81,    61,    73,    62,    -1,    82,    51,
      73,    52,    -1,    82,    59,    73,    60,    -1,    82,    61,
      73,    62,    -1,    35,    -1,    44,    -1,    29,    -1,    39,
      -1,    30,    -1,    42,    -1,    31,    -1,    32,    -1,    40,
      -1,    41,    -1,    33,    -1,    34,    -1,    43,    -1,    38,
      -1,    36,    -1,    37,    -1,    45,    -1,     4,    -1,     5,
      -1,     3,    -1,    84,    -1,    51,    73,    52,    -1,    59,
      73,    60,    -1,    61,    73,    62,    -1,     7,    -1,     7,
      51,    87,    52,    -1,    73,    -1,    85,    73,    -1,    86,
      -1,    85,    86,    -1,     6,    -1,    85,     6,    -1,    63,
      -1,    85,    63,    -1,    50,    -1,    64,    -1,    65,    -1,
      66,    -1,    73,    -1,    87,    50,    73,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   158,   158,   160,   162,   164,   169,   169,   182,   187,
     196,   200,   207,   214,   232,   239,   246,   257,   264,   278,
     291,   298,   307,   325,   344,   371,   406,   414,   421,   428,
     434,   440,   460,   467,   473,   480,   512,   515,   517,   537,
     538,   542,   544,   553,   554,   555,   558,   560,   569,   573,
     576,   583,   589,   595,   602,   609,   616,   623,   630,   637,
     647,   648,   653,   654,   655,   656,   657,   658,   659,   660,
     661,   662,   663,   664,   665,   666,   667,   675,   707,   714,
     726,   733,   738,   743,   750,   761,   772,   780,   789,   797,
     811,   819,   828,   836,   847,   852,   857,   862,   871,   876
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "NUMBER", "NUMSTR", "FMTSTR",
  "VARIABLE_NAME", "FUNCTION_NAME", "ASK", "COMMENT", "CONTINUE", "DO",
  "ERASE", "FOR", "GO", "GOTO", "IF", "MODIFY", "QUIT", "RETURN", "SET",
  "TYPE", "WRITE", "LIBRARY", "CALL", "SAVE", "RUN", "ALL", "FABS", "FATN",
  "FCOS", "FEXP", "FITR", "FLOG", "FRAN", "FSGN", "FSIN", "FSQT", "FADC",
  "FDIS", "FDXS", "FCOM", "FNEW", "FIN", "FOUT", "VARLIST", "'\\n'", "';'",
  "'='", "','", "'('", "')'", "'$'", "'+'", "'-'", "'*'", "'/'", "'^'",
  "'['", "']'", "'<'", "'>'", "'%'", "'!'", "'#'", "':'", "$accept",
  "program", "line", "@1", "statements", "statement", "expression",
  "expression2", "e2op", "expression3", "term", "expression4", "unary_op",
  "function", "fn_0", "fn_1", "factor", "variable", "printlist",
  "printsep", "exprlist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,    10,    59,    61,
      44,    40,    41,    36,    43,    45,    42,    47,    94,    91,
      93,    60,    62,    37,    33,    35,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    67,    68,    68,    68,    68,    70,    69,    71,    71,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    73,    74,    74,    75,
      75,    76,    76,    77,    77,    77,    78,    78,    79,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      81,    81,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    83,    83,    83,
      83,    83,    83,    83,    84,    84,    85,    85,    85,    85,
      85,    85,    85,    85,    86,    86,    86,    86,    87,    87
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     3,     3,     0,     3,     1,     3,
       0,     2,     1,     2,     1,     2,     2,     2,     6,     8,
       1,     1,     2,     5,     7,     9,     3,     3,     3,     1,
       1,     4,     2,     2,     1,     2,     1,     1,     3,     1,
       1,     1,     3,     1,     1,     1,     1,     2,     1,     1,
       1,     3,     3,     3,     4,     4,     4,     4,     4,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     1,     4,     1,     2,     1,     2,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     6,     0,     2,    10,     1,     0,     3,     0,    12,
       0,    14,     0,    20,    21,     0,     0,    29,    30,     0,
       0,    34,     0,     7,     8,     5,     4,    79,    77,    78,
      90,    84,    62,    64,    66,    67,    70,    71,    60,    74,
      75,    73,    63,    68,    69,    65,    72,    61,    76,    94,
       0,    48,     0,     0,    92,    95,    96,    97,    86,    36,
      37,    41,     0,    46,    50,     0,    49,    80,    11,    88,
      13,    16,    15,     0,    22,     0,    17,     0,    33,    32,
      35,     0,     0,     0,    10,     0,     0,     0,     0,    39,
      40,     0,    43,    44,    45,     0,    47,     0,     0,     0,
       0,     0,     0,    91,    93,    87,    89,     0,     0,     0,
      26,    27,    28,     9,    98,     0,    81,    82,    83,    38,
      42,    51,     0,    52,     0,    53,     0,     0,     0,     0,
       0,     0,    31,     0,    85,    54,    55,    56,    57,    58,
      59,     0,    23,    99,    18,     0,     0,    24,    19,     0,
      25
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     4,    23,    24,    58,    59,    91,    60,
      95,    61,    62,    63,    64,    65,    66,    67,    68,    69,
     115
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -47
static const yytype_int16 yypact[] =
{
       2,   -47,     1,   -47,    -5,   -47,   -26,     2,   127,   -47,
      36,    -1,    39,   -47,    45,     0,    46,   -47,   -47,    39,
      63,   369,    -2,   -47,     4,   -47,   -47,   -47,   -47,   -47,
     -47,     5,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,
     369,   -47,   369,   369,   -47,   -47,   -47,   -47,   -47,   -10,
     -19,   -47,   412,   -47,   -31,   -25,   -47,   -47,   191,   -47,
     -47,   -47,   -47,     6,   -47,   369,   -47,     8,   -47,   191,
     -47,    50,    56,    57,    -5,   369,     9,     3,    10,   -47,
     -47,   369,   -47,   -47,   -47,   369,   -47,   283,   326,   240,
     369,   369,   369,   -47,   -47,   -47,   -47,   369,    13,   369,
     -47,   -47,   -47,   -47,   -47,    -9,   -47,   -47,   -47,   -19,
     -47,   -47,    19,   -47,    14,   -47,    11,    30,    15,    21,
      12,    80,   -47,   369,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   369,    35,   -47,    37,    85,   369,    40,   -47,    87,
     -47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -47,   -47,   102,   -47,    26,   -47,   -21,   -47,   -47,    20,
     -47,    22,   -47,    53,   -47,   -47,   -47,    23,    99,   -46,
     -47
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      80,     5,     6,    71,     8,     9,     1,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      97,    25,   106,    81,    82,    83,   100,    72,    98,    86,
      99,    87,    88,   106,   101,    73,   102,    92,    93,    94,
      70,   133,    77,   134,    89,    90,    31,   105,     7,    74,
      76,    75,    84,   110,   108,   107,    85,   109,   105,   111,
     112,   116,   141,   117,   114,   131,    27,    28,    29,    30,
      31,   135,   118,   137,   136,   139,   122,   124,   126,   127,
     128,   129,   138,   140,   142,   145,   130,   146,   132,   147,
     149,   150,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    26,
     113,   119,   143,    49,    50,    96,    78,   120,    51,    79,
     144,     0,    52,     0,    53,   148,    54,    55,    56,    57,
      27,    28,    29,    30,    31,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,     0,     0,     0,     0,    49,    50,     0,
       0,     0,    51,     0,     0,     0,    52,     0,    53,     0,
      54,    55,    56,    57,    27,    28,    29,   103,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,     0,     0,     0,
       0,    49,    50,    27,    28,    29,    51,    31,     0,     0,
      52,     0,    53,     0,   104,    55,    56,    57,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    27,    28,    29,     0,
      31,    50,     0,     0,     0,    51,     0,     0,     0,    52,
       0,    53,   125,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    27,
      28,    29,     0,    31,    50,   121,     0,     0,    51,     0,
       0,     0,    52,     0,    53,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    27,    28,    29,     0,    31,    50,     0,     0,
       0,    51,     0,     0,     0,    52,   123,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    27,    28,    29,     0,    31,
      50,     0,     0,     0,    51,     0,     0,     0,    52,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
       0,     0,     0,    50,     0,     0,     0,     0,     0,     0,
       0,    52,     0,    53
};

static const yytype_int16 yycheck[] =
{
      21,     0,     1,     4,     9,    10,     4,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      51,    47,    68,    25,    26,    27,    51,    28,    59,    50,
      61,    52,    53,    79,    59,    12,    61,    56,    57,    58,
       4,    50,    19,    52,    54,    55,     7,    68,    47,     4,
       4,    51,    48,     3,    75,    49,    51,    49,    79,     3,
       3,    52,    50,    60,    85,    52,     3,     4,     5,     6,
       7,    52,    62,    62,    60,    60,    97,    98,    99,   100,
     101,   102,    52,    62,     4,    50,   107,    50,   109,     4,
      50,     4,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     7,
      84,    91,   133,    50,    51,    62,    53,    95,    55,    20,
     141,    -1,    59,    -1,    61,   146,    63,    64,    65,    66,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    50,    51,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    59,    -1,    61,    -1,
      63,    64,    65,    66,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    -1,    -1,    -1,
      -1,    50,    51,     3,     4,     5,    55,     7,    -1,    -1,
      59,    -1,    61,    -1,    63,    64,    65,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,     3,     4,     5,    -1,
       7,    51,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,
      -1,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     3,
       4,     5,    -1,     7,    51,    52,    -1,    -1,    55,    -1,
      -1,    -1,    59,    -1,    61,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,     3,     4,     5,    -1,     7,    51,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    59,    60,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     3,     4,     5,    -1,     7,
      51,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    -1,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    -1,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    61
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,    68,    69,    70,     0,     1,    47,     9,    10,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    71,    72,    47,    69,     3,     4,     5,
       6,     7,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    50,
      51,    55,    59,    61,    63,    64,    65,    66,    73,    74,
      76,    78,    79,    80,    81,    82,    83,    84,    85,    86,
       4,     4,    28,    84,     4,    51,     4,    84,    53,    85,
      73,    25,    26,    27,    48,    51,    73,    73,    73,    54,
      55,    75,    56,    57,    58,    77,    80,    51,    59,    61,
      51,    59,    61,     6,    63,    73,    86,    49,    73,    49,
       3,     3,     3,    71,    73,    87,    52,    60,    62,    76,
      78,    52,    73,    60,    73,    62,    73,    73,    73,    73,
      73,    52,    73,    50,    52,    52,    60,    62,    52,    60,
      62,    50,     4,    73,    73,    50,    50,     4,    73,    50,
       4
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 169 "src/parse.y"
    { errline = (yyvsp[(1) - (1)].d); ;}
    break;

  case 7:
#line 170 "src/parse.y"
    {
    // there are only 3199 possible lines so we make an array that large
    // even though it ends up mostly empty. to convert the X.Y format, we
    // simply multiply by 100 to shift the decimal so that 3.10 is line 310
    // however, due to decimal conversion, 5.10 might end up as 5.099999...
    // and that would trunced to 5.09, so we have to round the result
	  interpreter_state.lines[(int)round((yyvsp[(1) - (3)].d) * 100)] = (yyvsp[(3) - (3)].l);
	;}
    break;

  case 8:
#line 183 "src/parse.y"
    {
	  (yyval.l) = lst_prepend(NULL, (yyvsp[(1) - (1)].statement));
  ;}
    break;

  case 9:
#line 188 "src/parse.y"
    {
    (yyval.l) = lst_prepend((yyvsp[(3) - (3)].l), (yyvsp[(1) - (3)].statement));
  ;}
    break;

  case 10:
#line 196 "src/parse.y"
    {
	  (yyval.statement) = NULL;
	;}
    break;

  case 11:
#line 201 "src/parse.y"
    {
    statement_t *new = make_statement(ASK);
    new->parms.input = (yyvsp[(2) - (2)].l);
    (yyval.statement) = new;
  ;}
    break;

  case 12:
#line 208 "src/parse.y"
    {
    statement_t *new = make_statement(COMMENT);
    new->parms.rem = yylval.s;
    (yyval.statement) = new;
  ;}
    break;

  case 13:
#line 215 "src/parse.y"
    {
    statement_t *new = make_statement(DO);
    new->parms._do = (yyvsp[(2) - (2)].d);
    (yyval.statement) = new;
      
    /* static analyzer */
    linenum_do_totals++;
    linenum_constants_total++;
    if ((yyvsp[(2) - (2)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(2) - (2)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  ;}
    break;

  case 14:
#line 233 "src/parse.y"
    {
    statement_t *new = make_statement(ERASE);
    new->parms.erase.mode = 0;
    (yyval.statement) = new;
  ;}
    break;

  case 15:
#line 240 "src/parse.y"
    {
    statement_t *new = make_statement(ERASE);
    new->parms.erase.mode = 3;
    (yyval.statement) = new;
  ;}
    break;

  case 16:
#line 247 "src/parse.y"
    {
    statement_t *new = make_statement(ERASE);
    new->parms.erase.target = (yyvsp[(2) - (2)].d);
    if (fabs((yyvsp[(2) - (2)].d) - trunc((yyvsp[(2) - (2)].d))) < 0.00001)
      new->parms.erase.mode = 2;
    else
      new->parms.erase.mode = 1;
    (yyval.statement) = new;
  ;}
    break;

  case 17:
#line 258 "src/parse.y"
    {
    statement_t *new = make_statement(MODIFY);
    new->parms.modify_line = (yyvsp[(2) - (2)].d);
    (yyval.statement) = new;
  ;}
    break;

  case 18:
#line 265 "src/parse.y"
    {
	  statement_t *new = make_statement(FOR);
	  new->parms._for.variable = (yyvsp[(2) - (6)].variable);
	  new->parms._for.begin = (yyvsp[(4) - (6)].expression);
	  new->parms._for.end = (yyvsp[(6) - (6)].expression);
	  new->parms._for.step = NULL;
	  (yyval.statement) = new;
    
    /* static analyser */
    for_loops_total++;
    for_loops_step_1++;
	;}
    break;

  case 19:
#line 279 "src/parse.y"
    {
	  statement_t *new = make_statement(FOR);
	  new->parms._for.variable = (yyvsp[(2) - (8)].variable);
	  new->parms._for.begin = (yyvsp[(4) - (8)].expression);
	  new->parms._for.step = (yyvsp[(6) - (8)].expression);
    new->parms._for.end = (yyvsp[(8) - (8)].expression);
	  (yyval.statement) = new;
	
    /* static analyser - consider anything with a STEP special even if it is a 1 */
    for_loops_total++;
  ;}
    break;

  case 20:
#line 292 "src/parse.y"
    {
    statement_t *new = make_statement(GOTO);
    new->parms.go = 0;
    (yyval.statement) = new;
  ;}
    break;

  case 21:
#line 299 "src/parse.y"
    {
    statement_t *new = make_statement(GOTO);
    new->parms.go = 0;
    (yyval.statement) = new;
    
    linenum_go_totals++;
  ;}
    break;

  case 22:
#line 308 "src/parse.y"
    {
    statement_t *new = make_statement(GOTO);
    new->parms.go = (yyvsp[(2) - (2)].d);
    (yyval.statement) = new;
    
    /* static analyzer */
    linenum_go_totals++;
    linenum_constants_total++;
    if ((yyvsp[(2) - (2)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(2) - (2)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  ;}
    break;

  case 23:
#line 326 "src/parse.y"
    {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = (yyvsp[(3) - (5)].expression);
    new->parms._if.less_line = (yyvsp[(5) - (5)].d);
    (yyval.statement) = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total++;
    if ((yyvsp[(5) - (5)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(5) - (5)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  ;}
    break;

  case 24:
#line 345 "src/parse.y"
    {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = (yyvsp[(3) - (7)].expression);
    new->parms._if.less_line = (yyvsp[(5) - (7)].d);
    new->parms._if.zero_line = (yyvsp[(7) - (7)].d);
    (yyval.statement) = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total += 2;
    if ((yyvsp[(5) - (7)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(5) - (7)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ((yyvsp[(7) - (7)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(7) - (7)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  ;}
    break;

  case 25:
#line 372 "src/parse.y"
    {
    statement_t *new = make_statement(IF);
    new->parms._if.condition = (yyvsp[(3) - (9)].expression);
    new->parms._if.less_line = (yyvsp[(5) - (9)].d);
    new->parms._if.zero_line = (yyvsp[(7) - (9)].d);
    new->parms._if.more_line = (yyvsp[(9) - (9)].d);
    (yyval.statement) = new;
    
    /* static analyzer */
    linenum_then_go_totals++;
    linenum_constants_total += 3;
    if ((yyvsp[(5) - (9)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(5) - (9)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ((yyvsp[(7) - (9)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(7) - (9)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
    if ((yyvsp[(9) - (9)].d) == errline) {
      linenum_same_line++;
    } else if ((yyvsp[(9) - (9)].d) > errline) {
      linenum_forwards++;
    } else {
      linenum_backwards++;
    }
  ;}
    break;

  case 26:
#line 407 "src/parse.y"
    {
    statement_t *new = make_statement(LIBRARY);
    new->parms.library.filename = (yyvsp[(3) - (3)].s);
    new->parms.library.action = 1;
    (yyval.statement) = new;
  ;}
    break;

  case 27:
#line 415 "src/parse.y"
    {
    statement_t *new = make_statement(LIBRARY);
    new->parms.library.filename = (yyvsp[(3) - (3)].s);
    new->parms.library.action = 0;
    (yyval.statement) = new;
  ;}
    break;

  case 28:
#line 422 "src/parse.y"
    {
    statement_t *new = make_statement(LIBRARY);
    new->parms.library.filename = (yyvsp[(3) - (3)].s);
    new->parms.library.action = 2;
    (yyval.statement) = new;
  ;}
    break;

  case 29:
#line 429 "src/parse.y"
    {
    statement_t *new = make_statement(QUIT);
    (yyval.statement) = new;
  ;}
    break;

  case 30:
#line 435 "src/parse.y"
    {
	  statement_t *new = make_statement(RETURN);
	  (yyval.statement) = new;
	;}
    break;

  case 31:
#line 441 "src/parse.y"
    {
    statement_t *new = make_statement(SET);
    new->parms.set.variable = (yyvsp[(2) - (4)].variable);
    new->parms.set.expression = (yyvsp[(4) - (4)].expression);
    (yyval.statement) = new;
    
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
  ;}
    break;

  case 32:
#line 461 "src/parse.y"
    {
    statement_t *new = make_statement(TYPE);
    new->parms.print = (yyvsp[(2) - (2)].l);
    (yyval.statement) = new;
  ;}
    break;

  case 33:
#line 468 "src/parse.y"
    {
	  statement_t *new = make_statement(VARLIST);
	  (yyval.statement) = new;
	;}
    break;

  case 34:
#line 474 "src/parse.y"
    {
    statement_t *new = make_statement(WRITE);
    new->parms.write_spec = NULL;
    (yyval.statement) = new;
  ;}
    break;

  case 35:
#line 481 "src/parse.y"
    {
    statement_t *new = make_statement(WRITE);
    new->parms.write_spec = (yyvsp[(2) - (2)].expression);
    (yyval.statement) = new;
  ;}
    break;

  case 38:
#line 518 "src/parse.y"
    {
	  expression_t *new = make_operator(2, (yyvsp[(2) - (3)].i));
	  new->parms.op.p[0] = (yyvsp[(1) - (3)].expression);
	  new->parms.op.p[1] = (yyvsp[(3) - (3)].expression);
	  (yyval.expression) = new;
    
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
	;}
    break;

  case 39:
#line 537 "src/parse.y"
    { (yyval.i) = '+'; ;}
    break;

  case 40:
#line 538 "src/parse.y"
    { (yyval.i) = '-'; ;}
    break;

  case 42:
#line 545 "src/parse.y"
    {
	  expression_t *new = make_operator(2, (yyvsp[(2) - (3)].i));
	  new->parms.op.p[0] = (yyvsp[(1) - (3)].expression);
	  new->parms.op.p[1] = (yyvsp[(3) - (3)].expression);
	  (yyval.expression) = new;
	;}
    break;

  case 43:
#line 553 "src/parse.y"
    { (yyval.i) = '*'; ;}
    break;

  case 44:
#line 554 "src/parse.y"
    { (yyval.i) = '/'; ;}
    break;

  case 45:
#line 555 "src/parse.y"
    { (yyval.i) = '^'; ;}
    break;

  case 47:
#line 561 "src/parse.y"
    {
	  expression_t *new = make_operator(1, (yyvsp[(1) - (2)].i));
	  new->parms.op.p[0] = (yyvsp[(2) - (2)].expression);
	  (yyval.expression) = new;
	;}
    break;

  case 48:
#line 569 "src/parse.y"
    { (yyval.i) = '-'; ;}
    break;

  case 50:
#line 577 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (1)].i));
    (yyval.expression) = new;
  ;}
    break;

  case 51:
#line 584 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (3)].i));
    (yyval.expression) = new;
  ;}
    break;

  case 52:
#line 590 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (3)].i));
    (yyval.expression) = new;
  ;}
    break;

  case 53:
#line 596 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (3)].i));
    (yyval.expression) = new;
  ;}
    break;

  case 54:
#line 603 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (4)].i));
    new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
    (yyval.expression) = new;
  ;}
    break;

  case 55:
#line 610 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (4)].i));
    new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
    (yyval.expression) = new;
  ;}
    break;

  case 56:
#line 617 "src/parse.y"
    {
    expression_t *new = make_operator(0, (yyvsp[(1) - (4)].i));
    new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
    (yyval.expression) = new;
  ;}
    break;

  case 57:
#line 624 "src/parse.y"
    {
    expression_t *new = make_operator(1, (yyvsp[(1) - (4)].i));
    new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
    (yyval.expression) = new;
  ;}
    break;

  case 58:
#line 631 "src/parse.y"
    {
    expression_t *new = make_operator(1, (yyvsp[(1) - (4)].i));
    new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
    (yyval.expression) = new;
  ;}
    break;

  case 59:
#line 638 "src/parse.y"
    {
	  expression_t *new = make_operator(1, (yyvsp[(1) - (4)].i));
	  new->parms.op.p[0] = (yyvsp[(3) - (4)].expression);
	  (yyval.expression) = new;
	;}
    break;

  case 60:
#line 647 "src/parse.y"
    { (yyval.i) = FRAN; ;}
    break;

  case 61:
#line 648 "src/parse.y"
    { (yyval.i) = FIN; ;}
    break;

  case 62:
#line 653 "src/parse.y"
    { (yyval.i) = FABS; ;}
    break;

  case 63:
#line 654 "src/parse.y"
    { (yyval.i) = FADC; ;}
    break;

  case 64:
#line 655 "src/parse.y"
    { (yyval.i) = FATN; ;}
    break;

  case 65:
#line 656 "src/parse.y"
    { (yyval.i) = FCOM; ;}
    break;

  case 66:
#line 657 "src/parse.y"
    { (yyval.i) = FCOS; ;}
    break;

  case 67:
#line 658 "src/parse.y"
    { (yyval.i) = FEXP; ;}
    break;

  case 68:
#line 659 "src/parse.y"
    { (yyval.i) = FDIS; ;}
    break;

  case 69:
#line 660 "src/parse.y"
    { (yyval.i) = FDXS; ;}
    break;

  case 70:
#line 661 "src/parse.y"
    { (yyval.i) = FITR; ;}
    break;

  case 71:
#line 662 "src/parse.y"
    { (yyval.i) = FLOG; ;}
    break;

  case 72:
#line 663 "src/parse.y"
    { (yyval.i) = FNEW; ;}
    break;

  case 73:
#line 664 "src/parse.y"
    { (yyval.i) = FSQT; ;}
    break;

  case 74:
#line 665 "src/parse.y"
    { (yyval.i) = FSGN; ;}
    break;

  case 75:
#line 666 "src/parse.y"
    { (yyval.i) = FSIN; ;}
    break;

  case 76:
#line 667 "src/parse.y"
    { (yyval.i) = FOUT; ;}
    break;

  case 77:
#line 676 "src/parse.y"
    {
	  expression_t *new = make_expression(number);
	  new->parms.number = (yyvsp[(1) - (1)].d);
	  (yyval.expression) = new;
    
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
    /* everything else is a float - NOTE: in FOCAL, this does include line numbers */
    else {
        numeric_constants_float++;
    }
	;}
    break;

  case 78:
#line 708 "src/parse.y"
    {
    expression_t *new = make_expression(numstr);
    new->parms.string = (yyvsp[(1) - (1)].s);
    (yyval.expression) = new;
  ;}
    break;

  case 79:
#line 715 "src/parse.y"
    {
	  expression_t *new = make_expression(string);
	  new->parms.string = (yyvsp[(1) - (1)].s);
	  (yyval.expression) = new;
      
    /* static analyzer code */
    size_t len = strlen((yyvsp[(1) - (1)].s));
    string_constants_total++;
    if (len > string_constants_max) string_constants_max = (int)len;
  ;}
    break;

  case 80:
#line 727 "src/parse.y"
    {
    expression_t *new = make_expression(variable);
    new->parms.variable = (yyvsp[(1) - (1)].variable);
    (yyval.expression) = new;
  ;}
    break;

  case 81:
#line 734 "src/parse.y"
    {
    (yyval.expression) = (yyvsp[(2) - (3)].expression);
  ;}
    break;

  case 82:
#line 739 "src/parse.y"
    {
    (yyval.expression) = (yyvsp[(2) - (3)].expression);
  ;}
    break;

  case 83:
#line 744 "src/parse.y"
    {
    (yyval.expression) = (yyvsp[(2) - (3)].expression);
  ;}
    break;

  case 84:
#line 751 "src/parse.y"
    {
	  variable_t *new = malloc(sizeof(*new));
	  new->name = (yyvsp[(1) - (1)].s);
	  new->subscripts = NULL;
    (yyval.variable) = new;
    
    /* add it to the interpreter's variable list for the analyizer*/
    insert_variable(new);
	;}
    break;

  case 85:
#line 762 "src/parse.y"
    {
    variable_t *new = malloc(sizeof(*new));
    new->name = (yyvsp[(1) - (4)].s);
    new->subscripts = (yyvsp[(3) - (4)].l);
    (yyval.variable) = new;

    insert_variable(new);
  ;}
    break;

  case 86:
#line 773 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = (yyvsp[(1) - (1)].expression);
    new->separator = 0;
    (yyval.l) = lst_prepend(NULL, new);
  ;}
    break;

  case 87:
#line 781 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = (yyvsp[(2) - (2)].expression);
    new->separator = 0;
    (yyval.l) = lst_append((yyvsp[(1) - (2)].l), new);
  ;}
    break;

  case 88:
#line 790 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->separator = (yyvsp[(1) - (1)].i);
    (yyval.l) = lst_prepend(NULL, new);
  ;}
    break;

  case 89:
#line 798 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->separator = (yyvsp[(2) - (2)].i);
    (yyval.l) = lst_append((yyvsp[(1) - (2)].l), new);
  ;}
    break;

  case 90:
#line 812 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = (yyvsp[(1) - (1)].s);
    (yyval.l) = lst_append(NULL, new);
  ;}
    break;

  case 91:
#line 820 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = (yyvsp[(2) - (2)].s);
    (yyval.l) = lst_append((yyvsp[(1) - (2)].l), new);
  ;}
    break;

  case 92:
#line 829 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = "-1";
    (yyval.l) = lst_prepend(NULL, new);
  ;}
    break;

  case 93:
#line 837 "src/parse.y"
    {
    printitem_t *new = malloc(sizeof(*new));
    new->expression = NULL;
    new->format = "-1";
    (yyval.l) = lst_prepend((yyvsp[(1) - (2)].l), new);
  ;}
    break;

  case 94:
#line 848 "src/parse.y"
    {
	  (yyval.i) = ','; // this is the main separator, comparable to semicolon in BASIC
	;}
    break;

  case 95:
#line 853 "src/parse.y"
    {
	  (yyval.i) = '!'; // inserts a cr/lf
	;}
    break;

  case 96:
#line 858 "src/parse.y"
    {
    (yyval.i) = '#'; // inserts a cr
  ;}
    break;

  case 97:
#line 863 "src/parse.y"
    {
    (yyval.i) = ':'; // inserts a tab
  ;}
    break;

  case 98:
#line 872 "src/parse.y"
    {
	  (yyval.l) = lst_prepend(NULL, (yyvsp[(1) - (1)].expression));
	;}
    break;

  case 99:
#line 877 "src/parse.y"
    {
	  (yyval.l) = lst_append((yyvsp[(1) - (3)].l), (yyvsp[(3) - (3)].expression));
	;}
    break;


/* Line 1267 of yacc.c.  */
#line 2541 "parse.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 882 "src/parse.y"


