RetroFOCAL build notes
======================

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Introduction

This document lists a number of known issues found when building on different platforms.

## makefile use

RetroFOCAL should build on any system with a working makefile system. It uses standard ANSI-C, lex and yacc, and has no external depandancies. A simple `make` should produce a runnable binary. If it does not, please file bug reports and/or pull requests. There may be a few warnings from the lex/yacc code but these will be addressed over over time.

## macOS

An Xcode project is included that can be used to organize and build RetroFOCAL. I find its Git integration useful. It includes two schemes, one that runs lunar.fc and the other sumer.fc. sumer is very useful for syntax checking as it uses almost every standard feature of the language.

One annoyance is that Apple has redefined the yacc and lex commands to output their intermediary files to the `DerivedSources` directory, which makes it invisible to the other files in the build. As a result, if you make changes to `scan` or `parse`, you'll have to look in the `/obj/` directory for the y.tab.h and copy that back out to `/src/` as parse.h (or copypasta the code). The most obvious sign this has happened is errors like `Use of undeclared identifier 'TIME'; did you mean 'DIM'?`.

It should be possible to fix this problem with a suitable custom build rule, but my attempts to make one have all failed. If someone can get this to work, please submit a pull request.
