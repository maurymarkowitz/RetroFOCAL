# RetroFOCAL Code Review

**Reviewer:** Claude (automated review)
**Date:** 2026-04-12
**Branch:** claude-review
**Scope:** All hand-written source files in `src/` plus `parse.y` and `scan.l`
**Focus:** Pointer safety, memory management, correctness

## Summary

RetroFOCAL is a well-structured FOCAL interpreter built on lex/yacc with a
custom linked-list and string library replacing GLib. The overall architecture
is sound and the code is readable. However, the review uncovered several
critical memory-safety bugs -- including a heap-corruption bug in array
indexing, a broken linked-list copy function, a broken string-erase function,
and multiple missing NULL checks -- along with a number of warning-level
issues involving memory leaks and logic errors.

**Recommendation:** Request changes. The critical items below involve
out-of-bounds writes, use-after-free patterns, and corrupted data structures
that can crash or silently corrupt interpreter state.

---

## Critical Issues

### C1. Heap corruption: negative array subscript in `variable_value`
**File:** `retrofocal.c`, around line 197-207

FOCAL allows array subscripts from -2048 to +2047. The storage is allocated as
`calloc(4096, sizeof(storage->value[0]))`, giving indices 0..4095. But the
subscript value is used directly as the index:

```c
index = this_index.number;
...
return &storage->value[index];
```

When the subscript is negative (e.g., -100), this performs
`&storage->value[-100]`, which reads/writes memory *before* the allocated
block. This is a heap buffer underflow and will silently corrupt memory or
crash.

**Fix:** Offset the index so that -2048 maps to 0:

```c
index = (int)this_index.number + 2048;
```

---

### C2. `str_new()` dereferences NULL pointer
**File:** `strng.c`, line 33

```c
char* str_new(char *string)
{
  char *newstr = calloc((MAXSTRING + 1), sizeof(char));
  ...
  if (strlen(string) > 0)   // <-- crashes if string is NULL
```

The header documents the parameter as "The string to copy from, or NULL," but
the implementation calls `strlen(string)` with no NULL guard. Every caller that
might pass NULL triggers undefined behavior.

**Fix:** Add `if (string != NULL && strlen(string) > 0)` or, better,
`if (string != NULL && string[0] != '\0')`.

---

### C3. `str_new()` buffer overflow on long input
**File:** `strng.c`, line 34

```c
strcpy(newstr, string);
```

The destination buffer is `MAXSTRING + 1` (257) bytes. If `string` is longer
than 256 bytes, `strcpy` overflows the buffer. This is an unconditional heap
buffer overflow.

**Fix:** Use `strncpy(newstr, string, MAXSTRING); newstr[MAXSTRING] = '\0';`
or switch to dynamic sizing with `strlen(string) + 1`.

---

### C4. `str_erase()` logic is inverted
**File:** `strng.c`, lines 133-149

The function is documented as "Deletes characters from a string starting at
`starting_pos` and running for `no_of_chars`." But the implementation
*keeps* only the deleted range and discards everything else:

```c
memmove(string, string + sp, no);  // copies the TO-BE-DELETED chars to front
string[no] = '\0';
```

For example, `str_erase("HELLO WORLD", 5, 1)` should produce `"HELLOWORLD"`
but instead produces `" "` (a single space).

**Fix:** The correct implementation should be:

```c
memmove(string + sp, string + sp + no, len - sp - no + 1);
```

Additionally, when `no_of_chars` is 0, the expression `ep = starting_pos +
no_of_chars - 1` underflows because `size_t` is unsigned, producing SIZE_MAX.
The bounds clamp catches this, but `no` then becomes `len + 1`, and the
`memmove` copies one byte past the end of the string.

---

### C5. `str_append()` has no bounds checking -- buffer overflow
**File:** `strng.c`, lines 179-183

```c
char* str_append(char *orig_string, char *new_chars)
{
  return strcat(orig_string, new_chars);
}
```

