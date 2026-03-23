#include "codegen.h"
#include <stdexcept>

CodeGenerator::CodeGenerator(const std::string& outputFile) {
    out.open(outputFile);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << outputFile << std::endl;
        throw std::runtime_error("Could not open output file");
    }
}

CodeGenerator::~CodeGenerator() {
    if (out.is_open()) {
        out.close();
    }
}

std::string CodeGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCount++);
}

std::string CodeGenerator::formatOffset(int offset) const {
    if (offset >= 0) return "+" + std::to_string(offset);
    return std::to_string(offset); // negative already includes '-'
}

void CodeGenerator::enterScope() {
    int startOffset = currentScope ? currentScope->currentOffset : 0;
    scopes.push_back(std::make_unique<Scope>(currentScope, startOffset));
    currentScope = scopes.back().get();
}

void CodeGenerator::leaveScope() {
    if (currentScope) {
        currentScope = currentScope->parent;
    }
}

void CodeGenerator::generate(ProgramNode* ast) {
    out << ".intel_syntax noprefix\n";
    
    out << ".text\n";
    out << ".global _main\n";
    
    out << ".extern _printf\n";
    out << ".extern _scanf\n";

    for (const auto& func : ast->functions) {
        visit(func.get());
    }

    
    if (!strings.empty()) {
        out << "\n.data\n";
        for (const auto& pair : strings) {
            out << pair.second << ":\n";
            out << "  .asciz \"" << pair.first << "\"\n";
        }
    }
}

void CodeGenerator::visit(FunctionDeclNode* node) {
    if (node->name == "main") out << "_main:\n";
    else out << "_" << node->name << ":\n";
    
    // Prologue
    out << "  push ebp\n";
    out << "  mov ebp, esp\n";
    out << "  sub esp, 64\n";

    enterScope();
    

    int paramOffset = 8;
    for (const auto& param : node->parameters) {
        currentScope->symbols[param] = {param, "int", paramOffset};
        paramOffset += 4;
    }
    
    visit(node->body.get());
    
    leaveScope();
    
    // Default epilogue in case missing return
    out << "  mov eax, 0\n";
    out << "  leave\n";
    out << "  ret\n";
}

void CodeGenerator::visit(BlockStmtNode* node) {
    enterScope();
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
    leaveScope();
}

void CodeGenerator::visit(StmtNode* node) {
    if (auto* varDecl = dynamic_cast<VarDeclNode*>(node)) {
        currentScope->declare(varDecl->name, "int");
        auto sym = currentScope->resolve(varDecl->name);
        
        if (varDecl->initializer) {
            visit(varDecl->initializer.get());
            // Result is in eax, store it in the local var on stack
            out << "  mov dword ptr [ebp" << formatOffset(sym->offset) << "], eax\n";
        } else {
            // Default initialize to 0
            out << "  mov dword ptr [ebp" << formatOffset(sym->offset) << "], 0\n";
        }
    } else if (auto* exprStmt = dynamic_cast<ExprStmtNode*>(node)) {
        visit(exprStmt->expr.get());
    } else if (auto* returnStmt = dynamic_cast<ReturnStmtNode*>(node)) {
        if (returnStmt->expr) {
            visit(returnStmt->expr.get());
        }
        // Result is in rax
        // Epilogue
        out << "  leave\n";
        out << "  ret\n";
    } else if (auto* ifStmt = dynamic_cast<IfStmtNode*>(node)) {
        std::string elseLabel = newLabel(".L_else");
        std::string endLabel = newLabel(".L_end");
        
        visit(ifStmt->condition.get());
        out << "  cmp eax, 0\n";
        if (ifStmt->elseBranch) {
            out << "  je " << elseLabel << "\n";
        } else {
            out << "  je " << endLabel << "\n";
        }
        
        visit(ifStmt->thenBranch.get());
        
        if (ifStmt->elseBranch) {
            out << "  jmp " << endLabel << "\n";
            out << elseLabel << ":\n";
            visit(ifStmt->elseBranch.get());
        }
        
        out << endLabel << ":\n";

    } else if (auto* whileStmt = dynamic_cast<WhileStmtNode*>(node)) {
        std::string startLabel = newLabel(".L_while_start");
        std::string endLabel = newLabel(".L_while_end");

        out << startLabel << ":\n";
        visit(whileStmt->condition.get());
        out << "  cmp eax, 0\n";
        out << "  je " << endLabel << "\n";

        visit(whileStmt->body.get());
        out << "  jmp " << startLabel << "\n";
        out << endLabel << ":\n";
    } else if (auto* blockStmt = dynamic_cast<BlockStmtNode*>(node)) {
        visit(blockStmt);
    }
}

