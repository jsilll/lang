#ifndef LANG_TYPE_CHECKER_H
#define LANG_TYPE_CHECKER_H

#include "Support/Reporting.h"

#include "AST/AST.h"
#include "AST/ASTVisitor.h"

#include "Typing/TypeContext.h"

#include <vector>

namespace lang {

enum class TypeCheckerErrorKind {
    InvalidReturn,
    InvalidAssignment,
    InvalidBinaryOperation,
};

struct TypeCheckerError {
    TypeCheckerErrorKind kind;
    std::string_view span;
    TextError toTextError() const;
    JSONError toJSONError() const;
};

struct TypeCheckerResult {
    std::vector<TypeCheckerError> errors;
};

class TypeChecker : public MutableASTVisitor<TypeChecker> {
    friend class ASTVisitor<TypeChecker, false>;

  public:
    TypeChecker(TypeContext &typeCtx) : typeCtx(typeCtx), currentFunction(nullptr) {}

    TypeCheckerResult analyzeModuleAST(ModuleAST &module);

  private:
    TypeContext &typeCtx;
    FunctionDeclAST *currentFunction;
    std::vector<TypeCheckerError> errors;

    void visit(FunctionDeclAST &node);

    void visit(ExprStmtAST &node);

    void visit(BreakStmtAST &node) {}

    void visit(ReturnStmtAST &node);

    void visit(LocalStmtAST &node);

    void visit(AssignStmtAST &node);

    void visit(BlockStmtAST &node);

    void visit(IfStmtAST &node);

    void visit(WhileStmtAST &node);

    void visit(IdentifierExprAST &node);

    void visit(NumberExprAST &node);

    void visit(UnaryExprAST &node);

    void visit(BinaryExprAST &node);

    void visit(CallExprAST &node);

    void visit(IndexExprAST &node);

    void visit(GroupedExprAST &node);
};

} // namespace lang

#endif // LANG_TYPE_CHECKER
