/* Interactive CLI for RetroFOCAL
 Copyright (C) 2024 Maury Markowitz

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
#include "parse.h"
#include "io.h"
#include "strng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

/* External declarations for the parser and lexer */
extern int yyparse(void);
extern void yyrestart(FILE *input_file);
extern FILE *yyin;
extern char *yytext;
extern int yylineno;
/* Flex functions for string-based scanning - YY_BUFFER_STATE is an opaque type defined by flex */
extern void *yy_scan_string(const char *str);
extern void yy_delete_buffer(void *buffer);
extern char *cli_prompt;
extern void delete_variables(void);

/* Helper to parse a line number from the start of input */
static int parse_line_number(const char *line, char **rest)
{
  /* Skip leading whitespace */
  const char *p = line;
  while (*p && isspace(*p))
    p++;
  
  /* Try to parse a number */
  char *endptr;
  double num = strtod(p, &endptr);
  
  /* If we got a valid number and it's followed by whitespace, period, or null, it's a line number */
  if (endptr > p && (isspace(*endptr) || *endptr == '\0' || *endptr == '.')) {
    *rest = endptr;
    return (int)round(num * 100);  /* Convert FOCAL line format (multiply by 100) */
  }
  
  return -1;  /* No line number found */
}

static bool handle_erase_cli_command(const char *input_line)
{
  const char *p = input_line;
  while (*p && isspace((unsigned char)*p))
    p++;

  if (strncasecmp(p, "ERASE", 5) != 0 || (p[5] && !isspace((unsigned char)p[5])))
    return false;

  p += 5;
  while (*p && isspace((unsigned char)*p))
    p++;

  if (*p == '\0') {
    delete_variables();
    return true;
  }

  if (strncasecmp(p, "ALL", 3) == 0 && (p[3] == '\0' || isspace((unsigned char)p[3]))) {
    for (int i = 0; i < MAXLINE; i++)
      interpreter_state.lines[i] = NULL;
    interpreter_post_parse();
    return true;
  }

  char *endptr;
  double num = strtod(p, &endptr);
  if (endptr == p)
    return false;

  while (*endptr && isspace((unsigned char)*endptr))
    endptr++;
  if (*endptr != '\0')
    return false;

  if (num < 0) {
    fprintf(stderr, "Invalid ERASE target.\n");
    return true;
  }

  if (fabs(num - trunc(num)) < 0.00001) {
    int group = (int)trunc(num);
    int start = group * 100;
    if (start >= MAXLINE) {
      fprintf(stderr, "Invalid ERASE group.\n");
      return true;
    }
    for (int i = start; i < MAXLINE && i < start + 100; i++)
      interpreter_state.lines[i] = NULL;
  } else {
    int index = (int)round(num * 100);
    if (index < 0 || index >= MAXLINE) {
      fprintf(stderr, "Invalid ERASE target.\n");
      return true;
    }
    interpreter_state.lines[index] = NULL;
  }

  interpreter_post_parse();
  return true;
}

/* Process a line of input in the CLI */
static void process_cli_line(const char *input_line)
{
  if (handle_erase_cli_command(input_line))
    return;
  char *rest = NULL;
  int line_num = parse_line_number(input_line, &rest);
  
  /* Skip whitespace and period after the line number */
  while (rest && *rest && (isspace(*rest) || *rest == '.'))
    rest++;
  if (line_num >= 0) {
    /* This is a line edit: either delete or store */
    if (!rest || *rest == '\0') {
      /* Just a line number - delete the line */
      if (line_num >= 0 && line_num < MAXLINE) {
        interpreter_state.lines[line_num] = NULL;
      }
    } else {
      /* Line number followed by code - parse and store the line */
      char statement_with_line[512];
      snprintf(statement_with_line, sizeof(statement_with_line), "%s\n", input_line);
      
      /* Parse this as a program line using string-based scanning */
      void *buffer = yy_scan_string(statement_with_line);
      yyparse();
      yy_delete_buffer(buffer);
    }
  } else {
    /* No line number - this is immediate-mode execution */
    char statement_with_line[512];
    snprintf(statement_with_line, sizeof(statement_with_line), "0.00 %s\n", input_line);
    
    /* Parse this line into the current program storage using string-based scanning */
    void *buffer = yy_scan_string(statement_with_line);
    yyparse();
    yy_delete_buffer(buffer);
    
    /* Check if this is a QUIT statement - if so, set a flag to exit CLI */
    bool should_exit_cli = false;
    if (interpreter_state.lines[0] != NULL && interpreter_state.lines[0]->data != NULL) {
      statement_t *stmt = (statement_t *)interpreter_state.lines[0]->data;
      if (stmt->type == QUIT) {
        should_exit_cli = true;
      }
    }
    
    /* Execute the immediate line in the context of the existing program. */
    /* Save the current first_line_index to restore after execution */
    int saved_first_line_index = interpreter_state.first_line_index;
    
    interpreter_state.current_statement = interpreter_state.lines[0];
    interpreter_state.running_state = 1;
    interpreter_run();

    /* Restore the saved first_line_index (needed for GO statements in immediate mode) */
    interpreter_state.first_line_index = saved_first_line_index;

    /* Remove the temporary line and restore line links */
    if (interpreter_state.lines[0] != NULL) {
      lst_free(interpreter_state.lines[0]);
      interpreter_state.lines[0] = NULL;
    }
    interpreter_post_parse();
    if (should_exit_cli) {
      terminate_retrofocal(EXIT_SUCCESS);
    }
  }
}

/* Main interactive CLI loop */
void interpreter_cli(void)
{
  /* Set cursor to column 0 for output */
  interpreter_state.cursor_column = 0;
  
  /* Make sure we're in running state for immediate-mode execution */
  interpreter_state.running_state = 1;
  
  /* Main interactive loop */
  while (1) {
    /* Print the FOCAL prompt (default: "*") */
    const char *prompt = (cli_prompt && cli_prompt[0]) ? cli_prompt : "*";
    printf("%s ", prompt);
    fflush(stdout);
    
    /* Read a line with break detection (ESC returns -1) */
    char input_buffer[256];
    int read_result = raw_mode_input_line(input_buffer, sizeof(input_buffer));
    
    if (read_result == -1) {
      /* ESC was pressed - go to next prompt without doing anything */
      printf("\n");
      continue;
    } else if (read_result == 0) {
      /* EOF reached - exit CLI */
      break;
    }
    
    /* Skip blank lines */
    const char *p = input_buffer;
    while (*p && isspace(*p))
      p++;
    if (*p == '\0')
      continue;
    
    /* Process the input line */
    process_cli_line(input_buffer);
  }
}

