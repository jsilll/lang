#include "Analysis/CFA.h"

namespace lang {

TextError CFAError::toTextError() const {
    switch (kind) {
    case CFAErrorKind::EarlyReturnStmt:
        return {span, "Early return statement",
                "Code after return statement will never be executed"};
    case CFAErrorKind::InvalidBreakStmt:
        return {span, "Invalid break statement",
                "Break statement outside loop"};
    }
    return {span, "Uknown control flow analysis error title",
            "Unknown control flow analysis error label"};
}

JSONError CFAError::toJSONError() const {
    switch (kind) {
    case CFAErrorKind::EarlyReturnStmt:
        return {span, "cfa-early-return-stmt"};
    case CFAErrorKind::InvalidBreakStmt:
        return {span, "cfa-invalid-break-stmt"};
    }
    return {span, "cfa-unknown-error"};
}

CFAResult CFA::analyzeModuleAST(ModuleAST &module) {
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return {std::move(errors)};
}

void CFA::visit(FunctionDeclAST &node) { visit(*node.body); }

void CFA::visit(BreakStmtAST &node) {
    if (breakableStack.empty()) {
        errors.push_back({CFAErrorKind::InvalidBreakStmt, node.span});
        return;
    }

    node.stmt = breakableStack.top();
}

void CFA::visit(BlockStmtAST &node) {
    std::size_t i = 0;
    for (auto *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);

        ++i;
        if (stmt->kind == StmtASTKind::Break ||
            stmt->kind == StmtASTKind::Return) {
            if (i != node.stmts.size()) {
                errors.push_back({CFAErrorKind::EarlyReturnStmt, stmt->span});
                break;
            }
        }
    }
}

void CFA::visit(IfStmtAST &node) {
    ASTVisitor::visit(*node.thenBranch);
    if (node.elseBranch != nullptr) {
        ASTVisitor::visit(*node.elseBranch);
    }
}

void CFA::visit(WhileStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    breakableStack.push(&node);
    visit(*node.body);
    breakableStack.pop();
}

} // namespace lang