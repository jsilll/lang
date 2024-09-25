#include "Analysis/Resolver.h"

#include <cassert>

namespace lang {

TextError ResolveError::toTextError() const {
    switch (kind) {
    case ResolveErrorKind::UnknownIdentifier:
        return {span, "Unknown identifier", "Unknown identifier"};
    }
    return {span, "Unknown resolution error title",
            "Unknown resolve error label"};
}

JSONError ResolveError::toJSONError() const {
    switch (kind) {
    case ResolveErrorKind::UnknownIdentifier:
        return {span, "resolve-unknown-identifier"};
    }
    return {span, "resolve-unknown-error"};
}

ResolveResult Resolver::resolveModuleAST(ModuleAST &module) {
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    deepResolution = true;
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return {std::move(errors)};
}

LocalStmtAST *Resolver::lookupLocal(std::string_view ident) const {
    for (auto it = localsMap.rbegin(); it != localsMap.rend(); ++it) {
        const auto &locals = *it;
        const auto found = locals.find(ident);
        if (found != locals.end()) {
            return found->second;
        }
    }
    return nullptr;
}

void Resolver::visit(FunctionDeclAST &node) {
    if (!deepResolution) {
        functionsMap.insert({node.ident, &node});
    } else {
        localsMap.emplace_back();
        for (auto *param : node.params) {
            visit(*param);
        }
        visit(*node.body);
        localsMap.pop_back();
    }
}

void Resolver::visit(ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(ReturnStmtAST &node) {
    if (node.expr != nullptr) {
        ASTVisitor::visit(*node.expr);
    }
}

void Resolver::visit(LocalStmtAST &node) {
    assert(!localsMap.empty());
    localsMap.back().insert({node.span, &node});
    if (node.init != nullptr) {
        ASTVisitor::visit(*node.init);
    }
}

void Resolver::visit(AssignStmtAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);
}

void Resolver::visit(BlockStmtAST &node) {
    localsMap.emplace_back();
    for (auto *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
    localsMap.pop_back();
}

void Resolver::visit(IfStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    visit(*node.thenStmt);
    if (node.elseStmt != nullptr) {
        ASTVisitor::visit(*node.elseStmt);
    }
}

void Resolver::visit(WhileStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    visit(*node.body);
}

void Resolver::visit(IdentifierExprAST &node) {
    LocalStmtAST *local = lookupLocal(node.span);
    if (local != nullptr) {
        node.decl = local;
        return;
    }

    const auto it = functionsMap.find(node.span);
    if (it == functionsMap.end()) {
        errors.push_back({ResolveErrorKind::UnknownIdentifier, node.span});
        return;
    }

    node.decl = it->second;
}

void Resolver::visit(UnaryExprAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(BinaryExprAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);
}

void Resolver::visit(CallExprAST &node) {
    ASTVisitor::visit(*node.callee);
    ASTVisitor::visit(*node.arg);
}

void Resolver::visit(IndexExprAST &node) {
    ASTVisitor::visit(*node.base);
    ASTVisitor::visit(*node.index);
}

void Resolver::visit(GroupedExprAST &node) { ASTVisitor::visit(*node.expr); }

} // namespace lang
