%{
/* Lexical analyser for RetroFOCAL
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
#include "strng.h"
#include "parse.h"

%}

%option noyywrap
%option caseless

%%

 /* end of file */
"\032"  {
          yyterminate();
        }
<<EOF>> {
          yyterminate();
        }

 /* statements */
 /* COMMENT statements are odd because they consist of a token and a line of text, but
    the rest of the line is part of the statement. If you want to keep the COMMENT part as
    a separate token, this is the simplest solution. See:
    https://stackoverflow.com/questions/59117309/rest-of-line-in-bison/59122569#59122569
  */
COMMENT.*   { yylval.s = str_new(str_copy(yytext + 3, yyleng - 3)); return COMMENT; }
CONTINUE.*  { yylval.s = str_new(str_copy(yytext + 3, yyleng - 3)); return REM; } // manual lists this separately, do the same for now
DO      { return DO; }      // combines GOTO and GOSUB
ERASE   { return ERASE; }   // erases lines of source
FOR     { return FOR; }     // FOR loops, one-line only
GO      { return GO; }      // RUN
GOTO    { return GOTO; }    // RUN at line number
IF      { return IF; }      // branches only
MODIFY  { return MODIFY; }  // edits a single line
QUIT    { return QUIT; }    // END/STOP/BYE
RETURN  { return RETURN; }
SET     { return SET; }     // LET
TYPE    { return TYPE; }    // PRINT
WRITE   { return WRITE; }   // LIST

 /* math functions */
FABS		{ return FABS; }
FATN		{ return FATN; }
FCOS		{ return FCOS; }
FEXP    { return FEXP; }
FITR    { return FITR; }
FLOG    { return FLOG; }
FRAN    { return FRAN; }
FSGN    { return FSGN; }
FSQR    { return FSQR; }

 /* other functions */
FADC    { return FADC; }
FDIS    { return FDIS; }
FDXS    { return FDXS; }

 /* FOCAL-71 additions */
FIN     { return FIN; }
FOUT    { return FOUT; }

 /* various operators and punctuation */
[:,;()\^=+\-*/\<\>\&]     { return yytext[0]; }

 /* line numbers */
[0-9]?[0-9.][0-9][0-9] {
              yylval.d = strtod(yytext, NULL);
              return LINENUMBER;
            }

 /* other numeric constants */
[0-9]*[0-9.][0-9]*([Ee][-+]?[0-9]+)? {
              yylval.d = strtod(yytext, NULL);
              return NUMBER;
            }

 /* variable references */
 /* currently set to allow only one or two chars */
[A-Za-z][A-Za-z_]?{
              yylval.s = str_new(yytext);
              return VARIABLE_NAME;
            }

 /* string constants */
\"[^"]*\"   {
              yytext[strlen(yytext) - 1] = '\0';
              yylval.s = str_new(yytext + 1);
              return STRING;
            }

 /* preserve line ends */
[\n]        { return '\n'; }

 /* eat other whitespace */
[ \t\r\l]   {  }

%%
