#include "ast_printer.h"

void ASTPrinter::printIndent(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
}

std::string ASTPrinter::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Percent: return "%";
        case TokenType::Assign: return "=";
        case TokenType::Equals: return "==";
        case TokenType::NotEquals: return "!=";
        case TokenType::Less: return "<";
        case TokenType::LessEqual: return "<=";
        case TokenType::Greater: return ">";
        case TokenType::GreaterEqual: return ">=";
        case TokenType::LogicalAnd: return "&&";
        case TokenType::LogicalOr: return "||";
        case TokenType::Ampersand: return "&";
        default: return "unknown_op";
    }
}

void ASTPrinter::print(ProgramNode* ast) {
    std::cout << "ProgramRoot\n";
    for (const auto& func : ast->functions) {
        visit(func.get(), 1);
    }
}

void ASTPrinter::visit(FunctionDeclNode* node, int depth) {
    printIndent(depth);
    std::cout << "FunctionDecl: " << node->name << "(";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        std::cout << "int " << node->parameters[i];
        if (i < node->parameters.size() - 1) std::cout << ", ";
    }
    std::cout << ") -> int\n";
    visit(node->body.get(), depth + 1);
}

void ASTPrinter::visit(BlockStmtNode* node, int depth) {
    printIndent(depth);
    std::cout << "BlockStatement\n";
    for (const auto& stmt : node->statements) {
        visit(stmt.get(), depth + 1);
    }
}

void ASTPrinter::visit(StmtNode* node, int depth) {
    if (!node) return;
    
    if (auto* varDecl = dynamic_cast<VarDeclNode*>(node)) {
        printIndent(depth);
        std::cout << "VarDecl: int " << varDecl->name << "\n";
        if (varDecl->initializer) {
            printIndent(depth + 1);
            std::cout << "Initializer:\n";
            visit(varDecl->initializer.get(), depth + 2);
        }
    } else if (auto* exprStmt = dynamic_cast<ExprStmtNode*>(node)) {
        printIndent(depth);
        std::cout << "ExpressionStatement\n";
        visit(exprStmt->expr.get(), depth + 1);
    } else if (auto* returnStmt = dynamic_cast<ReturnStmtNode*>(node)) {
        printIndent(depth);
        std::cout << "ReturnStatement\n";
        if (returnStmt->expr) {
            visit(returnStmt->expr.get(), depth + 1);
        }
    } else if (auto* ifStmt = dynamic_cast<IfStmtNode*>(node)) {
        printIndent(depth);
        std::cout << "IfStatement\n";
        printIndent(depth + 1);
        std::cout << "Condition:\n";
        visit(ifStmt->condition.get(), depth + 2);
        printIndent(depth + 1);
        std::cout << "ThenBranch:\n";
        visit(ifStmt->thenBranch.get(), depth + 2);
        if (ifStmt->elseBranch) {
            printIndent(depth + 1);
            std::cout << "ElseBranch:\n";
            visit(ifStmt->elseBranch.get(), depth + 2);
        }
    } else if (auto* whileStmt = dynamic_cast<WhileStmtNode*>(node)) {
        printIndent(depth);
        std::cout << "WhileStatement\n";
        printIndent(depth + 1);
        std::cout << "Condition:\n";
        visit(whileStmt->condition.get(), depth + 2);
        printIndent(depth + 1);
        std::cout << "Body:\n";
        visit(whileStmt->body.get(), depth + 2);
    } else if (auto* blockStmt = dynamic_cast<BlockStmtNode*>(node)) {
        visit(blockStmt, depth);
    }
}

void ASTPrinter::visit(ExprNode* node, int depth) {
    if (!node) return;
    
    if (auto* numExpr = dynamic_cast<NumberExprNode*>(node)) {
        printIndent(depth);
        std::cout << "NumberExpr: " << numExpr->value << "\n";
    } else if (auto* strExpr = dynamic_cast<StringExprNode*>(node)) {
        printIndent(depth);
        std::cout << "StringExpr: \"" << strExpr->value << "\"\n";
    } else if (auto* varExpr = dynamic_cast<VariableExprNode*>(node)) {
        printIndent(depth);
        std::cout << "VariableExpr: " << varExpr->name << "\n";
    } else if (auto* callExpr = dynamic_cast<CallExprNode*>(node)) {
        printIndent(depth);
        std::cout << "CallExpr: " << callExpr->callee << "()\n";
        if (!callExpr->arguments.empty()) {
            printIndent(depth + 1);
            std::cout << "Arguments:\n";
            for (auto& arg : callExpr->arguments) {
                visit(arg.get(), depth + 2);
            }
        }
    } else if (auto* unaryExpr = dynamic_cast<UnaryExprNode*>(node)) {
        printIndent(depth);
        std::cout << "UnaryExpr: " << tokenTypeToString(unaryExpr->op) << "\n";
        visit(unaryExpr->operand.get(), depth + 1);
    } else if (auto* binaryExpr = dynamic_cast<BinaryExprNode*>(node)) {
        printIndent(depth);
        std::cout << "BinaryExpr: " << tokenTypeToString(binaryExpr->op) << "\n";
        printIndent(depth + 1);
        std::cout << "LHS:\n";
        visit(binaryExpr->lhs.get(), depth + 2);
        printIndent(depth + 1);
        std::cout << "RHS:\n";
        visit(binaryExpr->rhs.get(), depth + 2);
    }
}
