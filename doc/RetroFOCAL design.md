RetroFOCAL design notes
=======================

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Introduction

This document explains some of the design goals, coding decisions and style selections used in RetroFOCAL.

## Impetus

RetroFOCAL was created as a fork of the RetroBASIC program. FOCAL is very similar to BASIC, which makes a port simple. It is also a simpler language and there is only two major versions that need to be supported, FOCAL and FOCAL-71, and the later is a strict superset of the former. It is a low-cost project that might be of some use in the retrocomputing field. 

RetroFOCAL aims to run any FOCAL program without modification. There is the important caveat that the program cannot not make use of invoke code using LIBRARY or various input/output functions that rely on OPEN. But such was the case for most FOCAL programs of interest in the retrocomputing field.

## Coding style

RetroFOCAL is writen in a semi-cannonical C format, as defined by the [Canonical C Style Guide](https://people.canonical.com/~msawicz/guides/c/cguide.html). There are two main differences:

1) single-line "blocks" are separated onto two lines to make them look more like multi-line blocks
```
  while (condition)
     dosomething;
```
  as opposed to:

```
  while (condition) dosomething;
```
2) multi-line blocks have the opening brace on the statement line, to make them look more like single-line blocks
```
  while (condition) {
     dosomething;
     andsomethingelse;
  }
```
    as opposed to:

```
  while (condition)
  {
     dosomething;
     andsomethingelse;
  }
```
