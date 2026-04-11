/* io (implementation) for RetroFOCAL
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

#include "io.h"

#if !defined(WIN32) && !defined(_WIN32)
static struct termios original_terminal_attrs;
static bool terminal_raw_mode = false;
#endif

/*
 * Sets up the terminal for non-blocking raw input. Called once at startup
 * so that raw_mode_input_line() can detect ESC for BREAK.
 */
void setup_terminal_for_input(void)
{
#if !defined(WIN32) && !defined(_WIN32)
  if (terminal_raw_mode)
    return;
  struct termios raw;
  if (tcgetattr(STDIN_FILENO, &original_terminal_attrs) < 0) {
    // stdin is not a TTY (e.g., piped input), skip terminal setup
    return;
  }
  raw = original_terminal_attrs;
  raw.c_lflag &= ~(ICANON | ECHO);  // non-canonical, no echo
  raw.c_oflag |= (OPOST | ONLCR);   // ensure output processing and \n to \r\n conversion
  // ISIG is intentionally left set so Ctrl-C/Ctrl-\ still raise signals
  raw.c_cc[VMIN]  = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
  terminal_raw_mode = true;
#endif
}

/*
 * Restores the terminal to its original canonical state. Call this before
 * any fgets(stdin) so that the user gets normal line-editing, then call
 * setup_terminal_for_input() again afterwards.
 */
void restore_terminal(void)
{
#if !defined(WIN32) && !defined(_WIN32)
  if (!terminal_raw_mode)
    return;
  tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal_attrs);
  terminal_raw_mode = false;
#endif
}

void terminate_retrofocal(int status)
{
#if !defined(WIN32) && !defined(_WIN32)
  restore_terminal();
#endif
  exit(status);
}

/*
 * Reads a line from stdin in raw mode, supporting basic editing and detecting ESC for BREAK.
 * Returns 1 on success (line in buffer), 0 on EOF, -1 if ESC/BREAK detected.
 */
int raw_mode_input_line(char *buffer, size_t size)
{
#if defined(_WIN32) || defined(WIN32)
  /* Windows falls back to canonical fgets behavior for INPUT */
  char *result = fgets(buffer, size, stdin);
  if (result == buffer) {
    /* Strip newline if present */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
      buffer[len-1] = '\0';
    }
    return 1;
  }
  return 0;
#else
  /* If stdin is not a TTY, use fgets instead of raw mode */
  if (!isatty(STDIN_FILENO)) {
    char *result = fgets(buffer, size, stdin);
    if (result == buffer) {
      /* Strip newline if present */
      size_t len = strlen(buffer);
      if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
      }
      return 1;
    }
    return 0;
  }
  
  size_t pos = 0;
  unsigned char c;
  
  while (pos < size - 1) {
    /* Use select to wait for input */
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 100000;  /* 100ms timeout to check for BREAK periodically */

    int sel = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
    if (sel < 0) {
      buffer[pos] = '\0';
      return -1;  /* error */
    }
    if (sel == 0) {
      /* Timeout - loop again */
      continue;
    }

    /* Data available, read one character */
    int ch = read(STDIN_FILENO, &c, 1);
    if (ch <= 0) {
      buffer[pos] = '\0';
      return (ch == 0) ? 0 : -1;  /* EOF or error */
    }
    
    if (c == 27) {  /* ESC */
      buffer[pos] = '\0';
      return -1;     /* BREAK */
    }
    if (c == 3 || c == 24) {  /* Ctrl-C or Ctrl-X */
      buffer[pos] = '\0';
      terminate_retrofocal(EXIT_SUCCESS);
    }
    
    if (c == '\n' || c == '\r') {
      buffer[pos] = '\0';
      write(STDOUT_FILENO, "\n", 1);
      return 1;      /* success */
    }
    
    if (c == 127 || c == 8) {  /* DEL or backspace */
      if (pos > 0) {
        pos--;
        write(STDOUT_FILENO, "\b \b", 3);  /* erase character */
      }
      continue;
    }
    
    if (c >= 32 && c < 127) {  /* printable ASCII */
      buffer[pos++] = c;
      write(STDOUT_FILENO, &c, 1);  /* echo */
    }
    /* ignore other control chars */
  }
  
  buffer[pos] = '\0';
  return 1;  /* buffer full */
#endif
}
