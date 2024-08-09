#ifndef LANG_AST_H
#define LANG_AST_H

#include "ADT/List.h"

#include "Type.h"

#include <string_view>
#include <variant>

namespace lang {

enum class UnOpKind {
  Neg,
  Not,
};

enum class BinOpKind {
  Add,
  Sub,
  Mul,
  Div,
  Eq,
  Ne,
  Lt,
  Le,
  Gt,
  Ge,
  And,
  Or,
};

enum class ExprASTKind {
  Number,
  Identifier,
  Unary,
  Binary,
  Call,
  Index,
};

struct ExprAST {
  ExprASTKind kind;
  std::string_view span;
  ExprAST(ExprASTKind kind, std::string_view span) : kind(kind), span(span) {}
};

enum class StmtASTKind {
  Expr,
  Local,
  Block,
};

struct StmtAST {
  StmtASTKind kind;
  std::string_view span;
  StmtAST(StmtASTKind kind, std::string_view span) : kind(kind), span(span) {}
};

enum class DeclASTKind {
  Function,
};

struct DeclAST {
  DeclASTKind kind;
  std::string_view ident;
  DeclAST(DeclASTKind kind, std::string_view ident)
      : kind(kind), ident(ident) {}
};

struct ModuleAST {
  std::string_view ident;
  List<DeclAST *> decls;
  ModuleAST(std::string_view ident, List<DeclAST *> decls)
      : ident(ident), decls(decls) {}
};

// === Expressions ===

struct NumberExprAST : public ExprAST {
  NumberExprAST(std::string_view span) : ExprAST(ExprASTKind::Number, span) {}
};

struct UnaryExprAST : public ExprAST {
  UnOpKind op;
  ExprAST *expr;
  UnaryExprAST(std::string_view span, UnOpKind op, ExprAST *expr)
      : ExprAST(ExprASTKind::Unary, span), op(op), expr(expr) {}
};

struct BinaryExprAST : public ExprAST {
  BinOpKind op;
  ExprAST *lhs;
  ExprAST *rhs;
  BinaryExprAST(std::string_view span, BinOpKind op, ExprAST *lhs, ExprAST *rhs)
      : ExprAST(ExprASTKind::Binary, span), op(op), lhs(lhs), rhs(rhs) {}
};

struct CallExprAST : public ExprAST {
  ExprAST *callee;
  ExprAST *arg;
  // NOLINTNEXTLINE
  CallExprAST(std::string_view span, ExprAST *callee, ExprAST *arg)
      : ExprAST(ExprASTKind::Call, span), callee(callee), arg(arg) {}
};

struct IndexExprAST : public ExprAST {
  ExprAST *base;
  ExprAST *index;
  // NOLINTNEXTLINE
  IndexExprAST(std::string_view span, ExprAST *base, ExprAST *index)
      : ExprAST(ExprASTKind::Index, span), base(base), index(index) {}
};

// === Statements ===

struct ExprStmtAST : public StmtAST {
  ExprAST *expr;
  ExprStmtAST(std::string_view span, ExprAST *expr)
      : StmtAST(StmtASTKind::Expr, span), expr(expr) {}
};

struct LocalStmtAST : public StmtAST {
  std::string_view ident;
  ExprAST *expr;
  LocalStmtAST(std::string_view ident, ExprAST *expr)
      : StmtAST(StmtASTKind::Local, ident), ident(ident), expr(expr) {}
};

struct BlockStmtAST : public StmtAST {
  List<StmtAST *> body;
  BlockStmtAST(std::string_view span, List<StmtAST *> body)
      : StmtAST(StmtASTKind::Block, span), body(body) {}
};

/// === Declarations ===

struct FunctionDeclAST : public DeclAST {
  Type *type;
  BlockStmtAST *body;
  FunctionDeclAST(std::string_view ident, Type *type, BlockStmtAST *body)
      : DeclAST(DeclASTKind::Function, ident), type(type), body(body) {}
};

/// === Identifier Expressions ===

struct IdentifierExprAST : public ExprAST {
  using ReferenceVariant =
      std::variant<std::monostate, LocalStmtAST *, FunctionDeclAST *>;
  ReferenceVariant reference;
  IdentifierExprAST(std::string_view span)
      : ExprAST(ExprASTKind::Identifier, span) {}
};

} // namespace lang

#endif // LANG_AST_H
