#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "ast_printer.h"

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
        std::cerr << "Failed to open file: " << filepath << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.c>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::cout << "Compiling: " << input_file << std::endl;

    std::string source = read_file(input_file);

    std::cout << "\n============================================\n";
    std::cout << "        COMPILER PIPELINE INITIATED         \n";
    std::cout << "============================================\n\n";

    // 1. Lexing
    std::cout << "--- Phase 1: Lexical Analysis ---\n";
    std::vector<Token> tokens;
    try {
        Lexer lexer(source);
        tokens = lexer.tokenize();
        std::cout << "Lexing successful (" << tokens.size() << " tokens).\n";
        std::cout << "Token Stream:\n";
        for (const auto& t : tokens) {
            std::cout << "  [" << getTokenName(t.type) << " | '" << t.text << "']\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during lexing: " << e.what() << std::endl;
        return 1;
    }

    // 2. Parsing
    std::cout << "\n--- Phase 2: Syntax Analysis (Parser) ---\n";
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = nullptr;
    try {
        ast = parser.parse();
        std::cout << "Parsing successful! AST root contains " 
                  << ast->functions.size() << " function(s).\n";
        std::cout << "Syntax Tree Dump:\n";
        ASTPrinter printer;
        printer.print(ast.get());
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during parsing: " << e.what() << std::endl;
        return 1;
    }

    // 3. Semantic Analysis
    std::cout << "\n--- Phase 3: Semantic Analysis ---\n";
    SemanticAnalyzer semantic;
    try {
        semantic.analyze(ast.get());
        std::cout << "Semantic analysis successful! Symbols resolved and scopes verified.\n";
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during semantic analysis: " << e.what() << std::endl;
        return 1;
    }

    // 4. Code Generation
    std::cout << "\n--- Phase 4: Code Generation (x86-32 Assembly) ---\n";
    std::string base_name = input_file.substr(0, input_file.find_last_of('.'));
    std::string out_file = base_name + ".s";
    std::string exe_file = base_name + ".exe";
    
    {
        CodeGenerator codegen(out_file);
        try {
            codegen.generate(ast.get());
            std::cout << "Code generation successful! Wrote generic assembly to: " << out_file << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Compilation failed during code generation: " << e.what() << std::endl;
            return 1;
        }
    }

    // 5. Native Assembly (via GCC)
    std::cout << "\n--- Phase 5: Native Assembly ---\n";
    std::cout << "Invoking GCC to assemble " << out_file << " into " << exe_file << "...\n";
    std::string gcc_cmd = "gcc -m32 -masm=intel -o " + exe_file + " " + out_file;
    int gcc_status = system(gcc_cmd.c_str());
    
    std::cout << "\n============================================\n";
    if (gcc_status == 0) {
        std::cout << "Compilation completed successfully!\n";
        std::cout << "Executable built: " << exe_file << "\n";
        std::cout << "Run it using: .\\" << exe_file << "\n";
    } else {
        std::cerr << "Compilation failed during native assembly phase.\n";
        return 1;
    }
    
    std::cout << "============================================\n\n";
    return 0;
}
