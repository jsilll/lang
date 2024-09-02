#include "Frontend/ASTPrinter.h"

namespace {

template <class... Ts> struct Overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

struct Identation {
  int &level;
  Identation(int &level) : level(level) { ++level; }
  ~Identation() { --level; }
};

} // namespace

#define INDENT()                                                               \
  Identation identation(level);                                                \
  ident();

namespace lang {

void ASTPrinter::ident() {
  for (int i = 0; i < level; ++i) {
    os << "  ";
  }
}

void ASTPrinter::visit(const ModuleAST &node) {
  INDENT();
  os << "ModuleAST: " << node.ident << "\n";
  for (const DeclAST *decl : node.decls) {
    ASTVisitor::visit(*decl);
  }
}

void ASTPrinter::visit(const FunctionDeclAST &node) {
  INDENT();
  os << "FunctionDeclAST: " << node.ident << "\n";
  ASTVisitor::visit(*node.body);
}

void ASTPrinter::visit(const ExprStmtAST &node) {
  INDENT();
  os << "ExprStmtAST\n";
  ASTVisitor::visit(*node.expr);
}

void ASTPrinter::visit(const LocalStmtAST &node) {
  INDENT();
  os << "LocalStmtAST: " << (node.isConst ? "let" : "mut") << " " << node.span
     << "\n";
  if (node.expr) {
    ASTVisitor::visit(*node.expr);
  }
}

void ASTPrinter::visit(const BlockStmtAST &node) {
  INDENT();
  os << "BlockStmtAST\n";
  for (const StmtAST *stmt : node.body) {
    ASTVisitor::visit(*stmt);
  }
}

void ASTPrinter::visit(const IdentifierExprAST &node) {
  INDENT();
  os << "IdentifierExprAST: " << node.span << " (";
  std::visit(Overloaded{[&](const std::monostate) { os << "unresolved"; },
                        [&](const LocalStmtAST *stmt) {
                          os << "LocalStmtAST("
                             << static_cast<const void *>(stmt) << ")";
                        },
                        [&](const FunctionDeclAST *decl) {
                          os << "FunctionDeclAST("
                             << static_cast<const void *>(decl) << ")";
                        }},
             node.reference);
  os << ")\n";
}

void ASTPrinter::visit(const NumberExprAST &node) {
  INDENT();
  os << "NumberExprAST: " << node.span << "\n";
}

void ASTPrinter::visit(const UnaryExprAST &node) {
  INDENT();
  os << "UnaryExprAST\n";
  ASTVisitor::visit(*node.expr);
}

void ASTPrinter::visit(const BinaryExprAST &node) {
  INDENT();
  os << "BinaryExprAST\n";
  ASTVisitor::visit(*node.lhs);
  ASTVisitor::visit(*node.rhs);
}

void ASTPrinter::visit(const CallExprAST &node) {
  INDENT();
  os << "CallExprAST\n";
  ASTVisitor::visit(*node.callee);
  ASTVisitor::visit(*node.arg);
}

void ASTPrinter::visit(const IndexExprAST &node) {
  INDENT();
  os << "IndexExprAST\n";
  ASTVisitor::visit(*node.base);
  ASTVisitor::visit(*node.index);
}

} // namespace lang
