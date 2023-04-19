RetroFOCAL
==========

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Contents

* [Introduction](#introduction)
* [Running RetroFOCAL](#running-retrofocal)
* [Building RetroFOCAL](#building-retrofocal)
* [Missing features and Erata](#missing-features-and-erata)

## Introduction

RetroFOCAL is an interpreter for programs written in the FOCAL language seen on DEC machines like the PDP-8. FOCAL is very similar to early BASIC interpreters and anyone familiar with BASIC will feel at home in FOCAL. FOCAL was only used for a short time from 1968 until the early 1970s, when DEC started promoting their own versions of BASIC as the market moved to that language *en mass*. In spite of this short lifetime, a number of historically important programs were intially created in this language, including [Lunar Lander](https://www.cs.brandeis.edu/~storer/LunarLander/LunarLander.html) and [The Sumerian Game](https://en.wikipedia.org/wiki/The_Sumerian_Game), better known today as Hamurabi.

RetroFOCAL can redirect the output from `TYPE` statements and `ASK` prompts to a file, and read the responses to `ASK` statements from a file. This can be used to provide the same input to a program multiple times, and then the output can be `diff`ed to look for changes. This is aided by a command-line option to set the random number seed value, so that the random numbers are always the same. It also includes a simple static analizer that (optionally) prints statistics for the program after it completes. This includes the length of the program and its line number range, the number and types of variables used, and similar details.

RetroFOCAL is based on RetroBASIC by Maury Markowitz, which is based on gnbasic by James Bowman.
 
## Running RetroFOCAL

RetroFOCAL is meant to be used with an existing program source file, not interactively. To run it, use:

```./retrofocal program.fc```

It will accept any text file as input and report (cryptic) errors if it cannot properly parse it. If parsing succeeds, the program (normally) begins running immediately.

Command-line options include:

`--help`, `-h`: print usage notes  
`--version`, `-v`: print version info  
`--upper-case`, `-u`: force input to upper-case, basically like using caps lock  
`--random`, `-r`: seed the random number generator  
`--output-file`, `-o`: redirect TYPE to the named file  
`--input-file`, `-i`: redirect ASK from the named file, one value per line  
`--no_run`, `-n`: do not run the FOCAL program, simply read and parse it and then exit  
`--print_statistics`, `-p`: send a selection of statistics to the console  
`--write_statistics`, `-w`: write the statistics to the named file in a machine readable format

If you wish to use RetroFOCAL to simply check syntax or collect statistics, use the -n and -p switches.

Short options with no parameters can be ganged, for instance -unp.

## Building RetroFOCAL

The RetroFOCAL interpreter is written for lex/yacc/c and is normally compiled with flex/bison. It has no external depedancies, although cygwin is required for compiling on Windows. A makefile is included that should run on almost any Unix-like system, including macOS.

An Xcode project is also included, which is the primary building method during testing. It has one drawback (currently), the bison build rule in Xcode does not properly build `parse.h` into the `src` folder, but follows yacc-like rules and builds `y.tab.h` file in the `../DerivedFiles` folder. If you make changes to `parse.y`, be sure to copy the new `y.tab.h` to `/src/parse.h` for those changes to be visible.

If anyone would like to contribute a VS.net project, it would be greatly appeciated.

## Missing features and Erata

A complete list of ongoing changes is maintained in the TODO file, but here are some important limitations:

* ASK statements with multiple values currently have to be entered with return/enter between each value, spaces do not work.
* The system does not support "immediate mode" (command line) input. It is not supposed to, but could potentially do so.
* WRITE, LIBRARY, MODIFY and ERASE (lines) are currently not implemented, in keeping with the use-case.
* File I/O is not supported, commands like OPEN will be parsed but have no effect.
