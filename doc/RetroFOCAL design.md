RetroFOCAL design notes
=======================

**Copyright © 2023 Maury Markowitz**

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Introduction

This document explains some of the design goals, coding decisions and style selections used in RetroFOCAL.

## Impetus

RetroFOCAL was created as a fork of the [RetroBASIC](https://github.com/maurymarkowitz/RetroBASIC) program. FOCAL is very similar to BASIC, which makes it very easy to port. It is also a simpler language than BASIC, and there are only two major versions that need to be supported. It is a low-cost project that might be of some use in the retrocomputing field. 

RetroFOCAL aims to run any DEC-style FOCAL program without modification. There is the important caveat that the program cannot not make use of invoke code using LIBRARY or various input/output functions that rely on OPEN. But such was the case for most FOCAL programs of interest in the retrocomputing field.

## Variations of FOCAL

DEC released two major versions of FOCAL, the original which was retroactively known as FOCAL-69, and the slightly expanded FOCAL-71. The later is a superset of the former, so  any program written for the earlier version should run without modification on the latter. For this reason, RetroFOCAL matches FOCAL-71.

Several other versions of FOCAL emerged, including a [lost version from Microsoft](https://devblogs.microsoft.com/oldnewthing/20200616-00/?p=103869). The only major variation that saw use was [U/W-FOCAL](https://www.grc.com/pdp-8/docs/FOCAL_Reference_Manual.pdf) from Jim van Zee at the University of Washington. This added a large number of new functions, two-dimensional arrays and a variety of array functions, and a variety of other tweaks and additions. There are not too many changes that it couldn't be supported for a little work. But as the main goal of RetroFOCAL is to run common FOCAL programs, and those use the DEC variations, support has not yet been added.

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
