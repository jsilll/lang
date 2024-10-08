#ifndef LANG_AST_PRINTER_H
#define LANG_AST_PRINTER_H

#include "AST/ASTVisitor.h"

#include "llvm/Support/raw_ostream.h"

namespace lang {

class ASTPrinter : public ConstASTVisitor<ASTPrinter> {
  public:
    explicit ASTPrinter(llvm::raw_ostream &os) : level(-1), os(os) {}

    void visit(const ModuleAST &node);

    void visit(const FunctionDeclAST &node);

    void visit(const ExprStmtAST &node);

    void visit(const BreakStmtAST &node);

    void visit(const ReturnStmtAST &node);

    void visit(const LocalStmtAST &node);

    void visit(const AssignStmtAST &node);

    void visit(const BlockStmtAST &node);

    void visit(const IfStmtAST &node);

    void visit(const WhileStmtAST &node);

    void visit(const IdentifierExprAST &node);

    void visit(const NumberExprAST &node);

    void visit(const UnaryExprAST &node);

    void visit(const BinaryExprAST &node);

    void visit(const CallExprAST &node);

    void visit(const IndexExprAST &node);

    void visit(const GroupedExprAST &node);

  private:
    int level;
    llvm::raw_ostream &os;

    void ident();
};

} // namespace lang

#endif // LANG_AST_PRINTER_H
