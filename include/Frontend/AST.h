#ifndef LANG_AST_H
#define LANG_AST_H

#include "ADT/List.h"

#include "Type.h"

#include <string_view>
#include <variant>

namespace lang {

enum class UnOpKind : int {
    Neg = '-',
    Not = '!',
};

std::string unOpKindToString(UnOpKind kind);

enum class BinOpKind : int {
    Mul = '*',
    Add = '+',
    Sub = '-',
    Div = '/',
    Lt = '<',
    Eq = '=',
    Gt = '>',
    Ne = -1,
    Le = -2,
    Ge = -3,
    And = -4,
    Or = -5,
};

std::string binOpKindToString(BinOpKind kind);

// == ExprAST ==

enum class ExprASTKind {
    Number,
    Identifier,
    Unary,
    Binary,
    Call,
    Index,
    Grouped,
};

struct ExprAST {
    ExprASTKind kind;
    std::string_view span;
    ExprAST(ExprASTKind kind, std::string_view span) : kind(kind), span(span) {}
};

// == StmtAST ==

enum class StmtASTKind {
    Expr,
    Break,
    Return,
    Local,
    Assign,
    Block,
    If,
    While,
};

struct StmtAST {
    StmtASTKind kind;
    std::string_view span;
    StmtAST(StmtASTKind kind, std::string_view span) : kind(kind), span(span) {}
};

// == DeclAST ==

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
    BinaryExprAST(std::string_view span, BinOpKind op, ExprAST *lhs,
                  ExprAST *rhs)
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

struct GroupedExprAST : public ExprAST {
    ExprAST *expr;
    GroupedExprAST(std::string_view span, ExprAST *expr)
        : ExprAST(ExprASTKind::Grouped, span), expr(expr) {}
};

// === Statements ===

struct ExprStmtAST : public StmtAST {
    ExprAST *expr;
    ExprStmtAST(std::string_view span, ExprAST *expr)
        : StmtAST(StmtASTKind::Expr, span), expr(expr) {}
};

struct BreakStmtAST : public StmtAST {
    StmtAST *stmt;
    BreakStmtAST(std::string_view span)
        : StmtAST(StmtASTKind::Break, span), stmt(nullptr) {}
};

struct ReturnStmtAST : public StmtAST {
    ExprAST *expr;
    ReturnStmtAST(std::string_view span, ExprAST *expr)
        : StmtAST(StmtASTKind::Return, span), expr(expr) {}
};

struct LocalStmtAST : public StmtAST {
    bool isConst;
    Type *type;
    ExprAST *expr;
    LocalStmtAST(bool isConst, std::string_view ident, Type *type,
                 ExprAST *expr)
        : StmtAST(StmtASTKind::Local, ident), isConst(isConst), type(type),
          expr(expr) {}
};

struct AssignStmtAST : public StmtAST {
    ExprAST *lhs;
    ExprAST *rhs;
    AssignStmtAST(std::string_view span, ExprAST *lhs, ExprAST *rhs)
        : StmtAST(StmtASTKind::Assign, span), lhs(lhs), rhs(rhs) {}
};

struct BlockStmtAST : public StmtAST {
    List<StmtAST *> body;
    BlockStmtAST(std::string_view span, List<StmtAST *> body)
        : StmtAST(StmtASTKind::Block, span), body(body) {}
};

struct IfStmtAST : public StmtAST {
    ExprAST *cond;
    BlockStmtAST *body;
    IfStmtAST(std::string_view span, ExprAST *cond, BlockStmtAST *body)
        : StmtAST(StmtASTKind::If, span), cond(cond), body(body) {}
};

struct WhileStmtAST : public StmtAST {
    ExprAST *cond;
    BlockStmtAST *body;
    WhileStmtAST(std::string_view span, ExprAST *cond, BlockStmtAST *body)
        : StmtAST(StmtASTKind::While, span), cond(cond), body(body) {}
};

/// === Declarations ===

struct FunctionDeclAST : public DeclAST {
    List<LocalStmtAST *> params;
    Type *type;
    BlockStmtAST *body;
    FunctionDeclAST(std::string_view ident, List<LocalStmtAST *> params,
                    Type *type, BlockStmtAST *body)
        : DeclAST(DeclASTKind::Function, ident), params(params), type(type),
          body(body) {}
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
