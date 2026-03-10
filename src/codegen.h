#pragma once
#include "ast.h"
#include "semantic.h" // We need it for Scope resolution
#include <string>
#include <fstream>
#include <iostream>

class CodeGenerator {
public:
    CodeGenerator(const std::string& outputFile);
    ~CodeGenerator();

    void generate(ProgramNode* ast);

private:
    std::ofstream out;
    int labelCount = 0;
    
    // String literals tracking
    std::unordered_map<std::string, std::string> strings;
    int stringCount = 0;
    
    // We recreate scope tracking purely for stack offsets
    Scope* currentScope = nullptr;
    std::vector<std::unique_ptr<Scope>> scopes;

    void enterScope();
    void leaveScope();

    std::string newLabel(const std::string& prefix = "L");

    void visit(FunctionDeclNode* node);
    void visit(BlockStmtNode* node);
    void visit(StmtNode* node);
    void visit(ExprNode* node);
};
