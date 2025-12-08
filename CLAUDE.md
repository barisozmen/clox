# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **clox**, a bytecode virtual machine implementation of the Lox programming language from [Crafting Interpreters](https://craftinginterpreters.com). It's a C-based interpreter that compiles Lox source code to bytecode and executes it on a stack-based VM.

## Build & Run

Currently, there's no Makefile. Build manually:

```bash
# Compile all source files
gcc -o clox *.c

# Run the interpreter
./clox
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

## Development Notes

- The VM is progressing through Chapters 14-16 of Crafting Interpreters
- Scanner is complete and functional
- Compiler is in development
- VM execution loop supports arithmetic operations
- REPL and file loading are functional
- Line tracking allows error messages to reference source locations
