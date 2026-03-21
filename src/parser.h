#pragma once
#include <vector>
#include <memory>
#include "lexer.h"
#include "ast.h"

/**
 * ============================================================================
 * PHASE 2: SYNTAX ANALYSIS (The Parser)
 * ============================================================================
 * The Parser takes the linear stream of Tokens from the Lexer and constructs
 * a hierarchical data structure called an Abstract Syntax Tree (AST). The AST
 * represents the grammatical structure of the program based on C language rules.
 * 
 * Implementation Approach: Recursive Descent Parsing
 * This is a top-down parsing technique built from a set of mutually recursive
 * functions (like `parseStatement()`, `parseExpression()`) that directly map 
 * to the language's grammar rules. Operator precedence is encoded naturally 
 * by the depth of the recursive calls.
 */

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
