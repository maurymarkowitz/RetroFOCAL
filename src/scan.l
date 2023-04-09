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

 // used to track where we are in the statement so we can have keywords
 // and variables with the same name
%s KEYWORD_FOUND

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
<INITIAL>{
C|COMMENT|CONTINUE.* { yylval.s = str_new(str_copy(yytext + 3, yyleng - 3)); return COMMENT; } // manual lists CONTINUE separately, but its a comment
A|ASK     { BEGIN(KEYWORD_FOUND); return ASK; }     // INPUT
D|DO      { BEGIN(KEYWORD_FOUND); return DO; }      // combines GOTO and GOSUB
F|FOR     { BEGIN(KEYWORD_FOUND); return FOR; }     // one-line only, no NEXT
I|IF      { BEGIN(KEYWORD_FOUND); return IF; }      // branches only
Q|QUIT    { BEGIN(KEYWORD_FOUND); return QUIT; }    // END/STOP/BYE
R|RETURN  { BEGIN(KEYWORD_FOUND); return RETURN; }  // optional RETURNs at end of group anyway
S|SET     { BEGIN(KEYWORD_FOUND); return SET; }     // LET
T|TYPE    { BEGIN(KEYWORD_FOUND); return TYPE; }    // PRINT

 /* non-program statements (mostly) */
E|ERASE   { BEGIN(KEYWORD_FOUND); return ERASE; }   // erases lines of source, but also double-duty as CLR to reset variables
G|GOTO    { BEGIN(KEYWORD_FOUND); return GOTO; }    // RUN, optional line number
M|MODIFY  { BEGIN(KEYWORD_FOUND); return MODIFY; }  // edits a single line
W|WRITE   { BEGIN(KEYWORD_FOUND); return WRITE; }   // LIST
}

 /* the following are only valid after one of the keyworda above */
<KEYWORD_FOUND>{
 /* math functions */
FABS		{ return FABS; }
FATN		{ return FATN; }
FCOS		{ return FCOS; }
FEXP    { return FEXP; }
FITR    { return FITR; }
FLOG    { return FLOG; }
FRAN    { return FRAN; }
FSGN    { return FSGN; }
FSIN    { return FSIN; }
FSQT    { return FSQT; }

 /* other functions */
FADC    { return FADC; }
FDIS    { return FDIS; }
FDXS    { return FDXS; }

 /* these are listed in the 1970 programming languages manual, but are not explained */
FNEW    { return FNEW; } // like a BASIC SYS or CALL, calls a machine-code routine
FCOM    { return FCOM; } // UW-FOCAL explains that this is used to access core, to share data between programs

 /* FOCAL-71 additions */
FIN     { return FIN; }
FOUT    { return FOUT; }

 /* various operators and punctuation */
[\?\%!:,()\^=+\-*/\<\>\&]     { return yytext[0]; }

 /* variable references, only first two characters are used but we save them all. F is not allowed. */
 /* NOTE: underscore is supported here but unlikely to have been used in actual code. */
[A-EG-Za-eg-z][A-Za-z0-9_\"\'"]* {
             yylval.s = str_new(yytext);
             return VARIABLE_NAME;
           }

 /* string constants */
\"[^"]*\"   {
             yytext[strlen(yytext) - 1] = '\0';   // cut off the trailing quote
             yylval.s = str_new(yytext + 1);      // and the leading one
             return STRING;
           }
 
 /* FOCAL has a second number format used for string inputs, it starts with a 0 like 0YES */
0[A-Za-z][A-Za-z0-9]* {
              yylval.s = str_new(yytext);
              return NUMSTR;
            }

} // back out of KEYWORD_FOUND, the following apply to everything

 /* other numeric constants and line numbers */
[0-9]*[0-9.][0-9]*([Ee][-+]?[0-9]+)? {
              yylval.d = strtod(yytext, NULL);
              return NUMBER;
            }

 /* semis outside strings are used to trigger statement-start */
;         { BEGIN(INITIAL); return ';'; }

 /* preserve line ends */
\n        { BEGIN(INITIAL); return '\n'; }

 /* eat other whitespace */
[ \t\r\l]   {  }

%%
