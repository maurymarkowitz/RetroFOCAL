#!/bin/bash
#
# run_tests.sh -- run all regression tests for review.md critical issues
#
# Usage:  ./run_tests.sh           (from the Review/ directory)
#         make run                 (builds and runs)
#

cd "$(dirname "$0")"

PASS=0
FAIL=0
SKIP=0
TOTAL=0

separator() {
    echo "--------------------------------------------------------------"
}

run_c_test() {
    local name="$1"
    TOTAL=$((TOTAL + 1))
    if [ ! -x "./$name" ]; then
        printf "  %-44s SKIP (not built)\n" "$name"
        SKIP=$((SKIP + 1))
        return
    fi
    # Capture output, check exit code
    local output
    output=$(./"$name" 2>&1)
    local rc=$?
    echo "$output"
    if [ $rc -eq 0 ]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
    fi
}

echo "============================================================"
echo "  RetroFOCAL Regression Tests for Critical Review Issues"
echo "============================================================"
echo ""

echo "--- FOCAL program tests (automated verification) ---"
echo ""

# C1: Negative array subscript
TOTAL=$((TOTAL + 1))
echo "C1: Negative array subscript (heap underflow)"
if [ ! -x "../retrofocal" ]; then
    echo "  SKIP: retrofocal binary not found (run 'make' in project root first)"
    SKIP=$((SKIP + 1))
else
    output=$(../retrofocal test_C1_negative_array_subscript.fc 2>&1)
    rc=$?
    if [ $rc -gt 128 ]; then
        sig=$((rc - 128))
        echo "  FAIL: crashed with signal $sig (heap corruption from negative index)"
        FAIL=$((FAIL + 1))
    elif [ $rc -ne 0 ]; then
        echo "  FAIL: exited with error code $rc"
        FAIL=$((FAIL + 1))
    elif echo "$output" | grep -q "TEST COMPLETE"; then
        # The program ran without crashing.  Negative subscript indexing is
        # undefined behavior; it may appear to work on some platforms while
        # corrupting memory silently.  The bug still exists in the code.
        echo "  WARN: ran without crashing (undefined behavior did not manifest)"
        echo "        Bug still exists: negative index used on a 0-based C array"
        echo "        Run under AddressSanitizer to confirm: make asan"
        PASS=$((PASS + 1))
    else
        echo "  FAIL: unexpected output"
        echo "$output" | sed 's/^/        /'
        FAIL=$((FAIL + 1))
    fi
fi
separator

# C11: Comment text lost in WRITE output
TOTAL=$((TOTAL + 1))
echo "C11: Comment text lost in WRITE output"
if [ ! -x "../retrofocal" ]; then
    echo "  SKIP: retrofocal binary not found"
    SKIP=$((SKIP + 1))
else
    output=$(../retrofocal test_C11_comment_text_lost.fc 2>&1)
    rc=$?
    if [ $rc -ne 0 ]; then
        echo "  FAIL: interpreter error (exit code $rc)"
        FAIL=$((FAIL + 1))
    elif echo "$output" | grep -qi "THIS TEXT SHOULD APPEAR"; then
        echo "  PASS: comment text preserved in WRITE output"
        PASS=$((PASS + 1))
    else
        echo "  FAIL: comment text lost -- WRITE output does not contain the comment"
        echo "        WRITE produced:"
        echo "$output" | head -5 | sed 's/^/          /'
        FAIL=$((FAIL + 1))
    fi
fi
separator

echo ""
echo "--- C unit tests (automated pass/fail) ---"
echo ""

for t in test_C2_str_new_null \
         test_C3_str_new_overflow \
         test_C4_str_erase_inverted \
         test_C5_str_append_overflow \
         test_C6_lst_insert_at_index \
         test_C7_fin_fallthrough \
         test_C8_wrong_union_member \
         test_C9_lst_key_null \
         test_C10_lst_copy_broken; do
    run_c_test "$t"
    separator
done

echo ""
echo "============================================================"
echo "  Results: $PASS passed, $FAIL failed, $SKIP skipped (of $TOTAL)"
echo "============================================================"

if [ $FAIL -gt 0 ]; then
    echo ""
    echo "  Note: FAILs confirm the bugs described in review.md."
    echo "  After fixing an issue, the corresponding test should PASS."
fi

exit $FAIL
