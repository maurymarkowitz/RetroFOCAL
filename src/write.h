/* write (header) for RetroFOCAL
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

#ifndef __WRITE_H__
#define __WRITE_H__

/**
 * @file write.h
 * @author Maury Markowitz
 * @date 11 April 2026
 * @brief Program listing reconstruction.
 *
 * Reconstructs the tokenized program as canonical FOCAL source code.
 * Used by WRITE command and LIBRARY SAVE.
 */

/**
 * Generate a canonical listing of the current program.
 *
 * @param start_line The first line number to include (in FOCAL xx.yy format, *100).
 * @param end_line The last line number to include (in FOCAL xx.yy format, *100).
 * @return A heap-allocated string containing the listing. Caller must free it.
 */
char *write_program(int start_line, int end_line);

#endif /* __WRITE_H__ */
