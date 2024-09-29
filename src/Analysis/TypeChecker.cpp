#include "Analysis/TypeChecker.h"
#include "ADT/NonOwningList.h"

namespace {

template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

} // namespace

namespace lang {

TextError TypeCheckerError::toTextError() const {
    switch (kind) {
    case TypeCheckerErrorKind::InvalidReturn:
        return {span, "Invalid return statement", "Return type mismatch"};
    case TypeCheckerErrorKind::InvalidAssignment:
        return {span, "Invalid assignment", "Type mismatch"};
    case TypeCheckerErrorKind::InvalidBinaryOperation:
        return {span, "Invalid binary operation", "Type mismatch"};
    }
    return {span, "Unknown type checking error title",
            "Unknown type checking error label"};
}

JSONError TypeCheckerError::toJSONError() const {
    switch (kind) {
    case TypeCheckerErrorKind::InvalidReturn:
        return {span, "type-check-invalid-return"};
    case TypeCheckerErrorKind::InvalidAssignment:
        return {span, "type-check-invalid-assignment"};
    case TypeCheckerErrorKind::InvalidBinaryOperation:
        return {span, "type-check-invalid-binary-operation"};
    }
    return {span, "type-check-unknown-error"};
}

TypeCheckerResult TypeChecker::analyzeModuleAST(ModuleAST &module) {
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return {std::move(errors)};
}

void TypeChecker::visit(FunctionDeclAST &node) {
    currentFunction = &node;
    ASTVisitor::visit(*node.body);
}

void TypeChecker::visit(ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void TypeChecker::visit(ReturnStmtAST &node) {
    if (node.expr != nullptr) {
        ASTVisitor::visit(*node.expr);
    }

    if (currentFunction->retType == nullptr) {
        if (node.expr != nullptr) {
            errors.push_back({TypeCheckerErrorKind::InvalidReturn, node.span});
        }
    } else {
        if (node.expr == nullptr) {
            errors.push_back({TypeCheckerErrorKind::InvalidReturn, node.span});
        } else {
            if (node.expr->type != currentFunction->retType) {
                errors.push_back(
                    {TypeCheckerErrorKind::InvalidReturn, node.span});
            }
        }
    }
}

void TypeChecker::visit(LocalStmtAST &node) {
    if (node.init != nullptr) {
        ASTVisitor::visit(*node.init);

        if (node.type == nullptr) {
            node.type = node.init->type;
        } else {
            if (node.type != node.init->type) {
                errors.push_back(
                    {TypeCheckerErrorKind::InvalidAssignment, node.span});
            }
        }
    } else {
        if (node.type == nullptr) {
            errors.push_back(
                {TypeCheckerErrorKind::InvalidAssignment, node.span});
        }
    }
}

void TypeChecker::visit(AssignStmtAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);

    if (node.lhs->type != node.rhs->type) {
        errors.push_back({TypeCheckerErrorKind::InvalidAssignment, node.span});
    }
}

void TypeChecker::visit(BlockStmtAST &node) {
    for (auto *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
}

void TypeChecker::visit(IfStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    ASTVisitor::visit(*node.thenStmt);

    if (node.elseStmt != nullptr) {
        ASTVisitor::visit(*node.elseStmt);
    }
}

void TypeChecker::visit(WhileStmtAST &node) {
    ASTVisitor::visit(*node.cond);
    ASTVisitor::visit(*node.body);
}

void TypeChecker::visit(IdentifierExprAST &node) {
    std::visit(Overloaded{
                   [&](const LocalStmtAST *stmt) { node.type = stmt->type; },
                   [&](const FunctionDeclAST *decl) {
                       NonOwningList<Type *> list;
                       for (auto *param : decl->params) {
                           list.emplace_back(arena, param->type);
                       }
                       list.emplace_back(arena, decl->retType);
                       node.type = typeCtx->make<FunctionType>(list);
                   },
               },
               node.decl);
}

void TypeChecker::visit(NumberExprAST &node) {
    node.type = typeCtx->getTypeNumber();
}

void TypeChecker::visit(UnaryExprAST &node) {
    ASTVisitor::visit(*node.expr);

    node.type = node.expr->type;
}

void TypeChecker::visit(BinaryExprAST &node) {
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);

    if (node.lhs->type != node.rhs->type) {
        errors.push_back(
            {TypeCheckerErrorKind::InvalidBinaryOperation, node.span});
    }

    node.type = node.lhs->type;
}

void TypeChecker::visit(CallExprAST &node) {
    ASTVisitor::visit(*node.callee);
    for (auto &arg : node.args) {
        ASTVisitor::visit(*arg);
    }

    // TODO: Verify that the callee is a function and that the argument types
    // match the parameter types

    // TODO: Set the type of the call expression correctly
    node.type = node.callee->type;
}

void TypeChecker::visit(IndexExprAST &node) {
    ASTVisitor::visit(*node.base);
    ASTVisitor::visit(*node.index);

    node.type = node.base->type;
}

void TypeChecker::visit(GroupedExprAST &node) {
    ASTVisitor::visit(*node.expr);

    node.type = node.expr->type;
}

} // namespace lang
