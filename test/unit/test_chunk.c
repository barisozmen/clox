#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include "chunk.h"

static int setup_chunk(void **state) {
    Chunk *chunk = malloc(sizeof(Chunk));
    initChunk(chunk);
    *state = chunk;
    return 0;
}

static int teardown_chunk(void **state) {
    Chunk *chunk = *state;
    freeChunk(chunk);
    free(chunk);
    return 0;
}

static void test_init_zeros_fields(void **state) {
    Chunk *chunk = *state;
    assert_int_equal(chunk->count, 0);
    assert_int_equal(chunk->capacity, 0);
    assert_null(chunk->code);
    assert_int_equal(chunk->constants.count, 0);
}

static void test_write_appends_byte(void **state) {
    Chunk *chunk = *state;
    writeChunk(chunk, OP_RETURN, 123);

    assert_int_equal(chunk->count, 1);
    assert_int_equal(chunk->code[0], OP_RETURN);
    assert_int_equal(chunk->lines[0], 123);
}

static void test_write_multiple_bytes(void **state) {
    Chunk *chunk = *state;
    writeChunk(chunk, OP_RETURN, 1);
    writeChunk(chunk, OP_CONSTANT, 2);
    writeChunk(chunk, OP_ADD, 3);

    assert_int_equal(chunk->count, 3);
    assert_int_equal(chunk->code[0], OP_RETURN);
    assert_int_equal(chunk->code[1], OP_CONSTANT);
    assert_int_equal(chunk->code[2], OP_ADD);
    assert_int_equal(chunk->lines[0], 1);
    assert_int_equal(chunk->lines[1], 2);
    assert_int_equal(chunk->lines[2], 3);
}

static void test_write_grows_array(void **state) {
    Chunk *chunk = *state;

    for (int i = 0; i < 20; i++) {
        writeChunk(chunk, OP_RETURN, i);
    }

    assert_int_equal(chunk->count, 20);
    assert_true(chunk->capacity >= 20);

    for (int i = 0; i < 20; i++) {
        assert_int_equal(chunk->code[i], OP_RETURN);
        assert_int_equal(chunk->lines[i], i);
    }
}

static void test_add_constant(void **state) {
    Chunk *chunk = *state;
    int index = addConstant(chunk, 1.2);

    assert_int_equal(index, 0);
    assert_int_equal(chunk->constants.count, 1);
    assert_float_equal(chunk->constants.values[0], 1.2, 0.001);
}

static void test_add_multiple_constants(void **state) {
    Chunk *chunk = *state;
    int index1 = addConstant(chunk, 1.2);
    int index2 = addConstant(chunk, 3.4);
    int index3 = addConstant(chunk, 5.6);

    assert_int_equal(index1, 0);
    assert_int_equal(index2, 1);
    assert_int_equal(index3, 2);
    assert_int_equal(chunk->constants.count, 3);
    assert_float_equal(chunk->constants.values[0], 1.2, 0.001);
    assert_float_equal(chunk->constants.values[1], 3.4, 0.001);
    assert_float_equal(chunk->constants.values[2], 5.6, 0.001);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_init_zeros_fields,
                                         setup_chunk, teardown_chunk),
        cmocka_unit_test_setup_teardown(test_write_appends_byte,
                                         setup_chunk, teardown_chunk),
        cmocka_unit_test_setup_teardown(test_write_multiple_bytes,
                                         setup_chunk, teardown_chunk),
        cmocka_unit_test_setup_teardown(test_write_grows_array,
                                         setup_chunk, teardown_chunk),
        cmocka_unit_test_setup_teardown(test_add_constant,
                                         setup_chunk, teardown_chunk),
        cmocka_unit_test_setup_teardown(test_add_multiple_constants,
                                         setup_chunk, teardown_chunk),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
