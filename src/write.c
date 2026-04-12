/* write (implementation) for RetroFOCAL
 Copyright (C) 2026 Maury Markowitz
 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "write.h"
#include "retrofocal.h"
#include "parse.h"

typedef struct {
  char *data;
  size_t length;
  size_t capacity;
} string_builder_t;

static void sb_init(string_builder_t *sb)
{
  sb->capacity = 256;
  sb->length = 0;
  sb->data = malloc(sb->capacity);
  if (sb->data == NULL) {
    fprintf(stderr, "Malloc failed in write_output\n");
    exit(EXIT_FAILURE);
  }
  sb->data[0] = '\0';
}

static void sb_reserve(string_builder_t *sb, size_t extra)
{
  if (sb->length + extra + 1 > sb->capacity) {
    size_t need = sb->length + extra + 1;
    size_t new_capacity = sb->capacity * 2;
    while (new_capacity < need)
      new_capacity *= 2;
    sb->data = realloc(sb->data, new_capacity);
    if (sb->data == NULL) {
      fprintf(stderr, "Realloc failed in write_output\n");
      exit(EXIT_FAILURE);
    }
    sb->capacity = new_capacity;
  }
}

static void sb_append(string_builder_t *sb, const char *text)
{
  if (text == NULL)
    return;

  size_t added = strlen(text);
  sb_reserve(sb, added);
  memcpy(sb->data + sb->length, text, added);
  sb->length += added;
  sb->data[sb->length] = '\0';
}

static void sb_append_fmt(string_builder_t *sb, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  va_list ap_copy;
  va_copy(ap_copy, ap);
  int needed = vsnprintf(NULL, 0, fmt, ap_copy);
  va_end(ap_copy);
  if (needed < 0) {
    va_end(ap);
    return;
  }

  sb_reserve(sb, (size_t)needed);
  vsnprintf(sb->data + sb->length, sb->capacity - sb->length, fmt, ap);
  sb->length += (size_t)needed;
  sb->data[sb->length] = '\0';
  va_end(ap);
}

/**
 * Reconstruct an expression as canonical FOCAL source code.
 *
 * @param e The expression to reconstruct.
 * @param sb String builder to append result to.
 */
