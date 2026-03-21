#pragma once
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

/**
 * ============================================================================
 * PHASE 3: SEMANTIC ANALYSIS (Type Checking & Scope Resolution)
 * ============================================================================
 * The Semantic Analyzer traverses the Abstract Syntax Tree (AST) to ensure
 * the program obeys the semantic rules of the C language. It catches errors
 * that the Parser cannot (e.g., using undeclared variables).
 * 
 * Core Concept: The Symbol Table & Scoping
 * - We maintain a hierarchical environment (Environment/Scope stack).
 * - Every variable declaration registers a 'Symbol' mapping its name to its
 *   primitive type and memory offset.
 * - Every expression node is validated to ensure variables exist in scope.
 */

struct Symbol {
    std::string name;
    std::string type; // Always "int" for now
    int offset; // For code gen (stack offset)
};

class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;
    int currentOffset; // offset for variables in this scope

    Scope(Scope* p = nullptr, int startOffset = 0) : parent(p), currentOffset(startOffset) {}

    bool declare(const std::string& name, const std::string& type, int size = 8) {
        if (symbols.count(name)) return false; // Already declared in this scope (error)
        currentOffset -= size; // Stack grows downwards
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
    std::vector<std::unique_ptr<Scope>> scopes; // To own the scopes

    void enterScope();
    void leaveScope();

    void visit(FunctionDeclNode* node);
    void visit(BlockStmtNode* node);
    void visit(StmtNode* node);
    void visit(ExprNode* node);
};
