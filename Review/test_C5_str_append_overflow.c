/*
 * test_C5_str_append_overflow.c
 * Regression test for review issue C5: str_append() has no bounds checking
 *
 * str_append() is just strcat() with no length check.  Strings from
 * str_new() are fixed at MAXSTRING+1 (257) bytes.  Appending enough
 * data to exceed that limit overflows the heap buffer.
 *
 * This test detects the overflow by checking whether the resulting
 * string exceeds MAXSTRING.  If it does, strcat wrote past the
 * allocation.
 *
 * For definitive detection, compile and run with AddressSanitizer:
 *   gcc -fsanitize=address -I../src test_C5_str_append_overflow.c ../src/strng.c -o test_C5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strng.h"

int main(void)
{
    printf("C5: str_append() buffer overflow\n");

    /* Start with a 200-char string, then append 200 more */
    char base[201];
    memset(base, 'A', 200);
    base[200] = '\0';

    char addition[201];
    memset(addition, 'B', 200);
    addition[200] = '\0';

    char *s = str_new(base);       /* 257-byte buffer, 200 chars used */
    str_append(s, addition);        /* tries to write 400 chars total */

    size_t len = strlen(s);

    if (len > MAXSTRING) {
        printf("  FAIL: str_append produced %zu-char string in a %d-byte buffer (overflow)\n",
               len, MAXSTRING + 1);
        free(s);
        return 1;
    }

    printf("  PASS: str_append respected buffer bounds (%zu chars)\n", len);
    free(s);
    return 0;
}
