#
# Makefile for RetroFOCAL regression tests
# Builds C tests for critical issues identified in review.md
#
# Usage:
#   make          - build all C tests
#   make run      - build and run all tests
#   make clean    - remove built test binaries
#   make asan     - build with AddressSanitizer (for C3, C5 overflow tests)
#

CC = gcc -g
CFLAGS = -I../src -Wall -Wno-unused-function
SRC = ../src

C_TESTS = test_C2_str_new_null \
          test_C3_str_new_overflow \
          test_C4_str_erase_inverted \
          test_C5_str_append_overflow \
          test_C6_lst_insert_at_index \
          test_C7_fin_fallthrough \
          test_C8_wrong_union_member \
          test_C9_lst_key_null \
          test_C10_lst_copy_broken

all: $(C_TESTS)

# String library tests -- link against strng.c
test_C2_str_new_null: test_C2_str_new_null.c $(SRC)/strng.c
	$(CC) $(CFLAGS) $^ -o $@

test_C3_str_new_overflow: test_C3_str_new_overflow.c $(SRC)/strng.c
	$(CC) $(CFLAGS) $^ -o $@

test_C4_str_erase_inverted: test_C4_str_erase_inverted.c $(SRC)/strng.c
	$(CC) $(CFLAGS) $^ -o $@

test_C5_str_append_overflow: test_C5_str_append_overflow.c $(SRC)/strng.c
	$(CC) $(CFLAGS) $^ -o $@

# List library tests -- link against list.c
test_C6_lst_insert_at_index: test_C6_lst_insert_at_index.c $(SRC)/list.c
	$(CC) $(CFLAGS) $^ -o $@

test_C9_lst_key_null: test_C9_lst_key_null.c $(SRC)/list.c
	$(CC) $(CFLAGS) $^ -o $@

test_C10_lst_copy_broken: test_C10_lst_copy_broken.c $(SRC)/list.c
	$(CC) $(CFLAGS) $^ -o $@

# Standalone tests -- no library dependencies
test_C7_fin_fallthrough: test_C7_fin_fallthrough.c
	$(CC) -Wall $< -o $@

test_C8_wrong_union_member: test_C8_wrong_union_member.c
	$(CC) -Wall $< -o $@

# Build with AddressSanitizer for overflow detection
asan: CFLAGS += -fsanitize=address -fno-omit-frame-pointer
asan: CC += -fsanitize=address
asan: all

# Run all tests
run: all
	@./run_tests.sh

clean:
	rm -f $(C_TESTS)

.PHONY: all clean run asan
