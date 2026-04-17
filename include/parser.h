#pragma once
#include <vector>
#include <memory>
#include "lexer.h"
#include "ast.h"


class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    const Token& peek(int offset = 0) const;
    const Token& advance();
    bool match(TokenType expected);
    void expect(TokenType expected, const std::string& message);
    bool check(TokenType type) const;
    bool isAtEnd() const;

    // Recursive Descent Methods
    std::unique_ptr<FunctionDeclNode> parseFunction();
    std::unique_ptr<StmtNode> parseStatement();
    std::unique_ptr<BlockStmtNode> parseBlock();
    std::unique_ptr<StmtNode> parseIfStatement();
    std::unique_ptr<StmtNode> parseWhileStatement();
    std::unique_ptr<StmtNode> parseForStatement();
    std::unique_ptr<StmtNode> parseReturnStatement();
    std::unique_ptr<StmtNode> parseDeclarationOrStatement();
    std::unique_ptr<VarDeclNode> parseVarDeclaration();
    std::unique_ptr<ExprStmtNode> parseExpressionStatement();

    // Expression Parsing (Recursive Descent with Precedence)
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parseAssignment();
    std::unique_ptr<ExprNode> parseLogicalOr();
    std::unique_ptr<ExprNode> parseLogicalAnd();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseRelational();
    std::unique_ptr<ExprNode> parseAdditive();
    std::unique_ptr<ExprNode> parseMultiplicative();
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parsePrimary();
};
