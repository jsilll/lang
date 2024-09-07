#include "Sema/Resolver.h"

#include <cassert>

namespace lang {

PrettyError ResolveError::toPretty() const {
    switch (kind) {
    case ResolveErrorKind::UnresolvedIdentifier:
        return {span, "Unresolved identifier", "Cannot be resolved"};
    }
    return {span, "Unknown resolve error title", "Unknown resolve error label"};
}

ResolveResult Resolver::resolveModuleAST(ModuleAST &module) {
    for (const auto &decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    deepResolution = true;
    for (const auto &decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return {std::move(errors)};
}

void Resolver::visit(FunctionDeclAST &node) {
    if (!deepResolution) {
        functions.insert({node.ident, &node});
    } else {
        locals.emplace_back();
        for (auto *param : node.params) {
            visit(*param);
        }
        visit(*node.body);
        locals.pop_back();
    }
}

void Resolver::visit(ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(BreakStmtAST &node) {
    // TODO: Handle break statements outside of loops (or maybe this should be
    // done in the resolver?)
}

void Resolver::visit(ReturnStmtAST &node) {
    if (node.expr != nullptr) {
        ASTVisitor::visit(*node.expr);
    }
}

void Resolver::visit(LocalStmtAST &node) {
    assert(!locals.empty());
    locals.back().insert({node.span, &node});
    if (node.init != nullptr) {
        ASTVisitor::visit(*node.init);
    }
}

void Resolver::visit(AssignStmtAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);
}

void Resolver::visit(BlockStmtAST &node) {
    locals.emplace_back();
    for (const auto &stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
    locals.pop_back();
}

void Resolver::visit(IfStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    visit(*node.thenBranch);
    if (node.elseBranch != nullptr) {
        ASTVisitor::visit(*node.elseBranch);
    }
}

void Resolver::visit(WhileStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    visit(*node.body);
}

void Resolver::visit(IdentifierExprAST &node) {
    const auto it = functions.find(node.span);
    if (it != functions.end()) {
        node.decl = it->second;
        return;
    }

    LocalStmtAST *local = lookupLocal(node.span);
    if (local == nullptr) {
        errors.push_back({ResolveErrorKind::UnresolvedIdentifier, node.span});
        return;
    }

    node.decl = local;
}

void Resolver::visit(NumberExprAST &node) {}

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

LocalStmtAST *Resolver::lookupLocal(std::string_view ident) const {
    for (auto it = locals.rbegin(); it != locals.rend(); ++it) {
        const auto &locals = *it;
        const auto found = locals.find(ident);
        if (found != locals.end()) {
            return found->second;
        }
    }
    return nullptr;
}

} // namespace lang
