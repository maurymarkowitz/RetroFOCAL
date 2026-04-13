/*
 * test_C9_lst_key_null.c
 * Regression test for review issue C9: lst_node_with_key and
 * lst_data_with_key return wrong node when a NULL key is encountered.
 *
 * The search loop in lst_node_with_key is:
 *   while (node != NULL && node->key != NULL && strcmp(...) != 0)
 *
 * This exits when node->key is NULL, and then returns that node --
 * even though its key doesn't match the search key.  If a node with
 * a NULL key exists between the search start and the target node,
 * the wrong node is returned.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "list.h"

int main(void)
{
    int failures = 0;

    printf("C9: lst_node_with_key returns wrong node on NULL key\n");

    /* Build a list with three nodes */
    list_t *list = NULL;
    list = lst_append(list, (void *)(intptr_t)100);
    list = lst_append(list, (void *)(intptr_t)200);
    list = lst_append(list, (void *)(intptr_t)300);

    /* Set keys: first and third have keys, middle has NULL key */
    list_t *n1 = lst_first_node(list);
    list_t *n2 = n1->next;
    list_t *n3 = n2->next;

    n1->key = "alpha";
    /* n2->key stays NULL (set by _lst_alloc) */
    n3->key = "gamma";

    /* Search for "gamma" -- should find n3 (data=300) */
    list_t *found_node = lst_node_with_key(list, "gamma");
    void *found_data = lst_data_with_key(list, "gamma");

    /* Test 1: lst_node_with_key should return n3, not n2 */
    if (found_node == NULL) {
        printf("  FAIL: lst_node_with_key returned NULL (key not found)\n");
        failures++;
    } else if (found_node == n2) {
        printf("  FAIL: lst_node_with_key returned the NULL-key node (data=%ld)\n",
               (long)(intptr_t)found_node->data);
        printf("        Search stopped at NULL key instead of continuing to find \"gamma\"\n");
        failures++;
    } else if (found_node == n3) {
        printf("  PASS: lst_node_with_key found the correct node (data=%ld)\n",
               (long)(intptr_t)found_node->data);
    } else {
        printf("  FAIL: lst_node_with_key returned unexpected node (data=%ld)\n",
               (long)(intptr_t)found_node->data);
        failures++;
    }

    /* Test 2: lst_data_with_key should return 300 */
    if (found_data == NULL) {
        printf("  FAIL: lst_data_with_key returned NULL\n");
        failures++;
    } else if ((intptr_t)found_data == 300) {
        printf("  PASS: lst_data_with_key returned correct data (300)\n");
    } else {
        printf("  FAIL: lst_data_with_key returned %ld, expected 300\n",
               (long)(intptr_t)found_data);
        failures++;
    }

    /* Test 3: searching for "alpha" should still work (it's before the NULL key) */
    found_data = lst_data_with_key(list, "alpha");
    if ((intptr_t)found_data == 100) {
        printf("  PASS: lst_data_with_key(\"alpha\") returned 100 (before NULL key, works)\n");
    } else {
        printf("  FAIL: lst_data_with_key(\"alpha\") returned %ld, expected 100\n",
               (long)(intptr_t)found_data);
        failures++;
    }

    /* Don't free keys -- they're string literals */
    n1->key = NULL;
    n3->key = NULL;
    lst_free(list);
    return failures > 0 ? 1 : 0;
}
