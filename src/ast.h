#pragma once
#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expressions
class ExprNode : public ASTNode {};

class NumberExprNode : public ExprNode {
public:
    int value;
    NumberExprNode(int val) : value(val) {}
};

class VariableExprNode : public ExprNode {
public:
    std::string name;
    VariableExprNode(const std::string& n) : name(n) {}
};

class StringExprNode : public ExprNode {
public:
    std::string value;
    StringExprNode(const std::string& v) : value(v) {}
};

class CallExprNode : public ExprNode {
public:
    std::string callee;
    std::vector<std::unique_ptr<ExprNode>> arguments;
    CallExprNode(const std::string& c, std::vector<std::unique_ptr<ExprNode>> args)
        : callee(c), arguments(std::move(args)) {}
};

class UnaryExprNode : public ExprNode {
public:
    TokenType op; // e.g., TokenType::Ampersand
    std::unique_ptr<ExprNode> operand;
    UnaryExprNode(TokenType op, std::unique_ptr<ExprNode> operand)
        : op(op), operand(std::move(operand)) {}
};

class BinaryExprNode : public ExprNode {
public:
    TokenType op;
    std::unique_ptr<ExprNode> lhs;
    std::unique_ptr<ExprNode> rhs;
    BinaryExprNode(TokenType op, std::unique_ptr<ExprNode> lhs, std::unique_ptr<ExprNode> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

// Statements
class StmtNode : public ASTNode {};

class ExprStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> expr;
    ExprStmtNode(std::unique_ptr<ExprNode> e) : expr(std::move(e)) {}
};

class ReturnStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> expr;
    ReturnStmtNode(std::unique_ptr<ExprNode> e) : expr(std::move(e)) {}
};

class BlockStmtNode : public StmtNode {
public:
    std::vector<std::unique_ptr<StmtNode>> statements;
    BlockStmtNode(std::vector<std::unique_ptr<StmtNode>> stmts) : statements(std::move(stmts)) {}
};

class IfStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> thenBranch;
    std::unique_ptr<StmtNode> elseBranch; // can be null
    IfStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> thenB, std::unique_ptr<StmtNode> elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
};

class WhileStmtNode : public StmtNode {
public:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> body;
    WhileStmtNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

// Declarations
class DeclNode : public StmtNode {};

class VarDeclNode : public DeclNode {
public:
    std::string name;
    std::unique_ptr<ExprNode> initializer; // can be null
    VarDeclNode(const std::string& n, std::unique_ptr<ExprNode> init)
        : name(n), initializer(std::move(init)) {}
};

class FunctionDeclNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> parameters; 
    std::unique_ptr<BlockStmtNode> body;
    FunctionDeclNode(const std::string& n, std::vector<std::string> params, std::unique_ptr<BlockStmtNode> b)
        : name(n), parameters(std::move(params)), body(std::move(b)) {}
};

// Program (Root Node)
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<FunctionDeclNode>> functions;
    ProgramNode(std::vector<std::unique_ptr<FunctionDeclNode>> funcs) : functions(std::move(funcs)) {}
};
