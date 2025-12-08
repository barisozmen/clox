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

## Development Notes

- The VM is in early stages (Chapter 14 of Crafting Interpreters)
- No scanner, parser, or VM execution loop yet
- main.c currently contains a hardcoded test chunk
- Line tracking allows future error messages to reference source locations
- Static helper functions in debug.c need forward declarations or reordering
