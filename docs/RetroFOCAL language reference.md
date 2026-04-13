RetroFOCAL Language Reference Manual
====================================

**Copyright © 2023 Maury Markowitz**

Version 1.1.3

[![GPL license](http://img.shields.io/badge/license-GPL-brightgreen.svg)](https://opensource.org/licenses/gpl-license)

## Introduction

### What is RetroFOCAL?

RetroFOCAL is a version of the FOCAL programming language intended to run classic FOCAL programs. RetroFOCAL is designed to run any program that ran on FOCAL-71, which is a superset of the otherwise similar FOCAL-69. Practically any major program should run properly without conversion. Programs must be provided in plain text, better known as "source code", which was the normal way to distribute FOCAL code.

RetroFOCAL also supports an interactive CLI mode. When started without a source file, it enters a prompt where users can type FOCAL statements directly, edit numbered lines, and execute code immediately. The prompt defaults to `*`, and can be changed via the `--prompt` command-line option.

### What RetroFOCAL is not

The goal of RetroFOCAL is to allow you to run popular FOCAL programs written during the language's Golden Age in the late 1960s. As such, it does not currently suport file handling beyond `LIBRARY CALL`, `LIBRARY SAVE`, and `LIBRARY RUN`.

There is only one major variation on FOCAL, U/W FOCAL, or UWF, from the University of Washington. Using memory overlays, UWF offers a range of new statements and functions while still using less memory that the original FOCAL-8. UWF extensions are not currently supported in RetroFOCAL.

## Contents

<!-- TOC start (generated with https://github.com/derlin/bitdowntoc) -->

- [About this manual](#about-this-manual)
   * [Abbreviations](#abbreviations)
   * [Formatting and notation](#formatting-and-notation)
- [Some underlying concepts](#some-underlying-concepts)
   * [Syntax example](#syntax-example)
- [Data in FOCAL programs](#data-in-focal-programs)
   * [Variables](#variables)
   * [Arrays](#arrays)
- [Commands](#commands)
   * [`MODIFY`](#modify)
   * [`WRITE`](#write)
   * [`LIBRARY CALL`](#library-call)
   * [`LIBRARY SAVE`](#library-save)
   * [`LIBRARY RUN`](#library-run)
- [Program statements](#program-statements)
   * [`COMMENT` and `CONTINUE`](#comment)
   * [`SET`](#set)
   * [`DO`](#do)
   * [`GOTO` and `GO`](#goto)
   * [`ERASE`](#erase)
   * [`IF`](#if)
   * [`FOR`](#for)
   * [`QUIT`](#quit)
   * [`RETURN`](#return)
- [Input/Output Statements](#inputoutput-statements)
   * [`ASK`](#ask)
   * [`TYPE`](#type)
- [Operators](#operators)
- [Mathematical functions](#mathematical-functions)
   * [`FABS`](#fabs)
   * [`FEXP`](#fexp)
   * [`FITR`](#fitr)
   * [`FLOG`](#flog)
   * [`FRAN`](#fran)
   * [`FSGN`](#fsgn)
   * [`FSQT`](#fsqt)
- [Trigonometric functions](#trigonometric-functions)
   * [`FATN`](#fatn)
   * [`FCOS`](#fcos)
   * [`FSIN`](#fsin)
- [Character functions](#character-functions)
   * [`FIN`](#fin)
   * [`FOUT`](#fout)

<!-- TOC end -->

<!-- TOC --><a name="about-this-manual"></a>
## About this manual

This manual is primarily a listing of the operators, commands and statements in the RetroFOCAL interpreter. It also details some of the internal structures where that aids understanding. It is written in Markdown in order to make it readable on most platforms.

<!-- TOC --><a name="abbreviations"></a>
### Abbreviations

The following abbreviations will be used in this manual:

- *ncon* - numeric constant
- *scon* - string constant
- *con* - either type of constant
- *var* - variable
- *op* - operator
- *exp* - expression
- *statmnt* - any complete statement
- *lineno* - a line number
- *groupno* - a group number

<!-- TOC --><a name="formatting-and-notation"></a>
### Formatting and notation

This manual will use the following typographic conventions:

- monospaced `CAPITAL LETTERS` will be used to indicate keywords
- *italicized* lower case words indicate an abbreviation from the list above
- [square brackets] indicate optional items separated by commas
- {curly braces} indicate "one of" options separated with the vertical bar
- \<angle brackets\> indicate special characters like a carriage return
- the ellipsis... indicates "and more of same", normally in option lists

For example:

`TYPE` {*scon*|*exp*|!|#|:}[,...]

This indicates that the `TYPE` statement consists of the statement keyword followed by a string constant, a numeric expression, or the control characters bang, hash, or colon. The square bracketed section at the end indicates that the first item can be followed by any number of similar expressions separated by commas.

<!-- TOC --><a name="some-underlying-concepts"></a>
## Some underlying concepts

Programming languages, in general, use english-like words to describe a number of instructions that will be carried out in order to produce a desired result. This textual description is known as the *source code*. The instructions have to be converted into an internal format that the computer understands, as opposed to the english-like source code which is understood by the programmer. RetroFOCAL performs this conversion. The result of the conversion is the *machine code* or *p-code* depending on how the system works. This internal language can then be *executed*, or in computer lingo, *run*.

Like most computer languages, FOCAL has a number of **keywords** that are reserved by the language. In FOCAL, the list of keywords is pre-defined, meaning that users cannot add new keywords or modify the actions of existing ones. This contrasts with programming languages like ALGOL which were based around the idea of creating new keywords in code.

In FOCAL, the individual instructions that make up the program are known as **statements**. A statement consists of a keyword and then zero or more **expressions**. Only certain keywords can appear at the front of a statement, others only as part of an expression. To confuse matters, the subset of keywords that can appear at the front are often referred to as "statements" as well, while other dialects refer to these special keywords simply as "keywords". To avoid confusion, this manual will use the term **statement keywords** for this subset of reserved words and to clearly separate the concept of a statement from a keyword.

FOCAL also included a number of **commands**. Commands look exactly like statements and work in the same fashion, but they generally cannot be used as program instructions, only as user-entered interactive instructions. Commands include things like loading and saving a program, sending the code to a printer, and managing files on a disk.

The purpose of an **expression** is to provide data to be acted on by the statements. There are many different types of expressions and much of a program's complexity is found within them. The primary types of expressions are classified by type:

- **number** - such as 10 or -1.75. Also known as **numeric constants**.
- **string** - a series of characters surrounded by double-quote marks, like "Hello, World!". Also known as **string constants**.
- **variable** - a symbolic name which can store the value of a number or a string for future recall.
- **array** - a type of variable that stores multiple values at once.
- **operator** - symbol characters, typically from mathematics, that take one or two expressions and produce a number.
- **function** - operators using names instead of symbols.

FOCAL programs are generally organized over multiple **lines**, each of which contains one or more statements. A line always starts with a **line number**, which acts as a label so other parts of the program can refer to it, as well as a way for various editing tools to allow the user to indicate which lines they wish to edit. In FOCAL, in a somewhat recursive definition, line numbers consist of two parts, the **group number** and the **line number**, which are separated by a period so they look like floating point numbers. Examples include 1.10 or 19.50.

After the line number, there is a statement keyword and then any expressions that keyword may use, forming a complete statement. Lines may have more than one statement separated by semicolons. Multi-statement lines were common in FOCAL programs; they saved memory because they did not have to have the 5-digit line number for every statement, only the single semicolon using one character of memory.

As memory was extremely scarce on the machines FOCAL ran on, it also included the ability to abbreviate any statement keyword to its first character. For instance, the statement keyword `TYPE` can be entered as `T`. The statement keywords were carefully selected so each one started with a unique character.

Execution normally starts with the first statement on the lowest-numbered line, and then continues statement by statement until it reaches the end of the program. Some statements can change this normally linear progression; these are known as **branches**.

<!-- TOC --><a name="syntax-example"></a>
### Syntax example

Here is a simple example of a FOCAL program that illustrates the overall form that RetroFOCAL expects:

    1.10 SET A=10
    1.20 S C=A+5;T C!
    1.30 IF (C-10)1.40;T C," is larger than 10"
    1.40 QUIT

Line 1.10 of this program uses the `SET` statement keyword to assign a value to the numeric variable A, using the operator `=` to separate the variable name from the expression that provides a value. In this case, the associated expression is the numeric constant 10.

Line 1.20 of this program starts with an assignment statement, this time using the abbreviation, `S`. The expression adds 5 to the value in A and places the result in C. This line also has a second statement following the semicolon, which `T`ypes the value of C to the screen. The exclamation mark, `!`, prints a <newline>.

Line 1.30 shows a conditional branch, which is based on the value of C. The expression calculates C-10, and as C will be 15 at this point, the resulting value will be 5. As 5 is a positive value, and the branch is only taken if it is negative, execution continues with the next statement, which prints "15 is larger than 10" to the console. If one were to set the value of A to 3 in line 1.10, this statement would not be printed.

Execution then continues to line 1.40, which ends the execution of the program.

<!-- TOC --><a name="command-line-usage"></a>
## Command-line usage

RetroFOCAL can be invoked in two different modes: batch mode for running programs, and interactive mode for testing programs at the CLI.

### Batch mode

To run a program in batch mode, provide the filename as an argument:

    retrofocal myprogram.fc

The program will be loaded and executed immediately, and the interpreter will exit when the program completes or encounters an error.

Command-line options:

- `--run` or `-r`: Automatically run the program after loading (default behavior)
- `--no-run` or `-n`: Load the program but do not run it automatically
- `--stats`: Print program statistics after execution
- `--help` or `-h`: Display help message

#### Examples:

    retrofocal myprogram.fc --no-run
    retrofocal myprogram.fc --stats

### Interactive mode

To enter interactive mode, invoke RetroFOCAL without providing a source file:

    retrofocal

This will display the FOCAL prompt (default: `*`) and wait for input. In interactive mode, you can:

- Type program lines with line numbers to store them (e.g., `1.10 SET A=10`)
- Type just a line number to delete that line
- Type FOCAL statements without a line number to execute them immediately
- Use `GO` or `DO` to run the stored program
- Use `MODIFY <line>` to print the specified stored line if it exists
- Use `WRITE` to display the current program
- Use `ERASE <line>` to remove a stored line
- Use `ERASE <group>` to remove all lines in a group
- Use `ERASE ALL` to remove all stored lines
- Use `LIBRARY CALL` to load a program from disk
- Use `LIBRARY RUN` to load a program from disk and execute it immediately
- Use `LIBRARY SAVE` to save the current program to disk
- Type `QUIT` or press Ctrl+D to exit

Command-line options for interactive mode:

- `--prompt` or `-p`: Set the CLI prompt (default: `*`)

#### Examples:

    retrofocal --prompt "FOCAL> "
    FOCAL> 1.10 SET A=10
    FOCAL> GO

<!-- TOC --><a name="control-keys"></a>
#### Control Keys: Break, Escape, Ctrl-C, Ctrl-X

In RetroFOCAL, several key combinations can be used to interrupt execution:

- The `ESC` key (or Escape key) pressed while the program is waiting for input during an `ASK` statement will stop execution immediately and return control to the CLI prompt in interactive mode, or terminate RetroFOCAL in batch mode.
- On Windows, the Pause/Break key is also supported for this purpose.
- `Ctrl-C` and `Ctrl-X` will immediately terminate RetroFOCAL and return to the shell, regardless of whether running in interactive or batch mode.

These key combinations allow users to interrupt a running program at any time.

### Using standard input and output

You can redirect program output to files or pipelines using standard shell features:

    retrofocal program.fc > output.txt          # Redirect output to file
    retrofocal program.fc | grep "Result"       # Pipe output to grep
    retrofocal program.fc 2>&1 | tee log.txt    # Capture both stdout and stderr

<!-- TOC --><a name="data-in-focal-programs"></a>
## Data in FOCAL programs

The ultimate goal of a program is to manipulate data. Data comes in two formats, *constants* which are values typed directly into the program code and cannot be changed while running, and *variables*, a letter that is used to reference a numeric value that can be changed by the program while it runs.

Internally, FOCAL only stores numeric values. In the PDP-8 version, this is in the binary "three-word floating-point format". This format cannot be easily read by humans, so FOCAL includes functionality that converts these values to and from the decimal format we are familiar with. So one can type "10.5" into a program and it will automatically be converted to the internal format. The largest number that the format can store is +0.999999 times 10^619, and the smallest is -0.999999 times 10^619.

FOCAL also needs to process textual data, or *strings*. String constants can be used in input and output statements and are denoted by surrounding the text in double-quotes, for instance, `TYPE "HELLO, WORLD!"`. The PDP-8 did not have lower-case characters, although later FOCAL dialects support them. String constants are only supported in the input and output statements, `TYPE` for output and `ASK` for input, where the constant is used as a prompt; `ASK "WHAT IS YOUR AGE",A`.

FOCAL cannot store strings in variables or allow them to be typed in any other statements. To work around this limitation, users found that they could type a string anywhere in code that a number could appear as long as it was prefixed with a digit, normally using 0. This is a kludge; it works because the FOCAL interpreter sees a digit in the code and calls the number parser. Any letters following the digit are, by luck not design, converted to their internal character code.

So for instance, if the program performs `ASK "TYPE Y OR N",X` and the user types "Y", this is converted to the numeric value 25. One can then test if the user typed a "Y" using `IF (X-0Y),,10.1`. `(X-25)` performs the same test, but using `0Y` makes the intention of the program much more obvious.

This conversion can lead to odd results. For instance, if the user types "YES", the conversion code will interpret that as "Y to the exponent S", 

<!-- TOC --><a name="variables"></a>
### Variables

Variables, referred to as *symbols* in the FOCAL documentation, are named references to a location in computer memory holding data. They are a key part of any programming language. In most FOCAL dialects, the name can be one or two characters, the first being A to Z and the optional second character A to Z or 0 to 9. Valid variable names include `A`, `B1` and `XY`. For historical reasons, it is very common to see `I`, `J` and `K` used in most programs, often in loops.

Most FOCAL interpreters store the values of the variables in a table with fixed-width fields. The first two locations in these fields hold the variable's name. FOCAL included a feature to list out all the variables used in a program along with their current values, and to make this line up properly in the display, single-character variable names were padded with the `@` character in these lists. So, for instance, the variable `A` would be printed as `A@`.

<!-- TOC --><a name="arrays"></a>
### Arrays

Any FOCAL variable can also be used as an **array**, or as they refer to it, a *subscripted variable*. In contrast to other languages like BASIC, FOCAL did not actually set aside memory to store these values before use. Instead, it treats a variable that included a parameter as if it were a new variable reference, as if it were part of the name. This also means that FOCAL arrays did not have to be set up in advance of use, as is the case in BASIC where one must `DEF`ine the array.

<!-- TOC --><a name="commands"></a>
## Commands

FOCAL consisted of two separate programs, the FOCAL runtime, and a shell program that edited the source code and performed a small number of commands that were deliberately intended to look like FOCAL statements. So some of the statement-looking lines could be used in your program, while other statement-looking lines could only be used in the shell. This led to the separate concepts of **commands** and **statements**, a separation that many later dialects maintained.

RetroFOCAL implements several commands as statements, allowing program code to control program loading, saving, execution, and display. These include `WRITE` for program listing, `MODIFY` for editing, and `LIBRARY CALL`, `LIBRARY SAVE`, and `LIBRARY RUN` for disk operations. FOCAL also re-uses or re-purposes several other keywords to have different meanings whether they are included inside a program or are used at the command line. These are documented below in the program statements area, and include `DO`, `GO` and `ERASE`.

<!-- TOC --><a name="modify"></a>
### `MODIFY` *lineno*

The `MODIFY` command displays the content of a stored program line if it exists. This is useful for inspecting the exact contents of a line before editing or deleting it.

#### Examples:
```
MODIFY 1.10
```

This will print line 1.10 to the console and ready it for editing, or show an error if the line does not exist.

#### Variations:

Although there is no change in the command or how it works, U/W FOCAL sometimes refers to this as `MOVE`.

<!-- TOC --><a name="write"></a>
### `WRITE` [*exp*]

The `WRITE` statement outputs the FOCAL program (or a portion of it) to the console in canonical format. This is the FOCAL equivalent to the `LIST` command in BASIC.

If no argument is provided, the entire program is listed.

If *exp* is provided, the output is filtered based on the value:
- If *exp* evaluates to an integer (e.g., `2` or `2.0`), all lines in that group are listed (e.g., `WRITE 2` lists lines 2.00 through 2.99)
- If *exp* evaluates to a non-integer (e.g., `2.1`), only that specific line is listed

#### Examples:
```
WRITE
```
Lists the entire program.

```
WRITE 2
```
Lists all lines in group 2 (2.00-2.99).

```
WRITE 2.5
```
Lists only line 2.50.

<!-- TOC --><a name="library-call"></a>
### `LIBRARY CALL` *filename*

The `LIBRARY CALL` statement loads a FOCAL program from disk and replaces the current program in memory. The *filename* should be a string constant representing the path to the FOCAL source file.

#### Examples:
```
LIBRARY CALL "myprogram.fc"
```

After a successful `LIBRARY CALL`, the newly loaded program replaces the current program. Execution does not automatically begin; the program can be run using the `GO` statement or by pressing Enter/Return at the CLI prompt.

<!-- TOC --><a name="library-save"></a>
### `LIBRARY SAVE` *filename*

The `LIBRARY SAVE` statement writes the current FOCAL program to the named file in canonical format. The *filename* should be a string constant representing the path where the file should be saved.

#### Examples:
```
LIBRARY SAVE "backup.fc"
```

This will write the current program to the specified file, overwriting any existing file with that name.

<!-- TOC --><a name="library-run"></a>
### `LIBRARY RUN` *filename*

The `LIBRARY RUN` statement loads a FOCAL program from disk and begins executing it immediately. The *filename* should be a string constant representing the path to the FOCAL source file.

#### Examples:
```
LIBRARY RUN "myprogram.fc"
```

Execution will continue with the newly loaded program, replacing the current program in memory.

DEC FOCAL also included a number of commands that performed disk actions, which are all proceeded by the `LIBRARY` command.

<!-- TOC --><a name="program-statements"></a>
## Program statements

This section explains the statements associated with loops, conditional and unconditional branches, subroutines, and similar functionality. It also explains the means of accessing data and the optional commands used for defining variables. The following list is not in alphabetical order; it is meant to group statements with similar functionality.

<!-- TOC --><a name="comment"></a>
### `COMMENT` and `CONTINUE`

`COMMENT` is used to insert comment text into the program code. Any text on the line after the `COMMENT` is ignored, and execution immediately moves to the next line.

In FOCAL, only the first letter of the statement keyword is examined, and the rest is ignored. For this reason, this statement is also sometimes used as a label in code, allowing you to target a line with `GOTO` or `DO` but leave it largely empty to make it more obvious. In this case, a `C` is considered to read `CONTINUE`, not `COMMENT`, and it is not uncommon to see `CONTINUE` in some programs.

<!-- TOC --><a name="set"></a>
### `SET` *var*`=`*expr*

The most common statement found in most programs is the assignment statement, with the keyword `SET`. This calculates the value of the expression *expr* and then assigns the result to *var*.

<!-- TOC --><a name="do"></a>
### `DO` {*lineno*|*groupno*}

`DO` is similar to `GOTO` in that it moves execution to the specified line number. However, the outcome is that control is transferred to the statement after the `DO` once the called line or group is complete. For instance, if one calls `DO 10`, execution will continue at the first line of group 10, progress through the rest of the lines in group 10, and then return to the statement after the `DO 10`. If one uses `DO 10.10`, execution will move to line 10.10 and then return when that single line has completed.

<!-- TOC --><a name="goto"></a>
### `GOTO` [*lineno*] and `GO`

FOCAL progresses through a program by performing statements one at a time, moving to the next statement when one is completed. In this respect, a program can be thought of as a single, long list of statements. This normally linear flow is interrupted by **branches**, statements that deliberately send the execution to another point in the program. In FOCAL, branches move to a **target**, which is indicated by a line number.

`GOTO` is the unconditional branch statement in FOCAL. It is followed by a line number, which can be either a specific line or a group. When the latter is used, execution moves to the first line in the group. If the line number is not valid, an error will be returned and execution will stop.

As FOCAL only reads the first letter of the keyword, this same instruction was also used in the shell to run programs. In this case it was almost always shortened to `GO` and no line number was provided. In this case, execution starts at the first line in the program.

In RetroFOCAL, `GO`ing occurs automatically after the program is loaded, so a `GO` with no line number found within the program text is a branch to the start of the problem.

<!-- TOC --><a name="erase"></a>
### `ERASE` {[*lineno*|*groupno*|ALL]}

A plain `ERASE` clears the stored values of all variables. In contrast to most programming languages of the era, FOCAL does not clear out variable values when the program is loaded or run. That means the values from the previous program or execution of the current program may still be in memory. For this reason, plain `ERASE` is still useful to clear variables before running a new program.

Additionally, a very different `ERASE` is suppored are the command level, which is used to erase lines from the program. There are three variations:

* `ERASE <lineno>` deletes the specified program line from the stored program.
* `ERASE <groupno>` deletes all lines in the specified group. In FOCAL, groups are indicated by the integer portion of the line number, e.g. `1` represents group `1.xx`.
* `ERASE ALL` deletes all stored program lines.

Line/group/ALL erasure is only supported from the interactive CLI prompt. If an `ERASE` statement with an argument is encountered during program execution, RetroFOCAL returns an error.

<!-- TOC --><a name="if"></a>
### `IF` (*exp*) *lineno*{;|[,*lineno*][{;|,*lineno*]}}

`IF` calculates the value of the expression *exp*. It then branches to one of the three optional line numbers following; if the value is negative it branches to the first number, the second if it is zero, and the third if it is positive. FOCAL does not include any logical expressions, to branch if the value of a variable is a particular value, one subtracts the value from the variable and then branches if the result is zero.

As many branches are really testing only one outcome, FOCAL allows the list to be shortened by leaving off options that are not used. This can be accomplished by ending the statement with a semicolon (the statement separator) or using a <newline> to end the line. In these cases, execution continues normally if that option is not included.

This peculiar style of *conditional branching* originates with FORTRAN, and may seem confusing to users more familiar with other languages like BASIC. It can also lead to confusing code as the only thing the `IF` statement can perform is a branch, you cannot use it to perform other statements as you can in most languages.

#### Examples:

    1.1 IF (25-25)2.1,2.3,2.5
    1.2 QUIT
    2.1 TYPE "LESS THAN ZERO"; QUIT
    2.3 TYPE "EQUAL TO ZERO"; QUIT
    2.5 TYPE "GREATER THAN ZERO"; QUIT

The test will always produce zero, so this program will output:

    EQUAL TO ZERO

`IF` statements are often used in loops. For instance:

    1.10 S A=9
    1.20 S A=A+1
    1.30 T A
    1.40 I (A-20)1.20
    1.50 Q
    
This causes the program to repeatedly add 1 to the current value of A, print the new value to the console, and then start again at line 1.20. This loop will continue until the value in A reaches 20, which will occur after 10 loops. When it does reach 20, the `IF` will then attempt to follow the second target, which does not exist, so it will continue on to the next line and end execution.

As FOCAL's `IF` only  supports branching, simple tasks may require several lines. For instance, this BASIC code:

    10 X=4
    20 IF X=4 THEN PRINT "X IS FOUR"
    30 END

Has no direct equivalent in FOCAL. Instead, one can use an `IF` to call the `TYPE` and skip over it otherwise:

    1.10 S X=4
    1.20 I (X-4)1.40,1.30,1.40
    1.30 T "X IS FOUR"
    1.40 Q

<!-- TOC --><a name="for"></a>
### `FOR` *var*=*expr1*,*expr2*[,*expr3*];*statmnt*[;*statmnt*...]

Another of the common statements found in FOCAL is the FOR loop. When this is encountered at runtime, the system calculates the value *aexpr1* and assigns it to *var* in the same fashion as as `SET` statement. It then performs the statements, if any, found after the semicolon. When those are completed, it adds the value *expr2*, known as the *step value*, to *var*. It then checks that the result in *var* is less than *expr3*, the limit. If *var* is less than the *limit*, it performs the statements and repeats this process. When *var* does eventually reach or exceed the limit, execution moves to the next line.

In the vast majority of real-world examples, the step value is 1, and thus redundant. In these cases the step value can be left out, in which case the second value is the limit instead of the step.

For those familiar with FOR loops in other languages like BASIC, it is important to note that in FOCAL (and in FORTRAN), the *step* is in the second parameter, not the third. This is a common source of confusion. Another is that `FOR` loops in FOCAL can only perform the statements on the line following, there is no equivalent of a `NEXT` or other way to form a *block* of lines to be performed. When this functionality is needed, it is normally handled by using a group and performing a `DO` as the action of the form loop.
    
#### Examples:

This code will produce five lines of output:

    1.1 SET A=100
    1.2 FOR B=1,1,5; TYPE %5.02, "8 IS" B+A,!

This program will produce:

    B IS =+101.00
    B IS =+102.00
    B IS =+103.00
    B IS =+104.00
    B IS =+105.00

This example leaves out the step value:

    1.1 FOR X=1,5; DO 2.0
    1.2 GOTO 3.1
    2.1 TYPE! "      " %3, "X "X
    2.2 SET A=X+100.000
    2.3 TYPE ! "     " %5.02, "A "A
    3.1 QUIT

This will produce:

    X =+  1
    A =+101.00
    X=+   2
    A =+102.00
    X=+   3
    A =+103.00
    X=+   4
    A =+104.00
    X=+   5
    A =+105.00

<!-- TOC --><a name="quit"></a>
### `QUIT`

`QUIT` stops the execution of the program and exits RetroFOCAL, returning you to the console shell. `QUIT` is optional, but adding it was considered good form. `QUIT` does not have to be at the end of the source code, it was often found higher in the code with subroutines below it, preventing them from running unless explicitly called. `QUIT` is used in places where `STOP` or `END` would be seen in BASIC.

A RetroBASIC extension is that if `QUIT` is entered manually in the command line, RetroFOCAL exits and returns to the user shell. This works in a fashion similar to `BYE` seen in other languages.

<!-- TOC --><a name="return"></a>
### `RETURN`

FOCAL programs normally call subroutines using the `DO` statement, which automatically returns to the statement after the `DO` when the reaches the end of the referenced group or line. There are situations where the subroutine needs to exit earlier. `RETURN` is called in these situations.

As the lines of code making up subroutines are normal statements, programs might accidentally run them when they reach the end of the main program code. If this occurs, a `RETURN` in that subroutine may be called without a `DO` and an error will occur. To prevent this, programs generally place a `QUIT` statement immediately before the subroutines, or alternately, place the `QUIT` at a high line number like 99.99 and then `GOTO 99.99` at the end of the main code.

<!-- TOC --><a name="inputoutput-statements"></a>
## Input/Output Statements

This section describes the input/output statements that are used to access and display data.

<!-- TOC --><a name="ask"></a>
### `ASK` [*sexp*,]*var*[,*sexp*][,*var*][,...]

`ASK` is the primary statement for asking the user for data from the keyboard. When it is encountered in a program, execution pauses and a colon prompt, `:`, is displayed on the console to indicate the computer is waiting for input. If the optional *sexp* is included, that text will be printed in front of the colon as an additional prompt. The user then enters their response and indicates they are done by pressing the <return> key. The value that they typed in is then processed and assigned to corresponding *var*.

`ASK` can accept multiple inputs in a single statement, using a comma-separated list of variables. The user may enter the value on its own and press <return>, or they can enter multiple values on a single line by separating the values with commas. If the user types in fewer values then there are variables in the statement, another colon prompt will be displayed, and the process will repeat until a value has been received for each *var*. In contrast to most BASIC dialects, prompt text can be placed in front of any variable, not just at the start of the statement. 

#### Examples:

    1.10 T "Enter five numbers to be summed..."
    1.20 F I=1,5; D 2
    1.30 T "The total is "T!
    1.40 Q
    2.10 A "Number",V
    2.20 S T=T+V

When run, this example will produce the following:

    Enter five numbers to be summed...
    Number?

The user responds by typing a number and pressing <return>. This returns to the front of the `FOR` loop and repeats, so the complete interaction would be something like:

    Enter five numbers to be summed...
    Number? 5
    Number? 3
    Number? 1
    Number? 1
    Number? 7
    The total is 17

Using multiple variables in a single `ASK` has a similar effect. For instance:

    1.10 T "Enter three numbers to be summed..."
    1.20 A "Type three numbers separated by commas "A,B,C
    1.30 T "The total is "A+B+C

The difference here is the way that the user's input is interpreted. The user can type three values on one line, separated by commas, as noted in the prompt. They can also type a single value and press <return>, which will cause the `ASK` to display the colon and continue until all values are provided. In this example, the user types one value per line:

    Enter three numbers to be summed...
    Type three numbers separated by commas 3
    : 2
    : 1
    The total is 6

<!-- TOC --><a name="type"></a>
### `TYPE` [*exp*{|[,|!|#|:]},...]]

`TYPE` is used to produce output on the user's console. It is one of the most common statements found in most FOCAL programs. The statement is designed to be very flexible, able to output values from any type of expression. In the case of string expressions, the output is sent unchanged to the console. For numeric values, it is formatted to ensure very large or small numbers do not fill the screen. `TYPE` can accept any number of subsequent expressions, including zero. These expressions are separated with one of the three "print separators", the comma, the semicolon, or if the preceding expression is a string constant, nothing.

In contrast to most languages, including BASIC, FOCAL does *not* automatically insert a newline at the end of a `TYPE` statement. To do so, and most uses of `TYPE` will want to, `!` is used as a separator. There can be any number of these on a line. FOCAL also allowed `#` to perform a carriage return without a subsequent linefeed, which was often used with line printers. `:` output a tab, working in a fashion similar to the `TAB` function in BASIC.

#### Examples:

    TYPE "Hello, World!"!

The classic Hello, World! example in FOCAL. The interpreter will evaluate the expression, producing a string, and on determining it is a string, prints it as-is. In contrast to BASIC, the newline is not generated automatically, and has to be explicitly added with the trailing `!`.

    T 2+3+4!

In this example, the expression is numeric. The value will be calculated, formatted, and output. A newline will be printed as well. This will produce:

     9

Because FOCAL explicitly defined the newlines, you could have a single statement with multiple lines of output:

    T "This..."!"is"!" multiple-line"!"output"

<!-- TOC --><a name="operators"></a>
## Operators

RetroFOCAL supports the standard set of arithmetic operators: `+` for addition, `-` for subtraction, `*` for multiplication, `/` for division, and `^` for exponentiation.

FOCAL does not group operator precedence in the same fashion as other languages. In most languages, `^` has the highest precedence, followed by `*` and `/`, and finally `+` and `-`. In FOCAL, multiplication and division are two different levels, so it is `^`, `*`, `/`, with `+` and `-` at the same level.

Another difference is that FOCAL allows expressions to be bracketed with parentheses, `(` and `)`, square brackets, `[` and `]`, or "triangle brackets", `<` and `>`. This can be helpful when reading complex expressions. For instance: `SET A=<10*[5+1]*(1+5)>` will evaluate 5+1, 1+5, and then 10*6*6.

<!-- TOC --><a name="mathematical-functions"></a>
## Mathematical functions

<!-- TOC --><a name="fabs"></a>
### `FABS`(*exp*)

Returns the absolute value of a number without regard to whether it is positive or negative. The returned value is always positive.

<!-- TOC --><a name="fexp"></a>
### `FEXP`(*exp*)

Returns the value of *e* (approximately 2.7182828), raised to the power specified by the expression in parentheses. For instance, `EXP(3)`, returns 20.0855365.

<!-- TOC --><a name="fitr"></a>
### `FITR`(*exp*)

Returns the greatest integer less than or equal to the value of the expression. This is true whether the expression evaluates to a positive or negative number. Thus, `FITR(3.445)` returns 3, while `FITR(-3.445)` returns -4. This function is similar to the `floor` function found in most programming languages, or `INT` in most dialects of BASIC.

<!-- TOC --><a name="flog"></a>
### `FLOG`(*exp*)

Returns the natural logarithm of the number or expression. `FLOG(0)` gives an error, and `FLOG(1)` equals 0.

<!-- TOC --><a name="fran"></a>
### `FRAN`()

Returns a random number between -1 and +1.

#### Notes:

This random number generator varies rather significantly from most languages, which generally produce a value between 0 and 0.99999...

FOCAL does not require a dummy expression in the parens.

<!-- TOC --><a name="fsgn"></a>
### `FSGN`(*exp*)

Returns a — 1 if *exp* evaluates to a negative number, or a 1 if *exp* evaluates to a positive number, or zero if *exp* is exactly zero.

#### Notes:

FOCAL-69 evaluated to 1 if *exp* was zero. This was changed in FOCAL-71, which is what RetroBASIC implements. This may introduce subtle bugs in earlier programs.

<!-- TOC --><a name="fsqt"></a>
### `FSQT`(*exp*)

Returns the square root of the *exp* which must be positive. If *exp* is negative, an error will be raised.

<!-- TOC --><a name="trigonometric-functions"></a>
## Trigonometric functions

Trigonometric functions in FOCAL are carried out using radians. These functions were optional in PDP-8 FOCAL, but always available in RetroFOCAL.

<!-- TOC --><a name="fatn"></a>
### `FATN`(*exp*)

Returns the arctangent of the variable or expression in parentheses.

<!-- TOC --><a name="fcos"></a>
### `FCOS`(*exp*)

Returns the cosine of the expression in parentheses.

<!-- TOC --><a name="fsin"></a>
### `FSIN`(*exp*)

Returns the sine of the expression in parentheses.

<!-- TOC --><a name="character-functions"></a>
## Character functions

FOCAL includes only two string functions, added in FOCAL-71. It is important to note that these functions use ASCII values, and were added specifically to aid programs that needed to work with ASCII as it became universal by the late 1960s. This contrasts with the conversions that occur using the `ASK` command or assigning a string constant to a variable, in which case the value will be a 6-bit DEC character value. This means that one cannot `ASK A` and then test the value using these functions, they will return different values.

<!-- TOC --><a name="fin"></a>
### `FIN`(*scon*)

This function returns the ASCII code number for the first character of the string constant *scon*. For instance, `FIN("Hello")` returns 72, the decimal ASCII value for "H".

<!-- TOC --><a name="fout"></a>
### `FOUT`(*exp*)

Returns a single-character string, represented by the ASCII code number in parentheses. `FOUT(72)` writes "H".
