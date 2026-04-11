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
      /* Create a file-based input using a unique temp file */
      static int cli_parse_count = 0;
      char temp_path[64];
      snprintf(temp_path, sizeof(temp_path), "/tmp/retrofocal_cli_%d_%d.focal", getpid(), cli_parse_count++);
      
      FILE *temp_file = fopen(temp_path, "w+");
      if (!temp_file) {
        return;
      }
      
      /* Write the statement to the temp file (input_line already has newline stripped) */
      fprintf(temp_file, "%s\n", input_line);
      fflush(temp_file);
      rewind(temp_file);
      
      /* Parse this as a program line */
      FILE *old_yyin = yyin;
      yyin = temp_file;
      yyrestart(temp_file);
      yyparse();
      fclose(temp_file);
      yyin = old_yyin;
      
      /* Clean up the temporary file */
      unlink(temp_path);
    }
  } else {
    /* No line number - this is immediate-mode execution */
    /* Create a file-based input using a unique temp file */
    static int cli_immed_count = 0;
    char temp_path[64];
    snprintf(temp_path, sizeof(temp_path), "/tmp/retrofocal_immed_%d_%d.focal", getpid(), cli_immed_count++);
    
    FILE *temp_file = fopen(temp_path, "w+");
    if (!temp_file) {
      return;
    }
    
    /* Write the statement to the temp file with a valid temporary line number (99.00) */
    fprintf(temp_file, "99.00 %s\n", input_line);
    fflush(temp_file);
    rewind(temp_file);
    
    /* Parse this line into the current program storage */
    FILE *old_yyin = yyin;
    yyin = temp_file;
    yyrestart(temp_file);
    yyparse();
    fclose(temp_file);
    yyin = old_yyin;
    
    /* Check if this is a QUIT statement - if so, set a flag to exit CLI */
    bool should_exit_cli = false;
    if (interpreter_state.lines[9900] != NULL && interpreter_state.lines[9900]->data != NULL) {
      statement_t *stmt = (statement_t *)interpreter_state.lines[9900]->data;
      if (stmt->type == QUIT) {
        should_exit_cli = true;
      }
    }
    
    /* Execute the immediate line in the context of the existing program. */
    interpreter_state.first_line_index = 9900;
    interpreter_state.current_statement = interpreter_state.lines[9900];
    interpreter_state.running_state = 1;
    interpreter_run();

    /* Remove the temporary line and restore line links */
    if (interpreter_state.lines[9900] != NULL) {
      lst_free(interpreter_state.lines[9900]);
      interpreter_state.lines[9900] = NULL;
    }
    interpreter_post_parse();
    
    /* Clean up the temporary file */
    unlink(temp_path);
    
    /* Exit CLI if QUIT was executed */
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

