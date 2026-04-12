# Regression Test Results

**Date:** 2026-04-12
**Platform:** macOS, Apple Clang/GCC
**Branch:** claude-review

## How to Run

```bash
cd Review
make            # build C tests
./run_tests.sh  # run all tests
make asan       # rebuild with AddressSanitizer (recommended for C1, C3, C5)
```

## Results

| Test | Type | Result | Confirms Bug? |
|------|------|--------|---------------|
| **C1** - Negative array subscript | FOCAL | WARN (UB didn't crash) | Yes - code uses negative C array index |
| **C2** - `str_new(NULL)` | C (fork) | FAIL - SIGSEGV | Yes |
| **C3** - `str_new` overflow | C (fork) | FAIL - 400 chars in 257-byte buffer | Yes |
| **C4** - `str_erase` inverted | C | FAIL - all 4 cases wrong | Yes |
| **C5** - `str_append` overflow | C | FAIL - 400 chars in 257-byte buffer | Yes |
| **C6** - `lst_insert_at_index` | C | FAIL - self-ref + infinite loop | Yes |
| **C7** - FIN fall-through | C | FAIL - returns random, not 193 | Yes |
| **C8** - Wrong struct member | C | FAIL - `.number` still 12345.7 | Yes |
| **C9** - `lst_node_with_key` NULL | C | FAIL - returns wrong node | Yes |
| **C10** - `lst_copy` broken | C | FAIL - data is heap pointers | Yes |
| **C11** - Comment text lost | FOCAL | FAIL - text not in WRITE | Yes |

All 11 critical issues from `review.md` are confirmed.

## Test Descriptions

### FOCAL Tests

**C1 - test_C1_negative_array_subscript.fc**
Sets and reads `A(-1)` and `A(-2048)`. FOCAL allows subscripts from -2048 to
+2047, but the C array backing storage starts at index 0. Negative subscripts
index before the allocation (heap underflow). On this platform the undefined
behavior did not manifest as a crash, but the bug is confirmed by code
inspection and would be caught by AddressSanitizer.

**C11 - test_C11_comment_text_lost.fc**
Stores a COMMENT with text, then uses WRITE to list the program. The lexer
computes `yyleng - strlen(yytext)` which is always 0, so all comment text is
discarded. WRITE output shows `COMMENT` with no text after it.

### C Tests

**C2 - test_C2_str_new_null.c**
Calls `str_new(NULL)` in a forked child. The function calls `strlen(string)`
with no NULL guard, causing SIGSEGV.

**C3 - test_C3_str_new_overflow.c**
Calls `str_new()` with a 400-character string. The function allocates 257
bytes (`MAXSTRING+1`) and uses `strcpy` with no bounds check. The child
process detects the overflow (or crashes from heap corruption).

**C4 - test_C4_str_erase_inverted.c**
Calls `str_erase("ABCDEFG", 2, 3)`. Expected result is `"ABFG"` (middle
removed). Actual result is `"CDE"` (only the deleted range is kept). Tests
four different cases; all four produce inverted results.

**C5 - test_C5_str_append_overflow.c**
Appends 200 characters onto a 200-character string in a 257-byte buffer.
`str_append` is just `strcat` with no bounds check. The resulting 400-character
string confirms the overflow.

**C6 - test_C6_lst_insert_at_index.c**
Inserts into a list at index 1. Line 334 of `list.c` sets
`new_node->prev = new_node` instead of `tmp_node->prev = new_node`. The test
detects the self-referencing prev pointer and the resulting infinite loop in
backward traversal.

**C7 - test_C7_fin_fallthrough.c**
Reproduces the switch/case structure from `evaluate_expression`. The arity-0
`FIN` case reads a character but has no `break`, so it falls through to `FRAN`
which overwrites the result with a random number. The test calls FIN and checks
whether it returns 193 (correct) or a random value (buggy).

**C8 - test_C8_wrong_union_member.c**
Reproduces the `value_t` struct layout. In the numeric branch, when the
variable lookup returns NULL, the code sets `result.string = 0` instead of
`result.number = 0`. Since these are separate struct fields at different
offsets (8 vs 16), `.number` retains its stale value.

**C9 - test_C9_lst_key_null.c**
Builds a list where the middle node has a NULL key. Searches for a key that
exists on the third node. The search loop exits early when it hits the NULL
key and returns the wrong node (data=200 instead of data=300).

**C10 - test_C10_lst_copy_broken.c**
Copies a three-element list `[10, 20, 30]`. The copy's second and third
elements contain heap pointers (the `list_t` node structs) instead of the
original integer data, because `lst_append(new_list, new_node)` stores the
node struct as data rather than `new_node->data`.

## Files

```
Review/
  review.md                              # full code review
  test_results.md                        # this file
  Makefile                               # builds C tests
  run_tests.sh                           # runs all tests
  test_C1_negative_array_subscript.fc    # FOCAL
  test_C11_comment_text_lost.fc          # FOCAL
  test_C2_str_new_null.c                 # C
  test_C3_str_new_overflow.c             # C
  test_C4_str_erase_inverted.c           # C
  test_C5_str_append_overflow.c          # C
  test_C6_lst_insert_at_index.c          # C
  test_C7_fin_fallthrough.c              # C
  test_C8_wrong_union_member.c           # C
  test_C9_lst_key_null.c                 # C
  test_C10_lst_copy_broken.c             # C
```
