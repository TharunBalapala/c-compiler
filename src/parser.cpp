#include "parser.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

const Token& Parser::peek(int offset) const {
    if (pos + offset >= tokens.size()) return tokens.back(); // EOF token
    return tokens[pos + offset];
}

const Token& Parser::advance() {
    if (!isAtEnd()) pos++;
    return peek(-1);
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType expected) {
    if (check(expected)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType expected, const std::string& message) {
    if (check(expected)) {
        advance();
        return;
    }
    std::cerr << "Parse Error at line " << peek().line << ":" << peek().column 
              << " - " << message << std::endl;
    throw std::runtime_error("Parse error");
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

std::unique_ptr<ProgramNode> Parser::parse() {
    std::vector<std::unique_ptr<FunctionDeclNode>> functions;
    while (!isAtEnd()) {
        functions.push_back(parseFunction());
    }
    return std::make_unique<ProgramNode>(std::move(functions));
}

std::unique_ptr<FunctionDeclNode> Parser::parseFunction() {
    expect(TokenType::Int, "Expected return type 'int' for function.");
    
    expect(TokenType::Identifier, "Expected function name.");
    std::string name = peek(-1).text;

    expect(TokenType::LeftParen, "Expected '(' after function name.");
    
    std::vector<std::string> params;
    if (!check(TokenType::RightParen)) {
        do {
            expect(TokenType::Int, "Expected parameter type 'int'.");
            expect(TokenType::Identifier, "Expected parameter name.");
            params.push_back(peek(-1).text);
        } while (match(TokenType::Comma));
    }
    
    expect(TokenType::RightParen, "Expected ')' after parameters.");
    
    std::unique_ptr<BlockStmtNode> body = parseBlock();
    
    return std::make_unique<FunctionDeclNode>(name, std::move(params), std::move(body));
}

std::unique_ptr<BlockStmtNode> Parser::parseBlock() {
    expect(TokenType::LeftBrace, "Expected '{' before block.");
    std::vector<std::unique_ptr<StmtNode>> statements;
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        statements.push_back(parseDeclarationOrStatement());
    }
    
    expect(TokenType::RightBrace, "Expected '}' after block.");
    return std::make_unique<BlockStmtNode>(std::move(statements));
}

std::unique_ptr<StmtNode> Parser::parseDeclarationOrStatement() {
    if (check(TokenType::Int)) {
        return parseVarDeclaration();
    }
    return parseStatement();
}

std::unique_ptr<VarDeclNode> Parser::parseVarDeclaration() {
    expect(TokenType::Int, "Expected 'int'.");
    expect(TokenType::Identifier, "Expected variable name.");
    std::string name = peek(-1).text;
    
    std::unique_ptr<ExprNode> initializer = nullptr;
    if (match(TokenType::Assign)) {
        initializer = parseExpression();
    }
    
    expect(TokenType::SemiColon, "Expected ';' after variable declaration.");
    return std::make_unique<VarDeclNode>(name, std::move(initializer));
}

std::unique_ptr<StmtNode> Parser::parseStatement() {
    if (match(TokenType::If)) return parseIfStatement();
    if (match(TokenType::While)) return parseWhileStatement();
    if (match(TokenType::Return)) return parseReturnStatement();
    if (check(TokenType::LeftBrace)) return parseBlock();
    
    return parseExpressionStatement();
}

std::unique_ptr<StmtNode> Parser::parseIfStatement() {
    expect(TokenType::LeftParen, "Expected '(' after 'if'.");
    std::unique_ptr<ExprNode> condition = parseExpression();
    expect(TokenType::RightParen, "Expected ')' after if condition.");
    
    std::unique_ptr<StmtNode> thenBranch = parseStatement();
    std::unique_ptr<StmtNode> elseBranch = nullptr;
    
    if (match(TokenType::Else)) {
        elseBranch = parseStatement();
    }
    
    return std::make_unique<IfStmtNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<StmtNode> Parser::parseWhileStatement() {
    expect(TokenType::LeftParen, "Expected '(' after 'while'.");
    std::unique_ptr<ExprNode> condition = parseExpression();
    expect(TokenType::RightParen, "Expected ')' after while condition.");
    
    std::unique_ptr<StmtNode> body = parseStatement();
    
    return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body));
}

std::unique_ptr<StmtNode> Parser::parseReturnStatement() {
    std::unique_ptr<ExprNode> value = nullptr;
    if (!check(TokenType::SemiColon)) {
        value = parseExpression();
    }
    expect(TokenType::SemiColon, "Expected ';' after return value.");
    return std::make_unique<ReturnStmtNode>(std::move(value));
}

std::unique_ptr<ExprStmtNode> Parser::parseExpressionStatement() {
    std::unique_ptr<ExprNode> expr = parseExpression();
    expect(TokenType::SemiColon, "Expected ';' after expression.");
    return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<ExprNode> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ExprNode> Parser::parseAssignment() {
    std::unique_ptr<ExprNode> expr = parseLogicalOr();
    
    if (match(TokenType::Assign)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> value = parseAssignment();
        // Here we ideally check that 'expr' is an l-value (VariableExprNode).
        return std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(value));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseLogicalOr() {
    std::unique_ptr<ExprNode> expr = parseLogicalAnd();
    while (match(TokenType::LogicalOr)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseLogicalAnd();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseLogicalAnd() {
    std::unique_ptr<ExprNode> expr = parseEquality();
    while (match(TokenType::LogicalAnd)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseEquality();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseEquality() {
    std::unique_ptr<ExprNode> expr = parseRelational();
    while (match(TokenType::Equals) || match(TokenType::NotEquals)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseRelational();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseRelational() {
    std::unique_ptr<ExprNode> expr = parseAdditive();
    while (match(TokenType::Less) || match(TokenType::LessEqual) || 
           match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseAdditive();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseAdditive() {
    std::unique_ptr<ExprNode> expr = parseMultiplicative();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseMultiplicative();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseMultiplicative() {
    std::unique_ptr<ExprNode> expr = parseUnary();
    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> right = parseUnary();
        expr = std::make_unique<BinaryExprNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parseUnary() {
    if (match(TokenType::Ampersand)) {
        TokenType op = peek(-1).type;
        std::unique_ptr<ExprNode> operand = parseUnary();
        return std::make_unique<UnaryExprNode>(op, std::move(operand));
    }
    return parsePrimary();
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExprNode>(std::stoi(peek(-1).text));
    }
    if (match(TokenType::StringLiteral)) {
        return std::make_unique<StringExprNode>(peek(-1).text);
    }
    if (match(TokenType::Identifier)) {
        std::string name = peek(-1).text;
        
        // Function call
        if (match(TokenType::LeftParen)) {
            std::vector<std::unique_ptr<ExprNode>> args;
            if (!check(TokenType::RightParen)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::Comma));
            }
            expect(TokenType::RightParen, "Expected ')' after arguments.");
            return std::make_unique<CallExprNode>(name, std::move(args));
        }
        
        return std::make_unique<VariableExprNode>(name);
    }
    if (match(TokenType::LeftParen)) {
        std::unique_ptr<ExprNode> expr = parseExpression();
        expect(TokenType::RightParen, "Expected ')' after expression.");
        return expr;
    }
    std::cerr << "Parse Error at line " << peek().line << ":" << peek().column 
              << " - Expected expression." << std::endl;
    throw std::runtime_error("Parse error");
}