Strings from `str_new()` are fixed at 257 bytes. `strcat` performs no length
check. Any concatenation that exceeds MAXSTRING characters overflows the heap
buffer.

**Fix:** Either bounds-check and truncate, or switch to dynamically-sized
strings.

---

### C6. `lst_insert_at_index()` corrupts list with self-referencing pointer
**File:** `list.c`, line 334

```c
new_node->prev = tmp_node->prev;
new_node->prev->next = new_node;
new_node->next = tmp_node;
new_node->prev = new_node;     // BUG: should be tmp_node->prev = new_node
```

The last line sets `new_node->prev` to point to itself, overwriting the
correct prev pointer set two lines earlier. This creates a self-referencing
cycle that will cause infinite loops in any traversal.

**Fix:** Change line 334 to `tmp_node->prev = new_node;`.

---

### C7. Missing `break` in `FIN` case -- fall-through to `FRAN`
**File:** `retrofocal.c`, lines 523-531

```c
case FIN:
{
    char c = getchar();
    result.number = (int)c + 128;
}                                   // <-- no break!
case FRAN:
    result.number = ((double)rand() / (double)RAND_MAX);
    break;
```

Every arity-0 `FIN()` call reads a character but then immediately falls
through and overwrites the result with a random number. The character input
is silently discarded.

**Fix:** Add `break;` after the FIN block.

---

### C8. Wrong union member assigned: `result.string = 0` should be `result.number = 0`
**File:** `retrofocal.c`, line 502

```c
else {
    if (p)
        result.number = p->number;
    else
        result.string = 0;    // BUG: should be result.number = 0
}
```

This is in the numeric branch of variable evaluation (the `else` clause is
for `type != STRING`). Setting `.string = 0` zeros the wrong union member.
On most platforms this accidentally works because the union members overlap,
but it is technically undefined behavior and clearly a typo.

**Fix:** Change to `result.number = 0;`.

---

### C9. `lst_node_with_key` / `lst_data_with_key` return wrong node on NULL key
**File:** `list.c`, lines 203 and 218-219

```c
while (node != NULL && node->key != NULL && strcmp(key, node->key) != 0)
    node = node->next;
return node;
```

The loop exits in three cases: (a) node is NULL, (b) node->key is NULL,
(c) key matches. In case (b), the function returns a node whose key does
*not* match the search key. For `lst_data_with_key`, this returns the data
from the wrong node.

This can happen if nodes were inserted without keys, or if the list was
built by code that doesn't set keys on every node.

**Fix:** After the loop, check `if (node != NULL && node->key != NULL &&
strcmp(key, node->key) == 0)` before returning.

---

### C10. `lst_copy()` is fundamentally broken
**File:** `list.c`, lines 384-412

Two bugs:

1. **First element duplicated, last element lost.** The loop body copies
   `list->data` before advancing `list = list->next`, so the first element's
   data is set both in `new_list` (line 400) and in the first loop-created
   node (line 405).

2. **Stores list nodes as data instead of data.** `lst_append(new_list,
   new_node)` passes the `list_t*` node struct as the data pointer, rather
   than `new_node->data`. This builds a list where every element's `data`
   field is a `list_t*` instead of the original user data.

This function is currently unused in the interpreter, so this is not
triggering bugs today, but it would crash any future caller.

**Fix:** Rewrite the loop:
```c
list_t *new_list = NULL;
list_t *src = lst_first_node(list);
while (src != NULL) {
    new_list = lst_append(new_list, src->data);
    src = src->next;
}
return new_list;
```

---

### C11. COMMENT text is always lost (lexer bug)
**File:** `scan.l`, line 62

```c
C.*|COMMENT.*|CONTINUE.* {
    yylval.s = str_new(str_copy(yytext, yyleng - strlen(yytext)));
    ...
}
```

`yyleng == strlen(yytext)` always, so `yyleng - strlen(yytext)` is always 0.
`str_copy(yytext, 0)` returns an empty string. The actual comment text
(everything after C/COMMENT/CONTINUE) is discarded.

