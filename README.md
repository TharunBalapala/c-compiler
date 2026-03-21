# C Compiler

A hand-written, miniature C compiler implemented from scratch in modern C++17. This project is built specifically for academic demonstration, prioritizing clarity, educational value, and the transparency of compiler phases over complex build systems or production features.

It pipelines a subset of the C programming language completely through **Lexical Analysis**, **Syntax Analysis**, **Semantic Analysis**, and **Code Generation** directly into executable `x86` (32-bit Intel syntax) assembly.

---

## 🚀 Features & Academic Focus

- **No Parser Generators:** Built without external tools like Flex or Bison. The parser is a handwritten **Recursive Descent Parser**, demonstrating top-down grammatical analysis.
- **Verbose Compiler Phases:** The compiler visually outputs the results of its internal passes (Token streams, AST trees, Symbol Tables) so professors and students can clearly verify its internal state.
- **x86 Machine Translation & Automated Assembly:** Generates raw `.intel_syntax` assembly and automatically invokes `gcc` to natively assemble the `.s` into a runnable `.exe` on Windows. Models the **CDECL calling convention** to properly handle function parameters on the stack and interface with the C Standard Library (`printf`, `scanf`).
- **Core C Subset:** 
  - `int` variables and arithmetic (`+`, `-`, `*`, `/`, `%`)
  - Control Flow (`if/else`, `while`, `return`)
  - Relational Operators (`==`, `!=`, `<`, `>`, `<=`, `>=`)
  - User-defined functions with parameters
  - Pointers (`&` address-of)

---

## 🧠 The Compiler Pipeline

This project cleanly separates the compilation process into four distinct modules (see `src/` for source code):

1. **Phase 1: Lexical Analysis (`lexer.cpp`)**
   Reads raw source code characters and converts them into a linear stream of `Token` objects.
2. **Phase 2: Syntax Analysis (`parser.cpp` & `ast_printer.cpp`)**
   Consumes the token stream to build an **Abstract Syntax Tree (AST)** representing the hierarchical grammar of the code. The `ASTPrinter` visually dumps this tree to the console.
3. **Phase 3: Semantic Analysis (`semantic.cpp`)**
   A tree-walking pass that builds hierarchical **Symbol Tables** mapping variables to stack offsets, enforcing scoping rules and catching undeclared variables.
4. **Phase 4 & 5: Code Generation & Assembly (`codegen.cpp` -> `gcc`)**
   A final tree-walker that translates the validated AST into Windows 32-bit Intel Assembly (`.s`), managing `ebp/esp` stack frames and conditional jumps. Finally, it delegates to `gcc` via a highly-authentic system pipeline to instantly assemble a ready-to-run `.exe`.

---

## 💻 Building and Running

This project intentionally avoids complex build systems like CMake to ensure it is trivially easy to compile and evaluate on any machine with `g++`.

### Prerequisites
- `g++` (MinGW) installed and added to your system PATH.

### 1. Compile the Compiler
Run the following single command from the root of the repository to compile the compiler itself:
```powershell
g++ -std=c++17 -Wall -Wextra src/*.cpp -o compiler.exe
```

### 2. Run the Compiler (Test the C code)
We provide a comprehensive demonstration file `test.c` that includes a while-loop, standard IO, and user-defined functions. Pass it to the compiler:
```powershell
.\compiler.exe test.c
```
*(Watch the console! The compiler will output its Tokens, Abstract Syntax Tree, Symbol Table registrations, and will automatically invoke GCC to build the native executable).*

### 3. Execute
Run the final generated program:
```powershell
.\test.exe
```

---

## 📝 Example C Program (`test.c`)
The provided `test.c` demonstrates the compiler's full capabilities:

```c
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int main() {
    int i = 0;
    int sum = 0;
    int user_input;
    
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }
    printf("The loop computed sum: %d\n", sum);

    printf("Enter a number to double: ");
    scanf("%d", &user_input);

    int doubled = multiply(user_input, 2);
    int final_result = add(sum, doubled);
    
    printf("Your number doubled is: %d\n", doubled);
    printf("Sum + Doubled = %d\n", final_result);

    return 0;
}
```

---
*Created for Academic Demonstration of Compiler Design Principles.*
