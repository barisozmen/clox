#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "vm.h"
#include "chunk.h"

static int setup_vm(void **state) {
    initVM();
    *state = NULL;
    return 0;
}

static int teardown_vm(void **state) {
    (void) state;
    freeVM();
    return 0;
}

static void test_push_and_pop(void **state) {
    (void) state;

    push(1.5);
    push(2.5);
    push(3.5);

    assert_float_equal(pop(), 3.5, 0.001);
    assert_float_equal(pop(), 2.5, 0.001);
    assert_float_equal(pop(), 1.5, 0.001);
}

static void test_stack_operations(void **state) {
    (void) state;

    push(10.0);
    Value val = pop();
    assert_float_equal(val, 10.0, 0.001);

    push(20.0);
    push(30.0);
    Value val2 = pop();
    Value val1 = pop();
    assert_float_equal(val2, 30.0, 0.001);
    assert_float_equal(val1, 20.0, 0.001);
}

static void test_vm_constant_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_add_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant1 = addConstant(&chunk, 1.2);
    int constant2 = addConstant(&chunk, 3.4);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);
    writeChunk(&chunk, OP_ADD, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_subtract_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant1 = addConstant(&chunk, 5.0);
    int constant2 = addConstant(&chunk, 3.0);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);
    writeChunk(&chunk, OP_SUBTRACT, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_multiply_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant1 = addConstant(&chunk, 2.0);
    int constant2 = addConstant(&chunk, 3.0);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);
    writeChunk(&chunk, OP_MULTIPLY, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_divide_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant1 = addConstant(&chunk, 6.0);
    int constant2 = addConstant(&chunk, 2.0);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);
    writeChunk(&chunk, OP_DIVIDE, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_negate_instruction(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 5.0);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    writeChunk(&chunk, OP_NEGATE, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

static void test_vm_complex_expression(void **state) {
    (void) state;

    Chunk chunk;
    initChunk(&chunk);

    int constant1 = addConstant(&chunk, 1.2);
    int constant2 = addConstant(&chunk, 3.4);
    int constant3 = addConstant(&chunk, 5.6);

    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant1, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant2, 123);
    writeChunk(&chunk, OP_ADD, 123);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant3, 123);
    writeChunk(&chunk, OP_MULTIPLY, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    InterpretResult result = interpret(&chunk);
    assert_int_equal(result, INTERPRET_OK);

    freeChunk(&chunk);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_push_and_pop,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_stack_operations,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_constant_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_add_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_subtract_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_multiply_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_divide_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_negate_instruction,
                                         setup_vm, teardown_vm),
        cmocka_unit_test_setup_teardown(test_vm_complex_expression,
                                         setup_vm, teardown_vm),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
