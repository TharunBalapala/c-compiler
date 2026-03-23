#pragma once
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>


struct Symbol {
    std::string name;
    std::string type; 
    int offset; 
};

class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;
    int currentOffset; 

    Scope(Scope* p = nullptr, int startOffset = 0) : parent(p), currentOffset(startOffset) {}

    bool declare(const std::string& name, const std::string& type, int size = 8) {
        if (symbols.count(name)) return false; 
        currentOffset -= size; 
        symbols[name] = {name, type, currentOffset};
        std::cout << "  [SymbolTable] Registered '" << name << "' (type: " << type << ") at stack offset " << currentOffset << "\n";
        return true;
    }

    Symbol* resolve(const std::string& name) {
        if (symbols.count(name)) return &symbols[name];
        if (parent) return parent->resolve(name);
        return nullptr;
    }
};

class SemanticAnalyzer {
public:
    void analyze(ProgramNode* ast);
    
private:
    Scope* currentScope = nullptr;
    std::vector<std::unique_ptr<Scope>> scopes;
    void enterScope();
    void leaveScope();

    void visit(FunctionDeclNode* node);
    void visit(BlockStmtNode* node);
    void visit(StmtNode* node);
    void visit(ExprNode* node);
};
