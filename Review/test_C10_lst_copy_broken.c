/*
 * test_C10_lst_copy_broken.c
 * Regression test for review issue C10: lst_copy() is fundamentally broken.
 *
 * Bug 1: The loop copies list->data BEFORE advancing list = list->next,
 *         so the first element's data is duplicated.
 * Bug 2: The loop's exit condition (list->next != NULL) skips the last
 *         element entirely.
 * Bug 3: lst_append(new_list, new_node) passes the list_t* node struct
 *         as the data pointer, instead of the node's actual data.
 *
 * For a source list [10, 20, 30], the copy should also be [10, 20, 30].
 * With the bugs, the copy contains node-struct pointers as data, the
 * first element is duplicated, and the last element is lost.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "list.h"

int main(void)
{
    int failures = 0;

    printf("C10: lst_copy() broken\n");

    /* Build a source list: [10, 20, 30] */
    list_t *orig = NULL;
    orig = lst_append(orig, (void *)(intptr_t)10);
    orig = lst_append(orig, (void *)(intptr_t)20);
    orig = lst_append(orig, (void *)(intptr_t)30);

    /* Copy it */
    list_t *copy = lst_copy(orig);

    /* Test 1: copy should have the same length */
    int orig_len = lst_length(orig);
    int copy_len = lst_length(copy);
    if (copy_len != orig_len) {
        printf("  FAIL: original has %d elements, copy has %d\n",
               orig_len, copy_len);
        failures++;
    } else {
        printf("  PASS: copy length matches original (%d)\n", copy_len);
    }

    /* Test 2: each element's data should match the original */
    list_t *o = lst_first_node(orig);
    list_t *c = lst_first_node(copy);
    int index = 0;

    while (o != NULL && c != NULL) {
        intptr_t orig_val = (intptr_t)o->data;
        intptr_t copy_val = (intptr_t)c->data;

        if (orig_val != copy_val) {
            printf("  FAIL: element %d: original=%ld, copy=%ld",
                   index, (long)orig_val, (long)copy_val);

            /* Check if the copy's data looks like a pointer to a list node
             * (Bug 3: lst_append stores the node struct as data) */
            if (copy_val > 1000) {
                printf(" (looks like a pointer, not the original value)");
            }
            printf("\n");
            failures++;
        } else {
            printf("  PASS: element %d matches (%ld)\n", index, (long)orig_val);
        }

        o = o->next;
        c = c->next;
        index++;
    }

    /* Check for leftover elements */
    if (o != NULL) {
        printf("  FAIL: original has more elements than copy (last element lost)\n");
        failures++;
    }
    if (c != NULL) {
        printf("  FAIL: copy has more elements than original\n");
        failures++;
    }

    lst_free(orig);
    lst_free(copy);
    return failures > 0 ? 1 : 0;
}
