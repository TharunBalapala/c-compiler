#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "ast_printer.h"

const std::string MINIC_VERSION = "1.0.0";

void print_help(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] file...\n"
              << "Options:\n"
              << "  --help          Display this information.\n"
              << "  --version       Display compiler version information.\n"
              << "  -o <file>       Place the output into <file>.\n"
              << "  -S              Compile only; do not assemble or link.\n"
              << "  -q, --quiet     Suppress phase output (only errors/warnings).\n"
              << "  -v, --verbose   Enable verbose output (tokens, AST, etc.) [default].\n";
}

void print_version() {
    std::cout << "minic version " << MINIC_VERSION << "\n";
}

std::string getTokenName(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::StringLiteral: return "String";
        case TokenType::Int: return "Keyword:int";
        case TokenType::Return: return "Keyword:return";
        case TokenType::If: return "Keyword:if";
        case TokenType::Else: return "Keyword:else";
        case TokenType::While: return "Keyword:while";
        case TokenType::For: return "Keyword:for";
        case TokenType::EndOfFile: return "EOF";
        default: return "Symbol/Operator";
    }
}

std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "minic: \x1b[31merror:\x1b[0m Failed to open file: " << filepath << "\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "minic: \x1b[31mfatal error:\x1b[0m no input files\n";
        std::cerr << "compilation terminated.\n";
        return 1;
    }

    std::string input_file = "";
    std::string output_file = "";
    bool verbose = true;
    bool compile_only = false;

    // Argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            print_help(argv[0]);
            return 0;
        } else if (arg == "--version") {
            print_version();
            return 0;
        } else if (arg == "-q" || arg == "--quiet") {
            verbose = false;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-S") {
            compile_only = true;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "minic: \x1b[31merror:\x1b[0m missing filename after '-o'\n";
                return 1;
            }
        } else {
            if (!arg.empty() && arg[0] == '-') {
                std::cerr << "minic: \x1b[31merror:\x1b[0m unrecognized command line option '" << arg << "'\n";
                return 1;
            }
            if (!input_file.empty()) {
                std::cerr << "minic: \x1b[31merror:\x1b[0m multiple input files are not supported yet\n";
                return 1;
            }
            input_file = arg;
        }
    }

    if (input_file.empty()) {
        std::cerr << "minic: \x1b[31mfatal error:\x1b[0m no input files\n";
        std::cerr << "compilation terminated.\n";
        return 1;
    }

    // Check extension
    if (input_file.length() < 3 || input_file.substr(input_file.length() - 3) != ".mc") {
        std::cerr << "minic: \x1b[33mwarning:\x1b[0m file '" << input_file << "' does not have a '.mc' extension.\n";
    }

    // Determine output file names early
    std::string base_name = input_file;
    size_t last_dot = base_name.find_last_of('.');
    if (last_dot != std::string::npos) {
        base_name = base_name.substr(0, last_dot);
    }
    
    std::string phases_file = base_name + "_phases.txt";
    std::ofstream out_log_file(phases_file);
    std::streambuf* coutbuf = nullptr;

    if (verbose) {
        std::cout << "Compiling " << input_file << "...\n";
        std::cout << "Phase results will be saved to: " << phases_file << "\n";
        // Redirect cout to the phases file
        coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out_log_file.rdbuf());

        std::cout << "Compiling: " << input_file << "\n";
        std::cout << "\n============================================\n";
        std::cout << "        COMPILER PIPELINE INITIATED         \n";
        std::cout << "============================================\n\n";
    }

    std::string source = read_file(input_file);

    // 1. Lexing
    if (verbose) std::cout << "--- Phase 1: Lexical Analysis ---\n";
    std::vector<Token> tokens;
    try {
        Lexer lexer(source);
        tokens = lexer.tokenize();
        if (verbose) {
            std::cout << "Lexing successful (" << tokens.size() << " tokens).\n";
            std::cout << "Token Stream:\n";
            for (const auto& t : tokens) {
                std::cout << "  [" << getTokenName(t.type) << " | '" << t.text << "']\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "minic: " << e.what() << std::endl;
        return 1;
    }

    // 2. Parsing
    if (verbose) std::cout << "\n--- Phase 2: Syntax Analysis (Parser) ---\n";
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = nullptr;
    try {
        ast = parser.parse();
        if (verbose) {
            std::cout << "Parsing successful! AST root contains " 
                      << ast->functions.size() << " function(s).\n";
            std::cout << "Syntax Tree Dump:\n";
            ASTPrinter printer;
            printer.print(ast.get());
        }
    } catch (const std::exception& e) {
        std::cerr << "minic: parsing failed.\n";
        return 1;
    }

    // 3. Semantic Analysis
    if (verbose) std::cout << "\n--- Phase 3: Semantic Analysis ---\n";
    SemanticAnalyzer semantic;
    try {
        semantic.analyze(ast.get());
        if (verbose) std::cout << "Semantic analysis successful! Symbols resolved and scopes verified.\n";
    } catch (const std::exception& e) {
        std::cerr << "minic: semantic analysis failed.\n";
        return 1;
    }

    std::string asm_file;
    std::string exe_file;

    if (compile_only) {
        asm_file = output_file.empty() ? base_name + ".s" : output_file;
    } else {
        asm_file = base_name + ".s";
#ifdef _WIN32
        exe_file = output_file.empty() ? base_name + ".exe" : output_file;
#else
        exe_file = output_file.empty() ? base_name : output_file;
#endif
    }

    // 4. Code Generation
    if (verbose) std::cout << "\n--- Phase 4: Code Generation (x86-32 Assembly) ---\n";
    {
        CodeGenerator codegen(asm_file);
        try {
            codegen.generate(ast.get());
            if (verbose) std::cout << "Code generation successful! Wrote generic assembly to: " << asm_file << "\n";
        } catch (const std::exception& e) {
            std::cerr << "minic: \x1b[31merror:\x1b[0m Code generation failed: " << e.what() << std::endl;
            return 1;
        }
    }

    // 5. Native Assembly & Linking (via GCC)
    if (!compile_only) {
        if (verbose) {
            std::cout << "\n--- Phase 5: Native Assembly & Linking ---\n";
            std::cout << "Invoking GCC to assemble " << asm_file << " into " << exe_file << "...\n";
        }
        std::string gcc_cmd = "gcc -m32 -masm=intel -o " + exe_file + " " + asm_file;
        int gcc_status = system(gcc_cmd.c_str());
        
        if (gcc_status != 0) {
            std::cerr << "minic: \x1b[31merror:\x1b[0m linker command failed with exit code " << gcc_status << "\n";
            return 1;
        }

        if (verbose) {
            std::cout << "\n============================================\n";
            std::cout << "Compilation completed successfully!\n";
            std::cout << "Executable built: " << exe_file << "\n";
            std::cout << "Run it using: .\\" << exe_file << "\n";
            std::cout << "============================================\n\n";
        }
    } else if (verbose) {
        std::cout << "\nCompilation to assembly completed successfully.\n";
        std::cout << "Output: " << asm_file << "\n";
    }

    // Restore standard output
    if (verbose && coutbuf) {
        std::cout.rdbuf(coutbuf);
        std::cout << "Compilation completed (phases saved to " << phases_file << ").\n";
    }

    return 0;
}
