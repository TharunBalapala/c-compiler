#pragma once
#include <string>
#include <vector>

/**
 * ============================================================================
 * PHASE 1: LEXICAL ANALYSIS (The Lexer / Scanner)
 * ============================================================================
 * The Lexer is the first phase of the compiler. It reads the raw source code 
 * (a string of characters) and converts it into a sequence of meaningful 
 * groups called 'Tokens' (e.g., Keywords, Identifiers, Operators, Literals).
 * 
 * Concept: Finite State Machine / Linear Scan
 * Time Complexity: O(N) where N is the length of the source code.
 */

enum class TokenType {
    Identifier,
    Number, // Integer literal
    StringLiteral, // "..."
    
    // Keywords
    Int,
    Return,
    If, Else, While, For,

    // Operators and Symbols
    Plus, Minus, Star, Slash, Percent,
    Assign,        // =
    Equals,        // ==
    NotEquals,     // !=
    Less,          // <
    LessEqual,     // <=
    Greater,       // >
    GreaterEqual,  // >=
    LogicalAnd,    // &&
    LogicalOr,     // ||
    SemiColon,     // ;
    Comma,         // ,
    LeftParen,     // (
    RightParen,    // )
    LeftBrace,     // {
    RightBrace,    // }
    Ampersand,     // &

    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek(int offset = 0) const;
    char advance();
    bool match(char expected);
    bool isAtEnd(int offset = 0) const;
    void skipWhitespace();
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
};
