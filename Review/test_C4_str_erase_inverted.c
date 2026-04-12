/*
 * test_C4_str_erase_inverted.c
 * Regression test for review issue C4: str_erase() logic is inverted
 *
 * str_erase(string, pos, count) should delete 'count' characters
 * starting at 'pos', returning the remainder.
 *
 * The buggy implementation does:
 *   memmove(string, string + sp, no)
 * which KEEPS only the to-be-deleted range and discards the rest.
 *
 * Example: str_erase("ABCDEFG", 2, 3)
 *   Expected: "ABFG"   (C, D, E removed)
 *   Actual:   "CDE"    (only the deleted range is kept)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strng.h"

static int test_erase(const char *input, size_t pos, size_t count,
                      const char *expected, const char *description)
{
    char *s = str_new((char *)input);
    str_erase(s, pos, count);

    int pass = (strcmp(s, expected) == 0);
    printf("  %s: str_erase(\"%s\", %zu, %zu) => \"%s\" (expected \"%s\") - %s\n",
           pass ? "PASS" : "FAIL",
           input, pos, count, s, expected, description);
    free(s);
    return pass ? 0 : 1;
}

int main(void)
{
    int failures = 0;

    printf("C4: str_erase() inverted logic\n");

    failures += test_erase("ABCDEFG", 2, 3, "ABFG",
                           "delete 3 chars from middle");

    failures += test_erase("HELLO WORLD", 5, 1, "HELLOWORLD",
                           "delete 1 space from middle");

    failures += test_erase("ABCDEFG", 0, 2, "CDEFG",
                           "delete 2 chars from front");

    failures += test_erase("ABCDEFG", 5, 2, "ABCDE",
                           "delete 2 chars from end");

    return failures > 0 ? 1 : 0;
}