**Fix:** The intent is to skip past the keyword. One approach:

```c
const char *text = yytext;
if (strncasecmp(text, "CONTINUE", 8) == 0) text += 8;
else if (strncasecmp(text, "COMMENT", 7) == 0) text += 7;
else text += 1;  /* just 'C' */
yylval.s = str_new((char *)text);
```

---

## Warning Issues

### W1. DO/RETURN stack entries leak memory
**File:** `retrofocal.c`, lines 1254, 1321-1322, 1325-1326

When a DO completes (either via explicit RETURN or implicit end-of-group
return), `lst_remove_node_with_data` frees the list node but the
`stackentry_t` data is never freed:

```c
// RETURN handler:
interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
// missing: free(se);

// End-of-group DO handler (two places):
interpreter_state.stack = lst_remove_node_with_data(interpreter_state.stack, se);
// missing: free(se);
```

Compare with the FOR completion code at line 1308-1309 which correctly calls
`free(se)` after removal. The DO paths omit this.

**Fix:** Add `free(se);` after each `lst_remove_node_with_data` call in the
DO/RETURN paths.

---

### W2. `ERASE ALL` in CLI mode leaks all program memory
**File:** `cli.c`, lines 87-91

```c
if (strncasecmp(p, "ALL", 3) == 0 ...) {
    for (int i = 0; i < MAXLINE; i++)
        interpreter_state.lines[i] = NULL;   // leaked!
```

The statement lists, their statement data, expression trees, and variable
names are all leaked. Similarly, individual line deletion at lines 119-125
and 147 sets pointers to NULL without freeing.

**Fix:** Call `lst_free(interpreter_state.lines[i])` (and ideally free the
statement data within) before setting to NULL.

---

### W3. `write_program` produces empty output for MODIFY
**File:** `retrofocal.c`, line 1007 calls `write_program(index, index)`

The loop in `write.c` line 421 is `for (int i = start_line; i < end_line ...)`.
When `start_line == end_line`, the loop body never executes and MODIFY prints
nothing.

**Fix:** Call `write_program(index, index + 1)`.

---

### W4. `delete_lines` leaves dangling pointers
**File:** `retrofocal.c`, lines 1369-1375

```c
static void delete_lines() {
    for(int i = MAXLINE - 1; i >= 0; i--) {
        if (interpreter_state.lines[i] != NULL) {
            lst_free(interpreter_state.lines[i]);
            // lines[i] is now dangling
        }
    }
}
```

After `lst_free`, the pointer in `lines[i]` is stale. Any subsequent access
is use-after-free. Additionally, `lst_free` only frees list nodes, not the
`statement_t` data within them, so all statement data leaks.

**Fix:** Set `interpreter_state.lines[i] = NULL` after freeing.

---

### W5. `lst_pop` cannot return the new list head
**File:** `list.c`, lines 540-543

```c
void* lst_pop(list_t *list) {
    return lst_remove_node_at_index(list, 0);
}
```

This frees the first node and returns its data, but the caller's `list`
pointer still points to the now-freed node. The next access through that
pointer is use-after-free. The function signature makes it impossible to
return both the data and the new head.

**Fix:** Either change the signature to `void* lst_pop(list_t **list)` so the
head pointer can be updated, or document that callers must use
`lst_remove_node_at_index` directly.

---

### W6. `LIBRARY CALL/RUN` during execution risks use-after-free
**File:** `retrofocal.c`, lines 1169-1239

`LIBRARY CALL` clears all `interpreter_state.lines[]`, loads a new program,
and calls `interpreter_post_parse()`. But the runtime stack still contains
`stackentry_t` entries with `head` and `returnpoint` pointers into the
old (now-freed) program. If any DO or FOR is active when LIBRARY CALL
executes, subsequent RETURN or loop completion will dereference freed memory.

