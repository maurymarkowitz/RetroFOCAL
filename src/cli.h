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

#ifndef CLI_H
#define CLI_H

/* Main interactive CLI loop
 * Reads commands from the user, handles line editing and execution
 * Only called when RetroFOCAL is started without a source file
 */
void interpreter_cli(void);

#endif
