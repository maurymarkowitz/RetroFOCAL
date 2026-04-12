/*
 * test_C6_lst_insert_at_index.c
 * Regression test for review issue C6: lst_insert_at_index() corrupts
 * the list with a self-referencing prev pointer.
 *
 * The bug is on list.c line 334:
 *   new_node->prev = new_node;    // should be tmp_node->prev = new_node;
 *
 * This creates a cycle: the inserted node's prev points to itself,
 * causing infinite loops in any backward traversal.  Additionally,
 * the node that was at the target index still has its old prev pointer,
 * so the new node is unreachable when traversing backward.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "list.h"

int main(void)
{
    int failures = 0;

    printf("C6: lst_insert_at_index() self-referencing prev\n");

    /* Build a list: [10, 20, 30] */
    list_t *list = NULL;
    list = lst_append(list, (void *)(intptr_t)10);
    list = lst_append(list, (void *)(intptr_t)20);
    list = lst_append(list, (void *)(intptr_t)30);

    /* Insert 15 at index 1 (between 10 and 20).
     * Expected result: [10, 15, 20, 30] */
    list = lst_insert_at_index(list, (void *)(intptr_t)15, 1);

    /* Test 1: list should have 4 elements */
    int len = lst_length(list);
    if (len != 4) {
        printf("  FAIL: expected length 4, got %d\n", len);
        failures++;
    } else {
        printf("  PASS: list length is 4\n");
    }

    /* Test 2: the inserted node's prev should NOT point to itself */
    list_t *inserted = lst_node_at(list, 1);
    if (inserted == NULL) {
        printf("  FAIL: inserted node not found at index 1\n");
        failures++;
    } else if (inserted->prev == inserted) {
        printf("  FAIL: inserted node's prev points to itself (self-referencing cycle)\n");
        failures++;
    } else {
        printf("  PASS: inserted node's prev does not self-reference\n");
    }

    /* Test 3: backward traversal from the inserted node should reach the head
     * without looping. With the bug, this loops forever. */
    if (inserted != NULL) {
        list_t *walk = inserted;
        int steps = 0;
        while (walk->prev != NULL && steps < 100) {
            walk = walk->prev;
            steps++;
        }
        if (steps >= 100) {
            printf("  FAIL: backward traversal stuck in infinite loop\n");
            failures++;
        } else if (walk != lst_first_node(list)) {
            printf("  FAIL: backward traversal did not reach list head\n");
            failures++;
        } else {
            printf("  PASS: backward traversal reaches list head\n");
        }
    }

    /* Test 4: the node after the insertion should have its prev updated */
    list_t *after = lst_node_at(list, 2);
    if (after != NULL && inserted != NULL) {
        if (after->prev != inserted) {
            printf("  FAIL: node after insertion still has old prev (new node unreachable backward)\n");
            failures++;
        } else {
            printf("  PASS: node after insertion has correct prev\n");
        }
    }

    /* Test 5: verify data order [10, 15, 20, 30] */
    intptr_t expected[] = {10, 15, 20, 30};
    list_t *node = lst_first_node(list);
    for (int i = 0; i < 4 && node != NULL; i++, node = node->next) {
        intptr_t val = (intptr_t)node->data;
        if (val != expected[i]) {
            printf("  FAIL: element %d is %ld, expected %ld\n", i, (long)val, (long)expected[i]);
            failures++;
            break;
        }
    }
    if (failures == 0 || node != NULL) {
        /* Only print if we didn't already fail above */
    }

    lst_free(list);
    return failures > 0 ? 1 : 0;
}
