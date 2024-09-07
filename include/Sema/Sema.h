#ifndef LANG_SEMA_H
#define LANG_SEMA_H

#include "Support/Reporting.h"

#include "AST/AST.h"
#include "AST/ASTVisitor.h"

#include <vector>

namespace lang {

enum class SemaErrorKind {
    InvalidReturn,
    InvalidAssignment,
    InvalidBinaryOperation,
};

struct SemaError {
    SemaErrorKind kind;
    std::string_view span;
    PrettyError toPretty() const;
};

struct SemaResult {
    std::vector<SemaError> errors;
};

class Sema : public MutableASTVisitor<Sema> {
    friend class ASTVisitor<Sema, false>;

  public:
    Sema(TypeContext &typeCtx) : typeCtx(typeCtx), currentFunction(nullptr) {}

    SemaResult analyzeModuleAST(ModuleAST &module);

  private:
    void visit(FunctionDeclAST &node);

    void visit(ExprStmtAST &node);

    void visit(BreakStmtAST &node);

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

    TypeContext &typeCtx;
    FunctionDeclAST *currentFunction;
    std::vector<SemaError> errors;
};

} // namespace lang

#endif // LANG_SEMA_H
