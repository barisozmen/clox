#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "memory.h"

static void test_grow_capacity_starts_at_8(void **state) {
    (void) state;
    assert_int_equal(GROW_CAPACITY(0), 8);
}

static void test_grow_capacity_doubles_after_8(void **state) {
    (void) state;
    assert_int_equal(GROW_CAPACITY(8), 16);
    assert_int_equal(GROW_CAPACITY(16), 32);
    assert_int_equal(GROW_CAPACITY(64), 128);
    assert_int_equal(GROW_CAPACITY(256), 512);
}

static void test_reallocate_allocates_new_memory(void **state) {
    (void) state;
    void *ptr = reallocate(NULL, 0, 100);
    assert_non_null(ptr);
    reallocate(ptr, 100, 0);
}

static void test_reallocate_expands_memory(void **state) {
    (void) state;
    int *array = reallocate(NULL, 0, sizeof(int) * 10);
    assert_non_null(array);

    array[0] = 42;
    array[9] = 99;

    int *expanded = reallocate(array, sizeof(int) * 10, sizeof(int) * 20);
    assert_non_null(expanded);
    assert_int_equal(expanded[0], 42);
    assert_int_equal(expanded[9], 99);

    reallocate(expanded, sizeof(int) * 20, 0);
}

static void test_reallocate_frees_memory(void **state) {
    (void) state;
    void *ptr = reallocate(NULL, 0, 100);
    assert_non_null(ptr);

    void *result = reallocate(ptr, 100, 0);
    assert_null(result);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_grow_capacity_starts_at_8),
        cmocka_unit_test(test_grow_capacity_doubles_after_8),
        cmocka_unit_test(test_reallocate_allocates_new_memory),
        cmocka_unit_test(test_reallocate_expands_memory),
        cmocka_unit_test(test_reallocate_frees_memory),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
