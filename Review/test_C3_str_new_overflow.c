/*
 * test_C3_str_new_overflow.c
 * Regression test for review issue C3: str_new() buffer overflow on long input
 *
 * str_new() allocates MAXSTRING+1 (257) bytes then copies the input
 * with strcpy, which has no length limit.  An input longer than 256
 * characters overflows the heap buffer.
 *
 * This test uses fork() to safely detect the crash caused by heap
 * corruption from the overflow.
 *
 * For detailed diagnostics, compile with AddressSanitizer:
 *   gcc -fsanitize=address -I../src test_C3_str_new_overflow.c ../src/strng.c -o test_C3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "strng.h"

int main(void)
{
    printf("C3: str_new() buffer overflow on long input\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Child: attempt the overflow */
        char long_string[401];
        memset(long_string, 'X', 400);
        long_string[400] = '\0';

        char *result = str_new(long_string);
        size_t len = strlen(result);

        if (len > MAXSTRING) {
            /* Overflow occurred: strcpy wrote past the allocation.
             * Exit with code 2 to signal the overflow was detected. */
            _exit(2);
        }

        /* str_new truncated properly (fixed version) */
        free(result);
        _exit(0);
    }

    /* Parent: check what happened */
    int status;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
        printf("  FAIL: str_new overflowed the heap buffer and crashed (signal %d)\n",
               WTERMSIG(status));
        printf("        strcpy wrote 401 bytes into a %d-byte buffer\n", MAXSTRING + 1);
        return 1;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code == 2) {
            printf("  FAIL: str_new accepted 400 chars into a %d-byte buffer (overflow)\n",
                   MAXSTRING + 1);
            return 1;
        }
        if (code == 0) {
            printf("  PASS: str_new truncated input to MAXSTRING\n");
            return 0;
        }
    }

    printf("  FAIL: unexpected child status %d\n", status);
    return 1;
}
