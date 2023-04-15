/* Version info for RetroFOCAL
 Copyright (C) 2023 Maury Markowitz

 Based on RetroBASIC
 Copyright (C) 2023 Maury Markowitz

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

#ifndef version_h
#define version_h

#include <stdio.h>
#include <string.h>

/**
 * @file version.h
 * @author Maury Markowitz
 * @date 3 March 2022
 * @brief Maintains program version info.
 *
 * Methods to return the version as a series of numbers or in string format.
 *
 */

/**
 * Version structure.
 */
struct VERSION {
    int major;
    int minor;
    int build;
};

/**
 * Returns the version information structure.
 */
static struct VERSION getVersion()
{
    struct VERSION v = {1, 1, 1};
    return v;
}

/**
 * Returns the version information as a dot-formatted string.
 *
 * @paramout vers The version in string format.
 */
static void getVersionString(char *vers)
{
    struct VERSION v = getVersion();
    char major[10];
    char minor[10];
    char build[20];
    
    sprintf(major, "%i.", v.major);
    sprintf(minor, "%i.", v.minor);
    sprintf(build, "%i", v.build);
    
    strcpy(vers, major);
    strcat(vers, minor);
    strcat(vers, build);
}

#endif /* version_h */
