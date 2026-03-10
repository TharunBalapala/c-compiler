# Miniature C Compiler in C++

![Language](https://img.shields.io/badge/Language-C++17-blue.svg)
![Target](https://img.shields.io/badge/Target-x86_32-red.svg)
![Status](https://img.shields.io/badge/Status-Functional-success.svg)

A hand-written, miniature C compiler implemented from scratch in modern C++. This compiler pipelines a subset of the C programming language completely through Lexical Analysis, syntax tree parsing, Semantic Analysis, and Code Generation directly into executable `x86` (32-bit Intel syntax) assembly.

The project demonstrates the core architectural components of a compiler without relying on external frontend tools like Flex or Bison, and links with `gcc` (MinGW) on Windows to create standalone native executables.

## 🚀 Features Supported
- **Data Types:** `int` and integer math operations (`+`, `-`, `*`, `/`, `%`)
- **Control Flow:** `if`, `else`, `while`, and `return` statements
- **Logic & Relational Operators:** (`==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`)
- **Variables:** Local variable declarations, initialization, and assignment.
- **Pointers / Addresses:** Specifically the address-of unary operator (`&`), evaluated securely through `lea` stack operations.
- **Strings:** Double-quoted `"string literals"`, safely allocated in the executable's `.data` segment.
- **Standard Library Interoperability:** Implements the `stdcall`/`cdecl` calling convention on the stack, allowing seamless external calls to standard functions like `printf` and `scanf`.

## 🧠 Architecture Pipeline

1. **Lexical Analysis (`lexer`):** Reads the raw C source code character-by-character and groups them into meaningful `Token` objects (e.g., Identifying the difference between the `=` assignment and `==` equality operators).
2. **Syntax Analysis (`parser`):** Uses a Recursive Descent Parsing algorithm to construct an Abstract Syntax Tree (AST). It enforces the grammatical rules of C, ensuring expressions and statements are nested correctly according to the order of operations.
3. **Semantic Analysis (`semantic`):** A tree-walker that manages variable scope. It maps variable declarations and guarantees that variables referenced in expressions have actually been declared in the active block scope. 
4. **Code Generation (`codegen`):** A second tree-walker that translates the validated AST into native Windows 32-bit Intel Assembly (`.intel_syntax`). It dynamically tracks stack offsets (`ebp - X`), pushes variables through CPU registers (`eax`, `ecx`), and generates uniquely labeled jumps for control flow loops.

## 💻 Building and Testing

### Prerequisites
- `g++` (MinGW) compiler installed and accessible in your system PATH.

### Quick Start (Windows PowerShell)

We provide an automated testing script (`run_test.ps1`) that sequentially compiles the C++ compiler project, compiles a target `test.c` script, assembles the output, and executes it.

1. Clone the repository:
   ```powershell
   git clone https://github.com/TharunBalapala/c-compiler.git
   cd c-compiler
   ```

2. Run the automated build/test pipeline:
   ```powershell
   .\run_test.ps1 tests\test.c
   ```

3. **Or, to interact with the standard library test (`printf` and `scanf`):**
   ```powershell
   .\run_test.ps1 tests\test_io.c
   ```

This script will automatically output the compiled assembly `.s` syntax alongside an executable `.exe` file native to your computer, and print the Exit Code returned by your custom C script.

## 📝 Example Source (`test_io.c`)

```c
int main() {
    int x;
    printf("Enter a number: ");
    scanf("%d", &x);
    
    int result = x * 2;
    printf("Double your number is: %d\n", result);
    
    return 0;
}
```

*Compiled output directly handles Windows Stdlib CDECL bridging for you!*

## 📜 License
This project is open-source and available for educational purposes to understand the magic that translates high-level text into machine electricity.
