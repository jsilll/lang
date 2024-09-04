#ifndef LANG_AST_VISITOR_H
#define LANG_AST_VISITOR_H

#include "AST.h"

namespace lang {

template <typename Derived, bool IsConst = true> class ASTVisitor {
protected:
  ASTVisitor() = default;

  template <typename T>
  using MaybeConst = std::conditional_t<IsConst, const T, T>;

  void visit(MaybeConst<ExprAST> &node) {
    switch (node.kind) {
    case ExprASTKind::Number:
      derived().visit(static_cast<MaybeConst<NumberExprAST> &>(node));
      break;
    case ExprASTKind::Identifier:
      derived().visit(static_cast<MaybeConst<IdentifierExprAST> &>(node));
      break;
    case ExprASTKind::Unary:
      derived().visit(static_cast<MaybeConst<UnaryExprAST> &>(node));
      break;
    case ExprASTKind::Binary:
      derived().visit(static_cast<MaybeConst<BinaryExprAST> &>(node));
      break;
    case ExprASTKind::Call:
      derived().visit(static_cast<MaybeConst<CallExprAST> &>(node));
      break;
    case ExprASTKind::Index:
      derived().visit(static_cast<MaybeConst<IndexExprAST> &>(node));
      break;
    case ExprASTKind::Grouped:
      derived().visit(static_cast<MaybeConst<GroupedExprAST> &>(node));
      break;
    }
  }

  void visit(MaybeConst<StmtAST> &node) {
    switch (node.kind) {
    case StmtASTKind::Expr:
      derived().visit(static_cast<MaybeConst<ExprStmtAST> &>(node));
      break;
    case StmtASTKind::Local:
      derived().visit(static_cast<MaybeConst<LocalStmtAST> &>(node));
      break;
    case StmtASTKind::Return:
      derived().visit(static_cast<MaybeConst<ReturnStmtAST> &>(node));
      break;
    case StmtASTKind::Block:
      derived().visit(static_cast<MaybeConst<BlockStmtAST> &>(node));
      break;
    }
  }

  void visit(MaybeConst<DeclAST> &node) {
    switch (node.kind) {
    case DeclASTKind::Function:
      derived().visit(static_cast<MaybeConst<FunctionDeclAST> &>(node));
      break;
    }
  }

private:
  Derived &derived() { return static_cast<Derived &>(*this); }
};

template <typename Derived> using ConstASTVisitor = ASTVisitor<Derived, true>;

template <typename Derived>
using MutableASTVisitor = ASTVisitor<Derived, false>;

} // namespace lang

#endif // LANG_AST_VISITOR_H
