RetroFOCAL design notes
=======================

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Introduction

This document explains some of the design goals, coding decisions and style selections used in RetroFOCAL.

## Impetus

RetroFOCAL was created as a fork of the [RetroBASIC](https://github.com/maurymarkowitz/RetroBASIC) program. FOCAL is very similar to BASIC, but much simpler, which makes it easy to port. It's further simplified by the fact that there are only two major dialects that need to be supported.

RetroFOCAL aims to run any DEC-style FOCAL program without modification. There is the important caveat that the program cannot not make use of invoke code using LIBRARY or various input/output functions that rely on OPEN. But such was the case for most FOCAL programs of interest in the retrocomputing field.

## Variations of FOCAL

DEC released two major versions of FOCAL, the original which was retroactively known as FOCAL-69, and the slightly expanded FOCAL-71. The later is a superset of the former, so  any program written for the earlier version should run without modification on the latter. For this reason, RetroFOCAL normally matches FOCAL-69, but extensions for -71 are included by default if they have no effect on the programs. Other changes, like prining a colon in ASK statements, are included as options that can be turned on with command line switches. The system otherwise matches the FOCAL-69 definiton in the [Advanced FOCAL Technical Specifications](http://bitsavers.trailing-edge.com/pdf/dec/pdp8/focal/DEC-08-AJBB-DL_Advanced_FOCAL_Technical_Specification_Apr69.pdf) manual.

Several other versions of FOCAL emerged over time, including a [lost version from Microsoft](https://devblogs.microsoft.com/oldnewthing/20200616-00/?p=103869). The only major variation that saw use was [U/W-FOCAL](https://www.grc.com/pdp-8/docs/FOCAL_Reference_Manual.pdf) from Jim van Zee at the University of Washington. This added a large number of new functions, two-dimensional arrays and a variety of array functions, and a variety of other tweaks and additions. There are not too many changes that it couldn't be supported for a little work. But as the main goal of RetroFOCAL is to run common FOCAL programs, and those use the DEC dialects, support for U/W has not yet been added.

There are other modern versions of FOCAL, including Dave Pitt's [FOCAL-69](https://github.com/AndrewSav/focal-69) interpreter. This version includes a fully interactive command line enviornment and full support for files and library calls. Any focal program that does not work in RetroFOCAL will likely work in Dave's version.

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
