RetroFOCAL
==========

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Contents

* [Introduction](#introduction)
* [Installing RetroFOCAL](#installing-retrofocal)
* [Building RetroFOCAL](#building-retrofocal)
* [Running RetroFOCAL with an existing program](#running-retrofocal-with-an-existing-program)
* [Running RetroFOCAL interactively](#running-retrofocal-interactively)
* [Missing features and Errata](#missing-features-and-errata)

## Introduction

RetroFOCAL is an interpreter for programs written in the FOCAL language seen on DEC machines like the PDP-8. FOCAL is very similar to early BASIC interpreters and anyone familiar with BASIC will feel at home in FOCAL. FOCAL was only used for a short time from 1968 until the early 1970s, when DEC started promoting their own versions of BASIC as the market moved to that language *en mass*. In spite of this short lifetime, a number of historically important programs were initially created in this language, including [Lunar Lander](https://www.cs.brandeis.edu/~storer/LunarLander/LunarLander.html) and [The Sumerian Game](https://en.wikipedia.org/wiki/The_Sumerian_Game), better known today as [Hamurabi](https://en.wikipedia.org/wiki/Hamurabi_(video_game)).

RetroFOCAL can redirect the output from `TYPE` statements and `ASK` prompts to a file, and read the responses to `ASK` statements from a file. This can be used to provide the same input to a program multiple times, and then the output can be `diff`ed to look for changes. This is aided by a command-line option to set the random number seed value, so that the random numbers are always the same. It also includes a simple static analyzer that (optionally) prints statistics for the program after it completes. This includes the length of the program and its line number range, the number and types of variables used, and similar details.

The installation also includes an extensive reference manual which includes a complete list of the FOCAL language and how it differs across various platforms. This can be used both as a reference for RetroFOCAL as well as a general guide to FOCAL and a useful resource for porting older programs.

RetroFOCAL is based on [RetroBASIC](https://github.com/maurymarkowitz/RetroBASIC) by Maury Markowitz, which is based on gnbasic by James Bowman.

## Installing RetroFOCAL

The latest source code and any prebuilt binaries are available at:

https://github.com/maurymarkowitz/RetroFOCAL/releases

Binary packages are provided for macOS, Linux, and Windows when releases are published.

On macOS and Linux, you can use Homebrew:

```sh
brew tap maurymarkowitz/tap https://github.com/maurymarkowitz/homebrew-tap
brew install maurymarkowitz/tap/retrofocal
```

**Note** Homebrew installs to `/opt/homebrew/bin` instead of `/usr/local/bin` on Apple Silicon Macs.

On Windows, you can use Scoop:

```powershell
scoop bucket add maurymarkowitz https://github.com/maurymarkowitz/scoop-bucket
scoop install retrofocal
```

## Building RetroFOCAL

You can also easily build the project from source. It is a makefile project that does not require `autoconfig` or other steps. A simple `make` in the root directory should produce a working executable on practically any unix-like system.

Windows compatibility is provided using MinGW, which needs to be installed manually. The same makefile is used for both Unix and Windows, and the `make` utility supplied with MinGW works perfectly. Flex and Bison will generally have to be manually specified as most basic MinGW installs will not include them by default.

Both platforms support `make install` which adds the manuals to the proper locations, and `make uninstall` to cleanly remove all the parts. `make install` defaults to `/usr/local`; override with `PREFIX` if needed (for example `make PREFIX=/opt/retrofocal install`).

## Running RetroFOCAL with an existing program

RetroFOCAL is generally used to run existing programs, saved to a text file normally with the extension `.fc`. You can use it this way using a command similar to this example, replacing the `program.fc` with the name of the text file containing the FOCAL program you wish to run:

```./retrofocal program.fc```

It will accept any text file as input and report (cryptic) errors if it cannot properly parse it. If parsing succeeds, the program (normally) begins running immediately.

Command-line options include:

`--help`, `-h`: print usage notes  
`--version`, `-v`: print version info  
`--upper-case`, `-u`: force input to upper-case, basically like using caps lock  
`--random`, `-r`: seed the random number generator  
`--output-file`, `-o`: redirect TYPE to the named file  
`--input-file`, `-i`: redirect ASK from the named file, one value per line  
`--no-run`, `-n`: do not run the FOCAL program, simply read and parse it and then exit  
`--print-stats`, `-p`: send a selection of statistics to the console  
`--write-stats`, `-w`: write the statistics to the named file in a machine readable format  

If you wish to use RetroFOCAL to simply check syntax or collect statistics, use the `-n` and `-p` switches.

Short options with no parameters can be ganged, for instance, `-unp`.

## Running RetroFOCAL interactively

RetroFOCAL includes a simple line-oriented editor that allows you to create and edit programs interactively, as well as run them. To enter interactive mode, launch RetroFOCAL with no parameters:

```
./retrofocal

RetroFOCAL CLI

* 
```

In interactive mode you can:

- Type program lines with line numbers to store them (e.g., `1.10 SET A=10`)
- Type just a line number to delete that line
- Type FOCAL statements without a line number to execute them immediately
- Use `DO` to run the stored program
- Use `MODIFY <line>` to print a specified stored line
- Use `WRITE` to display the current program
- Use `ERASE` to remove stored lines
- Use `LIBRARY CALL` to load a program from disk

You can customize the interactive prompt with the `--prompt` option. For example, to use a `>` prompt instead of the default `*`:

```./retrofocal --prompt ">```

The break key (Esc on Unix/macOS) can be used in interactive mode to return to the prompt, or in non-interactive mode to exit directly to the shell.

## Missing features and Errata

A complete list of ongoing changes is maintained in the TODO file, but here are some important limitations:

* ASK statements with multiple values currently have to be entered with return/enter between each value, spaces do not work.
* File I/O is not supported, commands like OPEN will be parsed but have no effect.
