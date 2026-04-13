/*
 * test_C8_wrong_union_member.c
 * Regression test for review issue C8: result.string = 0 should be
 * result.number = 0
 *
 * In retrofocal.c line 502, when a numeric variable lookup returns
 * NULL, the code sets result.string = 0 instead of result.number = 0.
 *
 * value_t is a struct (not a union), so .string and .number are at
 * different offsets.  Setting .string = 0 does NOT zero .number.
 * The .number field retains whatever stale value was there before.
 *
 * This test reproduces the struct layout and demonstrates that
 * setting .string does not affect .number.
 */

#include <stdio.h>
#include <string.h>

/* Reproduce the value_t struct from retrofocal.c */
typedef struct {
    int type;
    char *string;
    double number;
} value_t;

int main(void)
{
    int failures = 0;

    printf("C8: wrong union/struct member assignment\n");

    /* Simulate the variable evaluation code path */
    value_t result;

    /* Set number to a known non-zero value, as would happen from
     * a previous evaluation or stack garbage */
    result.type = 0;
    result.number = 12345.6789;
    result.string = (char *)0xDEAD;

    /* This is the buggy code path: numeric type, but p is NULL */
    int is_string = 0;  /* type != STRING */
    void *p = NULL;     /* variable_value returned NULL */

    if (is_string) {
        if (p)
            result.string = "hello";
        else
            result.string = "";
    } else {
        if (p)
            result.number = 42.0;
        else
            result.string = 0;  /* BUG: should be result.number = 0 */
    }

    /* Check: did .number get zeroed? */
    if (result.number == 0.0) {
        printf("  PASS: result.number is 0 after the assignment\n");
    } else {
        printf("  FAIL: result.number is %g (stale value), expected 0\n",
               result.number);
        printf("        Setting .string = 0 does not clear .number because\n");
        printf("        they are separate struct fields at different offsets.\n");
        printf("        .string is at offset %zu, .number is at offset %zu\n",
               (size_t)((char *)&result.string - (char *)&result),
               (size_t)((char *)&result.number - (char *)&result));
        failures++;
    }

    return failures > 0 ? 1 : 0;
}
