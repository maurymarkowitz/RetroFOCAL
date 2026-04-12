/*
 * test_C7_fin_fallthrough.c
 * Regression test for review issue C7: missing break in FIN case
 *
 * In retrofocal.c evaluate_expression(), the arity-0 FIN case reads
 * a character but has no break statement, so execution falls through
 * to the FRAN case, which overwrites the result with a random number.
 *
 * This test reproduces the switch/case structure in isolation.
 */

#include <stdio.h>
#include <stdlib.h>

/* Token values from parse.h */
#define FIN_TOKEN  299
#define FRAN_TOKEN 290

/*
 * Reproduces the buggy switch from retrofocal.c lines 515-536.
 * With the bug, FIN falls through to FRAN.
 */
static double evaluate_buggy(int opcode)
{
    double result = -999.0;

    switch (opcode) {
        case FIN_TOKEN:
        {
            /* Simulate reading char 'A' (the real code calls getchar()) */
            char c = 'A';
            result = (int)c + 128;  /* Should be 193 */
        }
        /* BUG: no break here -- falls through to FRAN */

        case FRAN_TOKEN:
            result = ((double)rand() / (double)RAND_MAX);
            break;

        default:
            result = -1.0;
    }
    return result;
}

/*
 * The fixed version, for comparison.
 */
static double evaluate_fixed(int opcode)
{
    double result = -999.0;

    switch (opcode) {
        case FIN_TOKEN:
        {
            char c = 'A';
            result = (int)c + 128;
        }
            break;  /* FIX: break prevents fall-through */

        case FRAN_TOKEN:
            result = ((double)rand() / (double)RAND_MAX);
            break;

        default:
            result = -1.0;
    }
    return result;
}

int main(void)
{
    int failures = 0;

    printf("C7: FIN case missing break (falls through to FRAN)\n");

    srand(42);  /* Fixed seed for reproducibility */

    double buggy_result = evaluate_buggy(FIN_TOKEN);
    double fixed_result = evaluate_fixed(FIN_TOKEN);

    /* The correct FIN result for 'A' is 65 + 128 = 193 */
    if (buggy_result == 193.0) {
        printf("  PASS: FIN returned 193 (no fall-through)\n");
    } else {
        printf("  FAIL: FIN returned %g instead of 193 (fell through to FRAN)\n",
               buggy_result);
        failures++;
    }

    /* Verify our reference implementation is correct */
    if (fixed_result != 193.0) {
        printf("  NOTE: fixed version returned %g (test logic error)\n",
               fixed_result);
        failures++;
    }

    return failures > 0 ? 1 : 0;
}
