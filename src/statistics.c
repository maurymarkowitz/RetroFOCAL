/* Statistics for RetroFOCAL
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

#include "statistics.h"

#include "parse.h"

/* declarations of the externs from the header */
int variables_total = 0;
int variables_default = 0;
int variables_int = 0;
int variables_float = 0;
int variables_double = 0;
int variables_string = 0;
int numeric_constants_total = 0;
int numeric_constants_float = 0;
int numeric_constants_zero = 0;
int numeric_constants_one = 0;
int string_constants_total = 0;
int string_constants_max = 0;
int linenum_constants_total = 0;
int linenum_forwards = 0;
int linenum_backwards = 0;
int linenum_same_line = 0;
int linenum_do_totals = 0;
int linenum_then_go_totals = 0;
int linenum_go_totals = 0;
int for_loops_total = 0;
int for_loops_step_1 = 0;
int increments = 0;
int decrements = 0;
int assign_zero = 0;
int assign_one = 0;
int assign_other = 0;

/* prints out various statistics from the static code,
 or if the write_stats flag is on, writes them to a file */
void print_statistics()
{
  int lines_total, line_min, line_max;
  
  // start with line number stats
  lines_total = 0;
  line_min = MAXLINE + 1;
  line_max = -1;
  // just look for any entry in the statement array with a non-null entry1
  for(int i = 0; i < MAXLINE; i++) {
    if (interpreter_state.lines[i] != NULL) {
      lines_total++;
      if (i > line_max) line_max = i;
      if (i < line_min) line_min = i;
    }
  }
  
  // exit if there's no program
  if (lines_total == 0) {
    printf("\nNO PROGRAM TO EXAMINE\n\n");
    return;
  }
  
  // since the statements are run together as one long list, it's
  // easy to print out the total number, but not so easy to print
  // out the number per line. so this code checks each node to see
  // if the ->next is the first item on the next line
  int stmts_max = 0, diff = 0, next_num;
  list_t *next_line;
  list_t *start = interpreter_state.lines[interpreter_state.first_line];
  
  for(int i = interpreter_state.first_line; i < MAXLINE - 1; i++) {
    // get the next line's statements, and continue if its empty
    list_t *this_line = interpreter_state.lines[i];
    if (interpreter_state.lines[i] == NULL)
      continue;
    
    // now find the next non-empty line
    next_num = i + 1; // note to me: no, you can't i++ here!
    while ((next_num < MAXLINE) && (interpreter_state.lines[next_num] == NULL))
      next_num++;
    
    // if we ran off the end of the list, exit
    if (next_num > MAXLINE - 1)
      break;
    
    // otherwise we found the next line
    next_line = interpreter_state.lines[next_num];
    
    // now count the number of statements between them
    diff = lst_index_of_node(start, next_line) - lst_index_of_node(start, this_line);
    if (diff > stmts_max)
      stmts_max = diff;
  }
  
  // the total number of statements is simply the length of every statement up to the last line
  int stmts_total = lst_length(interpreter_state.lines[line_max]);
  
  // variables - no string variables so this is easy
  int num_total = lst_length(interpreter_state.variable_values);
  
  // output to screen if selected
  if (print_stats) {
    printf("\nRUN TIME: %g\n", (double)(end_time.tv_usec - start_time.tv_usec) / 1000000 + (double)(end_time.tv_sec - start_time.tv_sec));
    printf("CPU TIME: %g\n", ((double) (end_ticks - start_ticks)) / CLOCKS_PER_SEC);
    
    printf("\nLINE NUMBERS\n\n");
    printf("  total: %i\n", lines_total);
    printf("  first: %2.2f\n", ((double)line_min / 100.0));
    printf("   last: %2.2f\n", ((double)line_max / 100.0));
    
    printf("\nSTATEMENTS\n\n");
    printf("  total: %i\n", lst_length(interpreter_state.lines[line_max]));
    printf("average: %2.2f\n", (double)stmts_total/(double)lines_total);
    printf("    max: %i\n", stmts_max);
    
    printf("\nVARIABLES\n\n");
    printf("  total: %i\n",num_total);
    
    printf("\nNUMERIC CONSTANTS\n\n");
    printf("  total: %i\n",numeric_constants_total);
    printf("non-int: %i\n",numeric_constants_float);
    printf("    int: %i\n",numeric_constants_total - numeric_constants_float);
    printf("  zeros: %i\n",numeric_constants_zero);
    printf("   ones: %i\n",numeric_constants_one);

    printf("\nSTRING CONSTANTS\n\n");
    printf("  total: %i\n",string_constants_total);
    printf("biggest: %i\n",string_constants_max);
    
    printf("\nBRANCHES\n\n");
    printf("  total: %i\n",linenum_constants_total);
    printf("    dos: %i\n",linenum_do_totals);
    printf("  gotos: %i\n",linenum_go_totals);
    printf("  thens: %i\n",linenum_then_go_totals);
    printf("forward: %i\n",linenum_forwards);
    printf("bckward: %i\n",linenum_backwards);
    printf("same ln: %i\n",linenum_same_line);
    
    printf("\nOTHER BITS\n\n");
    printf(" asgn 0: %i\n",assign_zero);
    printf(" asgn 1: %i\n",assign_one);
    printf(" asgn x: %i\n",assign_other);
    printf("   FORs: %i\n",for_loops_total);
    printf(" step 1: %i\n",for_loops_step_1);
    printf("   incs: %i\n",increments);
    printf("   decs: %i\n",decrements);
  }
  /* and/or the file if selected */
  if (write_stats) {
    //check that the file name is reasonable, and then try to open it
    FILE* fp = fopen(stats_file, "w+");
    if (!fp) return;
    
    double tu = (double)(end_time.tv_usec - start_time.tv_usec);
    double ts = (double)(end_time.tv_sec - start_time.tv_sec);
    fprintf(fp, "RUN TIME: %g\n", tu / 1000000 + ts);
    fprintf(fp, "CPU TIME,%g\n", ((double) (end_ticks - start_ticks)) / CLOCKS_PER_SEC);
    
    fprintf(fp, "LINE NUMBERS,total,%i\n", lines_total);
    fprintf(fp, "LINE NUMBERS,first,%2.2f\n", ((double)line_min / 100.0));
    fprintf(fp, "LINE NUMBERS,last,%2.2f\n", ((double)line_max / 100.0));
    
    fprintf(fp, "STATEMENTS,total,%i\n", lst_length(interpreter_state.lines[line_max]));
    fprintf(fp, "STATEMENTS,average,%g\n", (double)stmts_total/(double)lines_total);
    fprintf(fp, "STATEMENTS,max/ln,%i\n", stmts_max);
    
    fprintf(fp, "VARIABLES,total,%i\n",num_total);
    
    fprintf(fp, "NUMERIC CONSTANTS,total,%i\n",numeric_constants_total);
    fprintf(fp, "NUMERIC CONSTANTS,non-int,%i\n",numeric_constants_float);
    fprintf(fp, "NUMERIC CONSTANTS,int,%i\n",numeric_constants_total - numeric_constants_float);
    fprintf(fp, "NUMERIC CONSTANTS,zeros,%i\n",numeric_constants_zero);
    fprintf(fp, "NUMERIC CONSTANTS,ones,%i\n",numeric_constants_one);
    
    fprintf(fp, "STRING CONSTANTS,total,%i\n",string_constants_total);
    fprintf(fp, "STRING CONSTANTS,biggest,%i\n",string_constants_max);
    
    fprintf(fp, "BRANCHES,total,%i\n",linenum_constants_total);
    fprintf(fp, "BRANCHES,dos,%i\n",linenum_do_totals);
    fprintf(fp, "BRANCHES,gotos,%i\n",linenum_go_totals);
    fprintf(fp, "BRANCHES,thens,%i\n",linenum_then_go_totals);
    fprintf(fp, "BRANCHES,forward,%i\n",linenum_forwards);
    fprintf(fp, "BRANCHES,backward,%i\n",linenum_backwards);
    fprintf(fp, "BRANCHES,same line,%i\n",linenum_same_line);
    
    fprintf(fp, "OTHER,ASSIGN 0: %i\n",assign_zero);
    fprintf(fp, "OTHER,ASSIGN 1: %i\n",assign_one);
    fprintf(fp, "OTHER,ASSIGN OTHER: %i\n",assign_other);
    fprintf(fp, "OTHER,FORs: %i\n",for_loops_total);
    fprintf(fp, "OTHER,FORs step 1: %i\n",for_loops_step_1);
    fprintf(fp, "OTHER,incs: %i\n",increments);
    fprintf(fp, "OTHER,decs: %i\n",decrements);
    
    fclose(fp);
  }
}

