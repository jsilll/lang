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
      derived().visit(as<NumberExprAST>(node));
      break;
    case ExprASTKind::Identifier:
      derived().visit(as<IdentifierExprAST>(node));
      break;
    case ExprASTKind::Unary:
      derived().visit(as<UnaryExprAST>(node));
      break;
    case ExprASTKind::Binary:
      derived().visit(as<BinaryExprAST>(node));
      break;
    case ExprASTKind::Call:
      derived().visit(as<CallExprAST>(node));
      break;
    case ExprASTKind::Index:
      derived().visit(as<IndexExprAST>(node));
      break;
    }
  }

  void visit(MaybeConst<StmtAST> &node) {
    switch (node.kind) {
    case StmtASTKind::Expr:
      derived().visit(as<ExprStmtAST>(node));
      break;
    case StmtASTKind::Local:
      derived().visit(as<LocalStmtAST>(node));
      break;
    case StmtASTKind::Block:
      derived().visit(as<BlockStmtAST>(node));
      break;
    }
  }

  void visit(MaybeConst<DeclAST> &node) {
    switch (node.kind) {
    case DeclASTKind::Function:
      derived().visit(as<FunctionDeclAST>(node));
      break;
    }
  }

private:
  Derived &derived() { return static_cast<Derived &>(*this); }
  template <typename T> static MaybeConst<T> &as(MaybeConst<T> &node) {
    return static_cast<MaybeConst<T> &>(node);
  }
};

template <typename Derived>
using ConstASTVisitor = ASTVisitor<Derived, true>;

template <typename Derived>
using MutableASTVisitor = ASTVisitor<Derived, false>;

} // namespace lang

#endif // LANG_AST_VISITOR_H
