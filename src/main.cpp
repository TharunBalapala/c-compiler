#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

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

    // 1. Lexing
    std::vector<Token> tokens;
    try {
        Lexer lexer(source);
        tokens = lexer.tokenize();
        std::cout << "Lexing successful (" << tokens.size() << " tokens)." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during lexing: " << e.what() << std::endl;
        return 1;
    }

    // 2. Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = nullptr;
    try {
        ast = parser.parse();
        std::cout << "Parsing successful! AST root contains " 
                  << ast->functions.size() << " function(s)." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during parsing: " << e.what() << std::endl;
        return 1;
    }

    // 3. Semantic Analysis
    SemanticAnalyzer semantic;
    try {
        semantic.analyze(ast.get());
        std::cout << "Semantic analysis successful!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during semantic analysis: " << e.what() << std::endl;
        return 1;
    }

    // 4. Code Generation
    std::string out_file = input_file.substr(0, input_file.find_last_of('.')) + ".s";
    CodeGenerator codegen(out_file);
    try {
        codegen.generate(ast.get());
        std::cout << "Code generation successful! Wrote assembly to: " << out_file << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during code generation: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Compilation completed successfully!" << std::endl;
    return 0;
}
