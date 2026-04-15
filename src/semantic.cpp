#include "semantic.h"
#include <iostream>
#include <stdexcept>

void SemanticAnalyzer::analyze(ProgramNode* ast) {
    for (const auto& func : ast->functions) {
        visit(func.get());
    }
}

void SemanticAnalyzer::enterScope() {
    int startOffset = currentScope ? currentScope->currentOffset : 0;
    scopes.push_back(std::make_unique<Scope>(currentScope, startOffset));
    currentScope = scopes.back().get();
}

void SemanticAnalyzer::leaveScope() {
    if (currentScope) {
        currentScope = currentScope->parent;
    }
}

void SemanticAnalyzer::visit(FunctionDeclNode* node) {
    enterScope();
    
    // Add parameters to scope
    for (const auto& param : node->parameters) {
        if (!currentScope->declare(param, "int")) {
            std::cerr << "\x1b[31mSemantic Error:\x1b[0m Parameter '" << param << "' already defined." << std::endl;
            throw std::runtime_error("Semantic error");
        }
    }
    
    visit(node->body.get());
    
    leaveScope();
}

void SemanticAnalyzer::visit(BlockStmtNode* node) {
    enterScope();
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
    leaveScope();
}

void SemanticAnalyzer::visit(StmtNode* node) {
    if (auto* varDecl = dynamic_cast<VarDeclNode*>(node)) {
        if (varDecl->initializer) {
            visit(varDecl->initializer.get());
        }
        if (!currentScope->declare(varDecl->name, "int")) {
            std::cerr << "\x1b[31mSemantic Error:\x1b[0m Variable '" << varDecl->name << "' already declared in this scope." << std::endl;
            throw std::runtime_error("Semantic error");
        }
    } else if (auto* exprStmt = dynamic_cast<ExprStmtNode*>(node)) {
        visit(exprStmt->expr.get());
    } else if (auto* returnStmt = dynamic_cast<ReturnStmtNode*>(node)) {
        if (returnStmt->expr) {
            visit(returnStmt->expr.get());
        }
    } else if (auto* ifStmt = dynamic_cast<IfStmtNode*>(node)) {
        visit(ifStmt->condition.get());
        visit(ifStmt->thenBranch.get());
        if (ifStmt->elseBranch) {
            visit(ifStmt->elseBranch.get());
        }
    } else if (auto* whileStmt = dynamic_cast<WhileStmtNode*>(node)) {
        visit(whileStmt->condition.get());
        visit(whileStmt->body.get());
    } else if (auto* forStmt = dynamic_cast<ForStmtNode*>(node)) {
        // Evaluate in a block to isolate 'init' scope? No, C89 doesn't allow declarations in for(int i=0;) without block, but our parser allows it if we treat init as statement. Wait, if init has a VarDecl, it should be in the current scope or a new scope.
        // Let's create a new scope for the for-loop variables just in case.
        enterScope();
        if (forStmt->init) visit(forStmt->init.get());
        if (forStmt->condition) visit(forStmt->condition.get());
        if (forStmt->increment) visit(forStmt->increment.get());
        visit(forStmt->body.get());
        leaveScope();
    } else if (auto* blockStmt = dynamic_cast<BlockStmtNode*>(node)) {
        visit(blockStmt);
    }
}

void SemanticAnalyzer::visit(ExprNode* node) {
    if (auto* varExpr = dynamic_cast<VariableExprNode*>(node)) {
        Symbol* sym = currentScope->resolve(varExpr->name);
        if (!sym) {
            std::cerr << "\x1b[31mSemantic Error:\x1b[0m Undefined variable '" << varExpr->name << "'." << std::endl;
            throw std::runtime_error("Semantic error");
        }
    } else if (auto* binaryExpr = dynamic_cast<BinaryExprNode*>(node)) {
        visit(binaryExpr->lhs.get());
        visit(binaryExpr->rhs.get());
        
        // If it's an assignment, check if lhs is an L-value
        if (binaryExpr->op == TokenType::Assign) {
            if (!dynamic_cast<VariableExprNode*>(binaryExpr->lhs.get())) {
                std::cerr << "\x1b[31mSemantic Error:\x1b[0m Left hand side of assignment must be a variable." << std::endl;
                throw std::runtime_error("Semantic error");
            }
        }
    } else if (dynamic_cast<NumberExprNode*>(node)) {
        // Nothing to check for integer literals
    } else if (dynamic_cast<StringExprNode*>(node)) {
        // Nothing to check for string literals
    } else if (auto* callExpr = dynamic_cast<CallExprNode*>(node)) {
        // In a full compiler we would check if callee exists in symbol table
        // For standard library support, we blindly allow it for now or check specifically
        for (auto& arg : callExpr->arguments) {
            visit(arg.get());
        }
    } else if (auto* unaryExpr = dynamic_cast<UnaryExprNode*>(node)) {
        visit(unaryExpr->operand.get());
        if (unaryExpr->op == TokenType::Ampersand) {
            if (!dynamic_cast<VariableExprNode*>(unaryExpr->operand.get())) {
                std::cerr << "\x1b[31mSemantic Error:\x1b[0m Address-of operator '&' requires a variable." << std::endl;
                throw std::runtime_error("Semantic error");
            }
        }
    }
}
