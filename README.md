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

RetroFOCAL is an interpreter for programs written in the FOCAL language seen on DEC machines like the PDP-8. FOCAL is very similar to early BASIC interpreters and anyone familiar with BASIC will feel at home in FOCAL. FOCAL was only used for a short time from 1968 until the early 1970s, when DEC started promoting their own versions of BASIC as the market moved to that language *en mass*. In spite of this short lifetime, a number of historically important programs were intially created in this language, including [Lunar Lander](https://www.cs.brandeis.edu/~storer/LunarLander/LunarLander.html) and [The Sumerian Game](https://en.wikipedia.org/wiki/The_Sumerian_Game), better known today as [Hamurabi](https://en.wikipedia.org/wiki/Hamurabi_(video_game)).

RetroFOCAL can redirect the output from `TYPE` statements and `ASK` prompts to a file, and read the responses to `ASK` statements from a file. This can be used to provide the same input to a program multiple times, and then the output can be `diff`ed to look for changes. This is aided by a command-line option to set the random number seed value, so that the random numbers are always the same. It also includes a simple static analyzer that (optionally) prints statistics for the program after it completes. This includes the length of the program and its line number range, the number and types of variables used, and similar details.

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

**Note for Apple Silicon (M1/M2/M3) Macs:** Homebrew installs to `/opt/homebrew` instead of `/usr/local`. If `/opt/homebrew/bin` is not in your PATH, add it to your shell profile or run the executable directly from that directory.

On Windows, you can use Scoop:

```powershell
scoop bucket add maurymarkowitz https://github.com/maurymarkowitz/scoop-bucket
scoop install retrofocal
```

## Building RetroFOCAL

The project is built using Make (with `flex` and `bison` for lex/yacc). On macOS and Linux, all of the dependancies and tools should already be installed. On windows, the system uses MinGW, which you will need to install before building. See [docs/BUILD.md](docs/BUILD.md) for full instructions for Linux/macOS and Windows, including install/uninstall paths and required packages.

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

If you wish to use RetroFOCAL to simply check syntax or collect statistics, use the `-n` and `-p` switches.

Short options with no parameters can be ganged, for instance `-unp`.

## Missing features and Erata

A complete list of ongoing changes is maintained in the TODO file, but here are some important limitations:

* ASK statements with multiple values currently have to be entered with return/enter between each value, spaces do not work.
* The system does not support "immediate mode" (command line) input. It is not supposed to, but could potentially do so.
* WRITE, LIBRARY, MODIFY and ERASE (lines) are currently not implemented, in keeping with the use-case.
* File I/O is not supported, commands like OPEN will be parsed but have no effect.
