/*
 * test_C2_str_new_null.c
 * Regression test for review issue C2: str_new() dereferences NULL pointer
 *
 * str_new() is documented as accepting NULL but calls strlen(string)
 * without a NULL guard, causing a segmentation fault.
 *
 * This test forks a child process to safely detect the crash.
 * PASS: str_new(NULL) returns a valid empty string without crashing.
 * FAIL: str_new(NULL) crashes (segfault).
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "strng.h"

int main(void)
{
    printf("C2: str_new(NULL) dereference\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* Child: attempt the dangerous call */
        char *result = str_new(NULL);
        if (result == NULL) {
            /* Returned NULL instead of crashing -- acceptable fix */
            _exit(0);
        }
        if (result[0] == '\0') {
            /* Returned empty string -- also acceptable */
            free(result);
            _exit(0);
        }
        /* Unexpected non-empty result */
        _exit(2);
    }

    /* Parent: wait for child and check result */
    int status;
    waitpid(pid, &status, 0);

    if (WIFSIGNALED(status)) {
        printf("  FAIL: str_new(NULL) crashed with signal %d (%s)\n",
               WTERMSIG(status),
               WTERMSIG(status) == 11 ? "SIGSEGV" : "other");
        return 1;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("  PASS: str_new(NULL) handled safely\n");
        return 0;
    }

    printf("  FAIL: str_new(NULL) returned unexpected result (exit %d)\n",
           WEXITSTATUS(status));
    return 1;
}
