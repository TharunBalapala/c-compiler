#pragma once
#include "ast.h"
#include <iostream>
#include <string>

class ASTPrinter {
public:
    void print(ProgramNode* ast);
    
private:
    void printIndent(int depth);
    std::string tokenTypeToString(TokenType type);
    
    void visit(FunctionDeclNode* node, int depth);
    void visit(BlockStmtNode* node, int depth);
    void visit(StmtNode* node, int depth);
    void visit(ExprNode* node, int depth);
};
