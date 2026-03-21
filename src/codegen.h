#pragma once
#include "ast.h"
#include "semantic.h" // We need it for Scope resolution
#include <string>
#include <fstream>
#include <iostream>

/**
 * ============================================================================
 * PHASE 4: CODE GENERATION (Translation to Machine Architecture)
 * ============================================================================
 * The Code Generator visits the validated AST one final time to emit executable 
 * machine code instructions. In this project, we target the x86 32-bit architecture
 * using Intel assembly syntax.
 * 
 * Core Concepts Demoed:
 * - Stack Frames: Using `ebp` and `esp` to manage localized variable memory.
 * - CDECL Calling Convention: Passing arguments on the stack (right-to-left)
 *   to seamlessly interface with the MinGW Standard Library (printf, scanf).
 * - Control Flow Synthesis: Emitting uniquely labeled conditional jumps 
 *   (`jmp`, `je`) to model `if/else` logic and `while` loops.
 */

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
    std::string formatOffset(int offset) const;

    void visit(FunctionDeclNode* node);
    void visit(BlockStmtNode* node);
    void visit(StmtNode* node);
    void visit(ExprNode* node);
};
