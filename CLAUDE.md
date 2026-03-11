# Miniature C Compiler — Project Guide

## Overview
A hand-written C compiler in C++17 targeting 32-bit x86 Intel assembly on Windows.
Pipeline: **Lexer → Parser (AST) → Semantic Analysis → Code Generation**.

## Project Structure
```
src/
  main.cpp          — Entry point, orchestrates the pipeline
  lexer.h / .cpp    — Tokenizer (character-by-character scanning)
  ast.h             — AST node definitions (expressions, statements, types)
  parser.h / .cpp   — Recursive-descent parser producing AST
  semantic.h / .cpp — Scope/symbol table management, variable resolution
  codegen.h / .cpp  — AST → x86-32 Intel assembly (.s files)
tests/
  test.c            — Basic arithmetic / control flow test
  test_io.c         — printf / scanf interop test
build/              — Build output (gitignored)
```

## Build & Test Commands

### Build the compiler (g++ / MinGW)
```powershell
.\build.ps1
# or manually:
g++ -std=c++17 -Wall -Wextra src/*.cpp -o build/ccompiler.exe
```

### Run the full pipeline on a test file
```powershell
.\run_test.ps1 tests\test.c
# Compiles → assembles with gcc → runs the executable
```

## Prerequisites
- **g++ (MinGW)** on PATH — used both to build the compiler and to assemble output.
- Windows PowerShell for the helper scripts.

## Coding Conventions
- **Language:** C++17, no external parser generators (Flex/Bison).
- **Style:** PascalCase for types, camelCase for variables/functions.
- **Headers:** Each module has a `.h` / `.cpp` pair; AST nodes live in `ast.h`.
- **Warnings:** Compile with `-Wall -Wextra -pedantic`.

## Architecture Notes
- The **lexer** produces a flat `vector<Token>` consumed by the parser.
- The **parser** is pure recursive descent; operator precedence is encoded in the grammar functions.
- **Semantic analysis** walks the AST to build scoped symbol tables and checks variable declarations.
- **Code generation** emits Intel-syntax x86-32 assembly, targeting the `cdecl` calling convention for stdlib interop (`printf`, `scanf`).
- String literals are placed in the `.data` segment; locals live on the stack via `ebp` offsets.

## Supported C Subset
`int`, arithmetic (`+-*/%`), comparisons, logical operators, `if/else`, `while`, `return`, local variables, pointers (`&` address-of), string literals, `printf`, `scanf`.