static void expression_to_string(expression_t *e, string_builder_t *sb)
{
  if (e == NULL)
    return;
    
  switch (e->type) {
    case number:
      sb_append_fmt(sb, "%g", e->parms.number);
      break;
      
    case string:
      sb_append(sb, "\"");
      sb_append(sb, e->parms.string);
      sb_append(sb, "\"");
      break;
      
    case numstr:
      sb_append(sb, e->parms.string);
      break;
      
    case variable:
      if (e->parms.variable && e->parms.variable->name) {
        sb_append(sb, e->parms.variable->name);
        
        // Add subscripts if present
        if (e->parms.variable->subscripts != NULL) {
          sb_append(sb, "(");
          bool first = true;
          for (list_t *node = e->parms.variable->subscripts; node != NULL; node = node->next) {
            if (!first)
              sb_append(sb, ",");
            expression_to_string((expression_t *)node->data, sb);
            first = false;
          }
          sb_append(sb, ")");
        }
      }
      break;
      
    case op:
      // Unary operators
      if (e->parms.op.arity == 1) {
        switch (e->parms.op.opcode) {
          case '-':
            sb_append(sb, "-");
            break;
          case FABS:
            sb_append(sb, "FABS(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FATN:
            sb_append(sb, "FATN(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FCOS:
            sb_append(sb, "FCOS(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FEXP:
            sb_append(sb, "FEXP(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FITR:
            sb_append(sb, "FITR(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FLOG:
            sb_append(sb, "FLOG(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FSIN:
            sb_append(sb, "FSIN(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FSGN:
            sb_append(sb, "FSGN(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FSQT:
            sb_append(sb, "FSQT(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FIN:
            sb_append(sb, "FIN(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          case FOUT:
            sb_append(sb, "FOUT(");
            expression_to_string(e->parms.op.p[0], sb);
            sb_append(sb, ")");
            return;
          default:
            // Unknown unary operator
            sb_append(sb, "?");
        }
        expression_to_string(e->parms.op.p[0], sb);
      }
      // Binary operators
      else if (e->parms.op.arity == 2) {
        sb_append(sb, "(");
        expression_to_string(e->parms.op.p[0], sb);
        
        switch (e->parms.op.opcode) {
          case '+':
            sb_append(sb, "+");
            break;
          case '-':
            sb_append(sb, "-");
            break;
          case '*':
            sb_append(sb, "*");
            break;
          case '/':
            sb_append(sb, "/");
            break;
          case '^':
            sb_append(sb, "^");
            break;
          case '=':
            sb_append(sb, "=");
            break;
          default:
            sb_append(sb, "?");
        }
        
        expression_to_string(e->parms.op.p[1], sb);
        sb_append(sb, ")");
      }
      break;
      
    default:
      sb_append(sb, "?");
  }
}

/**
 * Reconstruct a single statement as canonical FOCAL source code.
 *
 * @param stmt The statement to reconstruct.
 * @param sb String builder to append result to.
 */
static void statement_to_string(statement_t *stmt, string_builder_t *sb)
{
  if (stmt == NULL)
    return;
    
  switch (stmt->type) {
    case COMMENT:
      sb_append(sb, stmt->abbreviated ? "C" : "COMMENT");
      if (stmt->parms.rem)
        sb_append(sb, stmt->parms.rem);
      break;
      
    case SET:
      sb_append(sb, stmt->abbreviated ? "S " : "SET ");
      if (stmt->parms.set.variable && stmt->parms.set.variable->name)
        sb_append(sb, stmt->parms.set.variable->name);
      sb_append(sb, "=");
      expression_to_string(stmt->parms.set.expression, sb);
      break;
      
    case DO:
      if (stmt->abbreviated)
        sb_append_fmt(sb, "D %2.2f", stmt->parms._do);
      else
        sb_append_fmt(sb, "DO %2.2f", stmt->parms._do);
      break;
      
    case GOTO:
      if (stmt->abbreviated)
        sb_append_fmt(sb, "G %2.2f", stmt->parms.go);
      else
        sb_append_fmt(sb, "GOTO %2.2f", stmt->parms.go);
      break;
      
    case FOR:
      sb_append(sb, stmt->abbreviated ? "F " : "FOR ");
      if (stmt->parms._for.variable && stmt->parms._for.variable->name)
        sb_append(sb, stmt->parms._for.variable->name);
      sb_append(sb, "=");
      expression_to_string(stmt->parms._for.begin, sb);
      sb_append(sb, ",");
      expression_to_string(stmt->parms._for.step, sb);
      sb_append(sb, ",");
      expression_to_string(stmt->parms._for.end, sb);
      break;
      
    case IF:
    {
      sb_append(sb, stmt->abbreviated ? "I (" : "IF (");
      expression_to_string(stmt->parms._if.condition, sb);
      sb_append(sb, ")");
      
      int need_first = 1;
      if (stmt->parms._if.less_line > 0) {
        sb_append_fmt(sb, "%2.2f", stmt->parms._if.less_line);
        need_first = 0;
      }
      
      if (stmt->parms._if.zero_line > 0) {
        if (!need_first)
          sb_append(sb, ",");
        sb_append_fmt(sb, "%2.2f", stmt->parms._if.zero_line);
        need_first = 0;
      }
      
      if (stmt->parms._if.more_line > 0) {
        if (!need_first)
          sb_append(sb, ",");
        sb_append_fmt(sb, "%2.2f", stmt->parms._if.more_line);
      }
      break;
    }
      
    case ASK:
    {
      sb_append(sb, stmt->abbreviated ? "A " : "ASK ");
      bool first = true;
      for (list_t *node = stmt->parms.input; node != NULL; node = node->next) {
        if (!first)
          sb_append(sb, ",");
        if (node->data) {
          variable_t *var = (variable_t *)node->data;
          if (var->name)
            sb_append(sb, var->name);
        }
        first = false;
      }
      break;
    }
      
    case TYPE:
    {
      sb_append(sb, stmt->abbreviated ? "T " : "TYPE ");
      bool first = true;
      for (list_t *node = stmt->parms.print; node != NULL; node = node->next) {
        if (!first)
          sb_append(sb, ",");
        if (node->data) {
          printitem_t *item = (printitem_t *)node->data;
          if (item->expression) {
            expression_to_string(item->expression, sb);
          } else if (item->format) {
            sb_append_fmt(sb, "%%%s", item->format);
          } else if (item->separator) {
            switch (item->separator) {
              case '!':
                sb_append(sb, "!");
                break;
              case '#':
                sb_append(sb, "#");
                break;
              case ':':
                sb_append(sb, ":");
                break;
            }
          }
        }
        first = false;
      }
      break;
    }
      
    case RETURN:
      sb_append(sb, stmt->abbreviated ? "R" : "RETURN");
      break;
      
    case QUIT:
      sb_append(sb, stmt->abbreviated ? "Q" : "QUIT");
      break;
      
    case ERASE:
      sb_append(sb, stmt->abbreviated ? "E" : "ERASE");
      break;
      
    case LIBRARY:
      sb_append(sb, stmt->abbreviated ? "L " : "LIBRARY ");
      switch (stmt->parms.library.action) {
        case 0:
          sb_append(sb, "SAVE ");
          break;
        case 1:
          sb_append(sb, "CALL ");
          break;
        case 2:
          sb_append(sb, "RUN ");
          break;
        default:
          sb_append(sb, "UNKNOWN ");
          break;
      }
      if (stmt->parms.library.filename)
        sb_append(sb, stmt->parms.library.filename);
      break;
      
    default:
      sb_append(sb, "; unknown statement");
  }
}

/**
 * Generate a canonical listing of the current program.
 */
char *write_program(int start_line, int end_line)
{
  string_builder_t sb;
  sb_init(&sb);
  
  // Iterate through the program lines
  for (int i = start_line; i < end_line && i < MAXLINE; i++) {
    if (interpreter_state.lines[i] == NULL)
      continue;
      
    // Get the line number in FOCAL format (xx.yy)
    int group = i / 100;
    int step = i % 100;
    
    // Append line number
    sb_append_fmt(&sb, "%d.%02d ", group, step);
    
    // Append each statement on the line
    bool first_stmt = true;
    for (list_t *node = interpreter_state.lines[i]; node != NULL; node = node->next) {
      if (node->data) {
        if (!first_stmt)
          sb_append(&sb, ";");
        statement_to_string((statement_t *)node->data, &sb);
        first_stmt = false;
      }
    }
    
    // End the line
    sb_append(&sb, "\n");
  }
  
  return sb.data;
}
