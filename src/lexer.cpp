#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {}

bool isAtEnd(size_t p, const std::string& src) {
    return p >= src.length();
}

bool Lexer::isAtEnd(int offset) const {
    return pos + offset >= source.length();
}

char Lexer::peek(int offset) const {
    if (isAtEnd(offset)) return '\0';
    return source[pos + offset];
}

char Lexer::advance() {
    char c = peek();
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            // Single-line comment
            while (peek() != '\n' && peek() != '\0') advance();
        } else if (c == '/' && peek(1) == '*') {
            // Multi-line comment
            advance(); advance();
            while (peek() != '\0' && !(peek() == '*' && peek(1) == '/')) {
                advance();
            }
            if (peek() != '\0') {
                advance(); advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::readIdentifierOrKeyword() {
    int startLine = line;
    int startColumn = column;
    std::string text;

    while (std::isalnum(peek()) || peek() == '_') {
        text += advance();
    }

    TokenType type = TokenType::Identifier;
    if (text == "int") type = TokenType::Int;
    else if (text == "return") type = TokenType::Return;
    else if (text == "if") type = TokenType::If;
    else if (text == "else") type = TokenType::Else;
    else if (text == "while") type = TokenType::While;
    else if (text == "for") type = TokenType::For;

    return {type, text, startLine, startColumn};
}

Token Lexer::readNumber() {
    int startLine = line;
    int startColumn = column;
    std::string text;

    while (std::isdigit(peek())) {
        text += advance();
    }

    return {TokenType::Number, text, startLine, startColumn};
}

Token Lexer::readString() {
    int startLine = line;
    int startColumn = column;
    std::string text;

    advance(); 

    while (peek() != '"' && !isAtEnd(0)) {
        if (peek() == '\\') {
            text += advance();
        }
        text += advance();
    }

    if (peek() == '"') {
        advance(); 
    }

    return {TokenType::StringLiteral, text, startLine, startColumn};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < source.length()) {
        skipWhitespace();
        if (pos >= source.length()) break;

        int startLine = line;
        int startColumn = column;
        char c = peek();

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
            continue;
        }

        if (std::isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }

        if (c == '"') {
            tokens.push_back(readString());
            continue;
        }

        advance(); 
        TokenType type = TokenType::Unknown;
        std::string text(1, c);

        switch (c) {
            case '+': type = TokenType::Plus; break;
            case '-': type = TokenType::Minus; break;
            case '*': type = TokenType::Star; break;
            case '/': type = TokenType::Slash; break;
            case '%': type = TokenType::Percent; break;
            case ';': type = TokenType::SemiColon; break;
            case ',': type = TokenType::Comma; break;
            case '(': type = TokenType::LeftParen; break;
            case ')': type = TokenType::RightParen; break;
            case '{': type = TokenType::LeftBrace; break;
            case '}': type = TokenType::RightBrace; break;
            case '=':
                if (match('=')) { type = TokenType::Equals; text = "=="; }
                else { type = TokenType::Assign; }
                break;
            case '!':
                if (match('=')) { type = TokenType::NotEquals; text = "!="; }
                break;
            case '<':
                if (match('=')) { type = TokenType::LessEqual; text = "<="; }
                else { type = TokenType::Less; }
                break;
            case '>':
                if (match('=')) { type = TokenType::GreaterEqual; text = ">="; }
                else { type = TokenType::Greater; }
                break;
            case '&':
                if (match('&')) { type = TokenType::LogicalAnd; text = "&&"; }
                else { type = TokenType::Ampersand; }
                break;
            case '|':
                if (match('|')) { type = TokenType::LogicalOr; text = "||"; }
                break;
        }
        
        if (type == TokenType::Unknown) {
            throw std::runtime_error("Lexical Error at line " + std::to_string(startLine) + ":" + std::to_string(startColumn) + " - Unexpected character '" + std::string(1, c) + "'");
        }

        tokens.push_back({type, text, startLine, startColumn});
    }

    tokens.push_back({TokenType::EndOfFile, "EOF", line, column});
    return tokens;
}
