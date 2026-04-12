/* io (header) for RetroFOCAL
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

#ifndef __IO_H__
#define __IO_H__

#include "stdhdr.h"

#if !defined(WIN32) && !defined(_WIN32)
  #include <termios.h>
  #include <sys/select.h>
  #include <sys/time.h>
#endif

#include <unistd.h>

/**
 * @file io.h
 * @author Maury Markowitz
 * @date 11 April 2026
 * @brief Terminal input handling for RetroFOCAL interactive mode.
 *
 */

/**
 * Sets up the terminal for non-blocking raw input.
 * Must be called once before the interpreter loop begins.
 */
void setup_terminal_for_input(void);

/**
 * Restores the terminal to canonical (line-buffered) mode.
 * Call before fgets(stdin) and follow with setup_terminal_for_input().
 */
void restore_terminal(void);

/**
 * Restores the terminal and exits the application cleanly.
 */
void terminate_retrofocal(int status);

/**
 * Reads a line in raw mode, detecting ESC for BREAK.
 * Terminal must be in raw mode via setup_terminal_for_input().
 * Supports basic editing (backspace). Echoes characters as typed.
 *
 * @param buffer, buffer to store the line (without trailing newline).
 * @param size, size of the buffer.
 * @return 1 if line successfully read, 0 if EOF, -1 if BREAK (ESC) detected.
 */
int raw_mode_input_line(char *buffer, size_t size);

#endif /* __IO_H__ */
