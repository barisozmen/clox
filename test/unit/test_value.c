#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include "value.h"

static int setup_value_array(void **state) {
    ValueArray *array = malloc(sizeof(ValueArray));
    initValueArray(array);
    *state = array;
    return 0;
}

static int teardown_value_array(void **state) {
    ValueArray *array = *state;
    freeValueArray(array);
    free(array);
    return 0;
}

static void test_init_zeros_fields(void **state) {
    ValueArray *array = *state;
    assert_int_equal(array->count, 0);
    assert_int_equal(array->capacity, 0);
    assert_null(array->values);
}

static void test_write_appends_value(void **state) {
    ValueArray *array = *state;
    writeValueArray(array, 1.2);

    assert_int_equal(array->count, 1);
    assert_float_equal(array->values[0], 1.2, 0.001);
}

static void test_write_multiple_values(void **state) {
    ValueArray *array = *state;
    writeValueArray(array, 1.2);
    writeValueArray(array, 3.4);
    writeValueArray(array, 5.6);

    assert_int_equal(array->count, 3);
    assert_float_equal(array->values[0], 1.2, 0.001);
    assert_float_equal(array->values[1], 3.4, 0.001);
    assert_float_equal(array->values[2], 5.6, 0.001);
}

static void test_write_grows_array(void **state) {
    ValueArray *array = *state;

    for (int i = 0; i < 20; i++) {
        writeValueArray(array, (double)i);
    }

    assert_int_equal(array->count, 20);
    assert_true(array->capacity >= 20);

    for (int i = 0; i < 20; i++) {
        assert_float_equal(array->values[i], (double)i, 0.001);
    }
}

static void test_write_preserves_values_on_growth(void **state) {
    ValueArray *array = *state;

    for (int i = 0; i < 100; i++) {
        writeValueArray(array, (double)i * 1.5);
    }

    assert_int_equal(array->count, 100);

    for (int i = 0; i < 100; i++) {
        assert_float_equal(array->values[i], (double)i * 1.5, 0.001);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_init_zeros_fields,
                                         setup_value_array, teardown_value_array),
        cmocka_unit_test_setup_teardown(test_write_appends_value,
                                         setup_value_array, teardown_value_array),
        cmocka_unit_test_setup_teardown(test_write_multiple_values,
                                         setup_value_array, teardown_value_array),
        cmocka_unit_test_setup_teardown(test_write_grows_array,
                                         setup_value_array, teardown_value_array),
        cmocka_unit_test_setup_teardown(test_write_preserves_values_on_growth,
                                         setup_value_array, teardown_value_array),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
