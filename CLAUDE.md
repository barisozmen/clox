# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **clox**, a bytecode virtual machine implementation of the Lox programming language from [Crafting Interpreters](https://craftinginterpreters.com). It's a C-based interpreter that compiles Lox source code to bytecode and executes it on a stack-based VM.

## Build & Run

Build using Make:

```bash
# Build the interpreter
make

# Run the interpreter
./clox

# Run REPL mode
./clox

# Run a Lox file
./clox script.lox
```

## Architecture

### Core Components

**Chunk System** (chunk.h, chunk.c)
- `Chunk` is the central bytecode container, holding:
  - `code`: dynamic array of bytecode instructions (uint8_t)
  - `lines`: parallel array tracking source line numbers for error reporting
  - `constants`: ValueArray storing constant values referenced by bytecode
- Uses dynamic array pattern with `count` (used) and `capacity` (allocated) fields
- Grows by 2x when capacity is exceeded (via GROW_CAPACITY macro)

**Value System** (value.h, value.c)
- `Value` is currently typedef'd to `double` (will expand to tagged union for multiple types)
- `ValueArray` uses same dynamic array pattern as Chunk
- `printValue()` handles value display

**Memory Management** (memory.h, memory.c)
- All dynamic allocations go through `reallocate()` centralized allocator
- Macros: `GROW_ARRAY`, `FREE_ARRAY`, `GROW_CAPACITY`
- Growth strategy: starts at 8, then doubles
- Future: will integrate with garbage collector

**OpCode System** (chunk.h)
- `OpCode` enum defines bytecode instructions:
  - `OP_CONSTANT`: load constant from constants array
  - `OP_ADD`, `OP_SUBTRACT`, `OP_MULTIPLY`, `OP_DIVIDE`: binary arithmetic operations
  - `OP_NEGATE`: unary negation operation
  - `OP_RETURN`: return from current function
- Instructions are 1-byte opcodes, some followed by operand bytes

**Disassembler** (debug.h, debug.c)
- `disassembleChunk()`: pretty-prints entire chunk
- `disassembleInstruction()`: decodes single instruction
- Instruction-specific helpers: `constantInstruction()`, `simpleInstruction()`
- Line number display optimizes repeated lines with "|" symbol

### Data Flow

1. Bytecode is written to Chunk via `writeChunk(chunk, byte, line)`
2. Constants are added via `addConstant(chunk, value)` which returns index
3. OP_CONSTANT instruction stores the constant pool index as operand
4. Disassembler reads bytecode and displays decoded instructions

### Key Patterns

**Dynamic Arrays**: All growable structures use the count/capacity pattern with centralized reallocation through memory.h macros.

**Chunk Lifecycle**:
```c
Chunk chunk;
initChunk(&chunk);      // Initialize to empty state
writeChunk(&chunk, ...); // Add bytecode
freeChunk(&chunk);      // Deallocate and reset
```

**Constant Pool Indexing**: Constants stored in chunk.constants array, referenced by index in bytecode. Currently limited to 256 constants (uint8_t index).

**Virtual Machine** (vm.h, vm.c)
- Stack-based VM with 256-slot value stack
- `VM` struct contains:
  - `chunk`: pointer to bytecode chunk being executed
  - `ip`: instruction pointer (points to next byte to execute)
  - `stack`: fixed-size array of Values
  - `stackTop`: points to next empty stack slot
- `run()`: main execution loop that fetches, decodes, and executes bytecode
- Stack operations: `push()` and `pop()` for value manipulation
- Arithmetic operations implemented: ADD, SUBTRACT, MULTIPLY, DIVIDE, NEGATE
- Uses macros for bytecode reading: `READ_BYTE()`, `READ_CONSTANT()`, `BINARY_OP()`

**Scanner** (scanner.h, scanner.c)
- Lexical analyzer that tokenizes Lox source code
- `Scanner` struct tracks:
  - `start`: beginning of current token
  - `current`: current character being examined
  - `line`: current line number
- `Token` struct contains type, lexeme location (start/length), and line number
- Supports all Lox token types: operators, keywords, literals, identifiers
- Features: lookahead (`peek()`, `peekNext()`), keyword recognition, string/number literals
- Handles single-char tokens, two-char operators, comments, whitespace

**Compiler** (compiler.h, compiler.c)
- Bridges scanner and VM by compiling source code to bytecode
- `compile()`: entry point for compilation pipeline
- Currently in development (scanner integration phase)

**Main Program** (main.c)
- Two execution modes:
  - REPL: Interactive line-by-line interpreter (`./clox`)
  - File execution: Run Lox scripts (`./clox script.lox`)
- `repl()`: reads and interprets lines in loop
- `runFile()`: reads entire file and interprets
- Proper exit codes: 64 (usage), 65 (compile error), 70 (runtime error), 74 (I/O error)

## Testing

The project uses a comprehensive testing infrastructure with both unit tests and integration tests.

### Running Tests

```bash
# Run all tests (unit + integration)
make test

# Run only unit tests
make test-unit

# Run only integration tests
make test-integration

# Clean build artifacts
make clean
```

### Unit Tests

Unit tests are written in C using the [CMocka](https://cmocka.org/) testing framework. Tests are located in `test/unit/`.

**Test Coverage:**
- `test_memory.c` - Memory allocation and dynamic array growth
- `test_chunk.c` - Bytecode chunk operations and constant pool
- `test_value.c` - Value array operations
- `test_scanner.c` - Lexical analysis and tokenization
- `test_vm.c` - VM stack operations and instruction execution

**Writing Unit Tests:**

```c
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include "chunk.h"

static void test_example(void **state) {
    (void) state;
    Chunk chunk;
    initChunk(&chunk);

    assert_int_equal(chunk.count, 0);
    freeChunk(&chunk);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_example),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### Integration Tests

Integration tests are end-to-end tests written as `.lox` files with expected output annotations. Tests are located in `test/integration/`.

**Test File Format:**

```lox
// test/integration/arithmetic/add.lox
print 1 + 2; // expect: 3
print 5 + 5; // expect: 10
```

The Python test runner (`test/run_tests.py`) parses these annotations:
- `// expect: <output>` - Expected output line
- `// expect runtime error: <message>` - Expected runtime error
- `// expect compile error` - Expected compilation error

**Writing Integration Tests:**

1. Create a `.lox` file in `test/integration/`
2. Add Lox code with `// expect:` comments
3. Run `make test-integration`

**Current Test Suites:**
- `arithmetic/` - Addition, subtraction, multiplication, division, negation, precedence

### Test-Driven Development

When adding new features:

1. **Write integration tests first** - Define expected behavior in `.lox` files
2. **Write unit tests** - Test individual components (scanner, compiler, VM)
3. **Implement the feature** - Make tests pass
4. **Run all tests** - Ensure no regressions with `make test`

### Testing Guidelines

- Every new VM opcode needs unit tests in `test_vm.c`
- Every language feature needs integration tests in `test/integration/`
- Scanner changes require tokenization tests in `test_scanner.c`
- Run tests frequently during development to catch issues early

## Development Notes

- The VM is progressing through Chapters 14-16 of Crafting Interpreters
- Scanner is complete and functional
- Compiler is in development
- VM execution loop supports arithmetic operations
- REPL and file loading are functional
- Line tracking allows error messages to reference source locations
- Comprehensive test suite covers memory, chunks, values, scanner, and VM
