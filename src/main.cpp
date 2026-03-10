#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"

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
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "Lexing successful (" << tokens.size() << " tokens)." << std::endl;

    // 2. Parsing
    Parser parser(tokens);
    try {
        std::unique_ptr<ProgramNode> ast = parser.parse();
        std::cout << "Parsing successful! AST root contains " 
                  << ast->functions.size() << " function(s)." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed during parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