**Fix:** Clear the runtime stack before loading a new program:
```c
lst_free_everything(interpreter_state.stack);
interpreter_state.stack = NULL;
```

---

### W7. `lst_insert_with_key_sorted` returns non-head node
**File:** `list.c`, line 378

The function returns `new_node`, which may be in the middle or end of the
list. Callers like `variable_value` store this as their list handle:

```c
interpreter_state.variable_values =
    lst_insert_with_key_sorted(..., storage, storage_name);
```

This works only because all list functions call `lst_first_node()` to rewind,
but it's fragile and makes every subsequent list operation O(n) slower than
necessary because it must first walk backward to the head.

**Fix:** Return `lst_first_node(new_node)` instead of `new_node`.

---

### W8. Case `0` fall-through in option parsing
**File:** `main.c`, lines 108-112

```c
case 0:
    if (program_options[option_index].flag != 0)
        break;
    // falls through to case 'h' when flag == 0
case 'h':
```

If a flag-setting option has `.flag == NULL` (which doesn't happen with the
current option table), it would unintentionally print help and exit. A
`break` after the `if` block would be safer.

---

### W9. Division by zero continues execution
**File:** `retrofocal.c`, lines 642-644

```c
case '/':
    if (b == 0)
        focal_error("Division by zero");
    result = double_to_value(a / b);   // still executes, producing inf/nan
```

`focal_error` prints a message but does not stop execution. The division
proceeds, producing `inf` or `nan`, which propagates through subsequent
calculations.

**Fix:** Either return a default value (0.0) after the error, or set a flag
to halt execution.

---

### W10. `str_copy` does not check for NULL input
**File:** `strng.c`, lines 42-55

```c
char* str_copy(const char *string, size_t no_of_chars)
{
    size_t len = 0;
    while (len < no_of_chars && string[len])  // crashes if string is NULL
```

**Fix:** Add `if (string == NULL) return NULL;` at the top.

---

## Nits

- **`number_to_string` static buffer** (`retrofocal.c:251`): Returns a pointer
  to a `static char[40]`. This is fine for single-threaded use but would be a
  hazard if the interpreter were ever made reentrant.

- **`sprintf` vs `snprintf` inconsistency** (`retrofocal.c:845`): Most places
  in `find_line` use `snprintf` but one uses `sprintf`. Should be `snprintf`
  for consistency.

- **`lst_free_everything` TODO comment** (`list.c:85`): The comment notes that
  "the original list still has one invalid node" after freeing. This is a known
  issue that should be resolved by having the caller set their pointer to NULL
  after calling, which `delete_variables` already does.

- **Mixed tabs and spaces**: Several files (notably `retrofocal.c`) mix tabs
  and spaces for indentation, making some blocks appear misaligned depending
  on tab width settings.

- **`elapsed_jiffies` is unused**: The function at `retrofocal.c:432` is
  defined but never called.

- **Redundant `extern` declarations in `cli.c`**: Several items declared
  `extern` in `cli.c` (lines 33-42) are already available through the
  included headers.

---

## Questions

1. **Is `str_erase` used anywhere?** If it's only used internally for the
   fruncate/truncate operations, the inverted logic may be masked. But if
   any caller depends on the documented "delete from middle" behavior, it
   will silently produce wrong results.

2. **Is `lst_copy` used anywhere?** If not, it's dead code that should either
   be fixed or removed. If it is used, the data corruption described in C10
   is actively occurring.

3. **What is the intended behavior when `focal_error` is called?** Currently
   it prints a message and returns, allowing execution to continue with
   potentially invalid state (e.g., after division by zero, undefined line
   references). Should it halt the current statement? Halt the program?

4. **Is the COMMENT text intentionally discarded?** The lexer rule (C11)
   produces an empty string for all comments. If this is intentional (comments
   are just markers), the `write.c` code that tries to reconstruct comment
   text is dead. If unintentional, comment round-tripping (WRITE after
   LIBRARY CALL) loses all comment text.
