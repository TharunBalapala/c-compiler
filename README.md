# Mini C Compiler (minic)

`minic` is a professional, portable compiler for a minimal subset of the C programming language. It performs all standard compiler phases, from lexical analysis to x86-32 assembly code generation, providing a hands-on tool for learning and compiling custom C-like programs.

## Features

- **Lexical Analysis (Scanner)**: Tokenizes input strings accurately and handles keywords, identifiers, constants, and string literals.
- **Syntax Analysis (Parser)**: Builds an Abstract Syntax Tree (AST) supporting loops, conditionals, functions, and mathematical expressions using Recursive Descent.
- **Semantic Analysis**: Enforces scope rules, manages the symbol table, stack offsets for variables, and verifies semantic correctness.
- **Code Generation**: Translates the AST directly into unoptimized x86-32 Intel-syntax assembly.
- **Native Assembly & Linking**: Automatically calls the GCC backend to assemble and link generated code into a runnable executable.

## Project Structure

```text
/minic
├── src/
│   └── main.cpp           # Compiler entry point and CLI logic
├── include/
│   ├── lexer.h            # Lexer and Token definitions
│   ├── parser.h           # Recursive Descent Parser
│   ├── ast.h              # Abstract Syntax Tree nodes
│   ├── semantic.h         # Scope and Symbol Table logic
│   ├── codegen.h          # x86-32 Code Generator
│   └── ast_printer.h      # AST visualizer class
├── lib/                   # Implementation files for core compiler modules
├── tests/                 # Sample `.mc` source files to compile
├── CMakeLists.txt         # Cross-platform build configuration
└── README.md              # Project documentation
```

## Installation

### Prerequisites
- **CMake** (3.10 or higher)
- **C++17** compatible compiler (GCC, Clang, MSVC)
- **GCC (MinGW on Windows)**: The final assembly and linking phase strictly relies on the `gcc` command being available in your system's PATH.

### Build Steps (All Platforms)

1. **Clone the repository and enter the directory**:
   ```bash
   cd minic
   ```

2. **Generate the build files with CMake**:
   ```bash
   cmake -B build -S .
   ```

3. **Compile the `minic` executable**:
   ```bash
   cmake --build build
   ```

### Installation Steps

You can now run `minic` from anywhere!

#### Debian Package (.deb) - Best for sharing!
If you want to share `minic` with a friend or install it properly on a Debian/Ubuntu system, you can generate a `.deb` package:

1. **Build the package**:
   ```bash
   cd build
   cpack -G DEB
   ```
2. **Install the package**:
   ```bash
   sudo apt install ./minic-1.0.0-Linux.deb
   ```
This will automatically handle all dependencies like `gcc-multilib`.

#### On Windows
1. Locate the built `minic.exe` in the `build/Debug/` or `build/` directory.
2. Move it to a dedicated tools folder (e.g., `C:\tools\minic\`).
3. Open your **Environment Variables** settings and add that tools folder to your `PATH` variable.
4. Open a new Command Prompt or PowerShell, and type `minic --version` to verify.

## Usage

Compile a `.mc` file directly into an executable:
```bash
minic tests/hello.mc
```

Place the output executable into a custom named file:
```bash
minic tests/hello.mc -o my_program.exe
```

Generate **assembly only** without linking:
```bash
minic -S tests/hello.mc
```
*(Produces `hello.s` in the current directory).*

View **verbose diagnostic output** (useful for debugging the tokenizer or AST):
```bash
minic --verbose tests/hello.mc
```

Print version or help info:
```bash
minic --version
minic --help
```

## Example Input / Output

**Input (`tests/hello.mc`):**
```c
int main() {
    printf("Hello from minic!\n");
    return 0;
}
```

**Compilation:**
```bash
> minic -v tests/hello.mc
Compiling: tests/hello.mc

============================================
        COMPILER PIPELINE INITIATED         
============================================

--- Phase 1: Lexical Analysis ---
Lexing successful (13 tokens).
Token Stream:
...

--- Phase 2: Syntax Analysis (Parser) ---
Parsing successful! AST root contains 1 function(s).
...

--- Phase 3: Semantic Analysis ---
Semantic analysis successful! Symbols resolved and scopes verified.

--- Phase 4: Code Generation (x86-32 Assembly) ---
Code generation successful! Wrote generic assembly to: tests/hello.s

--- Phase 5: Native Assembly & Linking ---
Invoking GCC to assemble tests/hello.s into tests/hello.exe...

============================================
Compilation completed successfully!
Executable built: tests/hello.exe
Run it using: .\tests\hello.exe
============================================
```

**Execution:**
```bash
> .\tests\hello.exe
Hello from minic!
```

## Contributing
Contributions are welcome. Please ensure your code conforms to C++17 standards and uses CMake for builds.