void CodeGenerator::visit(ExprNode* node) {
    if (auto* numExpr = dynamic_cast<NumberExprNode*>(node)) {
        out << "  mov eax, " << numExpr->value << "\n";
    } else if (auto* strExpr = dynamic_cast<StringExprNode*>(node)) {
        // Track the string and give it a label
        if (!strings.count(strExpr->value)) {
            strings[strExpr->value] = "LC" + std::to_string(stringCount++);
        }
        out << "  lea eax, [" << strings[strExpr->value] << "]\n";
    } else if (auto* varExpr = dynamic_cast<VariableExprNode*>(node)) {
        auto sym = currentScope->resolve(varExpr->name);
        out << "  mov eax, dword ptr [ebp" << formatOffset(sym->offset) << "]\n";
    } else if (auto* callExpr = dynamic_cast<CallExprNode*>(node)) {
        // Evaluate arguments (CDECL pushes them right-to-left)
        for (auto it = callExpr->arguments.rbegin(); it != callExpr->arguments.rend(); ++it) {
            visit(it->get());
            out << "  push eax\n";
        }
        
        out << "  call _" << callExpr->callee << "\n";
        
        // Clean up stack
        if (!callExpr->arguments.empty()) {
            out << "  add esp, " << (callExpr->arguments.size() * 4) << "\n";
        }
    } else if (auto* unaryExpr = dynamic_cast<UnaryExprNode*>(node)) {
        if (unaryExpr->op == TokenType::Ampersand) {
            auto* varExpr = dynamic_cast<VariableExprNode*>(unaryExpr->operand.get());
            auto sym = currentScope->resolve(varExpr->name);
            out << "  lea eax, [ebp" << formatOffset(sym->offset) << "]\n";
        }
    } else if (auto* binaryExpr = dynamic_cast<BinaryExprNode*>(node)) {
        
        if (binaryExpr->op == TokenType::Assign) {
            // lhs must be a variable
            auto* varExpr = dynamic_cast<VariableExprNode*>(binaryExpr->lhs.get());
            auto sym = currentScope->resolve(varExpr->name);
            
            visit(binaryExpr->rhs.get());
            // Store eax into the stack location
            out << "  mov dword ptr [ebp" << formatOffset(sym->offset) << "], eax\n";
            return;
        }

        // Standard binary op: evaluate RHS, push to stack, evaluate LHS
        visit(binaryExpr->rhs.get());
        out << "  push eax\n";
        visit(binaryExpr->lhs.get());
        out << "  pop ecx\n"; // ECX contains RHS, EAX contains LHS

        switch (binaryExpr->op) {
            case TokenType::Plus:  out << "  add eax, ecx\n"; break;
            case TokenType::Minus: out << "  sub eax, ecx\n"; break;
            case TokenType::Star:  out << "  imul eax, ecx\n"; break;
            case TokenType::Slash: 
                out << "  cdq\n"; // sign extend EAX into EDX:EAX
                out << "  idiv ecx\n"; 
                break;
            case TokenType::Percent:
                out << "  cdq\n";
                out << "  idiv ecx\n";
                out << "  mov eax, edx\n"; // remainder is in edx
                break;
            case TokenType::Equals:
                out << "  cmp eax, ecx\n";
                out << "  sete al\n";
                out << "  movzx eax, al\n";
                break;
            case TokenType::NotEquals:
                out << "  cmp eax, ecx\n";
                out << "  setne al\n";
                out << "  movzx eax, al\n";
                break;
            case TokenType::Less:
                out << "  cmp eax, ecx\n";
                out << "  setl al\n";
                out << "  movzx eax, al\n";
                break;
            case TokenType::LessEqual:
                out << "  cmp eax, ecx\n";
                out << "  setle al\n";
                out << "  movzx eax, al\n";
                break;
            case TokenType::Greater:
                out << "  cmp eax, ecx\n";
                out << "  setg al\n";
                out << "  movzx eax, al\n";
                break;
            case TokenType::GreaterEqual:
                out << "  cmp eax, ecx\n";
                out << "  setge al\n";
                out << "  movzx eax, al\n";
                break;
            default:
                break;
        }
    }
}
