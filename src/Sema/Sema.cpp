#include "Sema/Sema.h"

namespace {

template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

} // namespace

namespace lang {

TextError SemaError::toTextError() const {
    switch (kind) {
    case SemaErrorKind::InvalidReturn:
        return {span, "Invalid return statement", "Return type mismatch"};
    case SemaErrorKind::InvalidAssignment:
        return {span, "Invalid assignment", "Type mismatch"};
    case SemaErrorKind::InvalidBinaryOperation:
        return {span, "Invalid binary operation", "Type mismatch"};
    }
    return {span, "Unknown sema error title", "Unknown sema error label"};
}

JSONError SemaError::toJSONError() const {
    switch (kind) {
    case SemaErrorKind::InvalidReturn:
        return {span, "sema-invalid-return"};
    case SemaErrorKind::InvalidAssignment:
        return {span, "sema-invalid-assignment"};
    case SemaErrorKind::InvalidBinaryOperation:
        return {span, "sema-invalid-binary-operation"};
    }
    return {span, "sema-unknown-error"};
}

SemaResult Sema::analyzeModuleAST(ModuleAST &module) {
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return {std::move(errors)};
}

void Sema::visit(FunctionDeclAST &node) {
    currentFunction = &node;
    ASTVisitor::visit(*node.body);
}

void Sema::visit(ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Sema::visit(BreakStmtAST &node) { /* no-op */ }

void Sema::visit(ReturnStmtAST &node) {
    if (node.expr != nullptr) {
        ASTVisitor::visit(*node.expr);
    }

    if (currentFunction->retType == nullptr) {
        if (node.expr != nullptr) {
            errors.push_back({SemaErrorKind::InvalidReturn, node.span});
        }
    } else {
        if (node.expr == nullptr) {
            errors.push_back({SemaErrorKind::InvalidReturn, node.span});
        } else {
            if (node.expr->type != currentFunction->retType) {
                errors.push_back({SemaErrorKind::InvalidReturn, node.span});
            }
        }
    }
}

void Sema::visit(LocalStmtAST &node) {
    if (node.init != nullptr) {
        ASTVisitor::visit(*node.init);

        if (node.type == nullptr) {
            node.type = node.init->type;
        } else {
            if (node.type != node.init->type) {
                errors.push_back({SemaErrorKind::InvalidAssignment, node.span});
            }
        }
    } else {
        if (node.type == nullptr) {
            errors.push_back({SemaErrorKind::InvalidAssignment, node.span});
        }
    }
}

void Sema::visit(AssignStmtAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);

    if (node.lhs->type != node.rhs->type) {
        errors.push_back({SemaErrorKind::InvalidAssignment, node.span});
    }
}

void Sema::visit(BlockStmtAST &node) {
    for (auto *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
}

void Sema::visit(IfStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    ASTVisitor::visit(*node.thenBranch);

    if (node.elseBranch != nullptr) {
        ASTVisitor::visit(*node.elseBranch);
    }
}

void Sema::visit(WhileStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    ASTVisitor::visit(*node.body);
}

void Sema::visit(IdentifierExprAST &node) {
    std::visit(
        Overloaded{
            [&](const std::monostate) {},
            [&](const LocalStmtAST *stmt) { node.type = stmt->type; },
            // TODO: here the type should be function pointer
            [&](const FunctionDeclAST *decl) { node.type = decl->retType; },
        },
        node.decl);
}

void Sema::visit(NumberExprAST &node) { node.type = typeCtx.getTypeNumber(); }

void Sema::visit(UnaryExprAST &node) {
    ASTVisitor::visit(*node.expr);

    node.type = node.expr->type;
}

void Sema::visit(BinaryExprAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);

    if (node.lhs->type != node.rhs->type) {
        errors.push_back({SemaErrorKind::InvalidBinaryOperation, node.span});
    }

    node.type = node.lhs->type;
}

void Sema::visit(CallExprAST &node) {
    ASTVisitor::visit(*node.callee);
    ASTVisitor::visit(*node.arg);

    // TODO: Verify that the callee is a function and that the argument types
    // match the parameter types

    // TODO: Set the type of the call expression correctly
    node.type = node.callee->type;
}

void Sema::visit(IndexExprAST &node) {
    ASTVisitor::visit(*node.base);
    ASTVisitor::visit(*node.index);

    node.type = node.base->type;
}

void Sema::visit(GroupedExprAST &node) {
    ASTVisitor::visit(*node.expr);

    node.type = node.expr->type;
}

} // namespace lang
