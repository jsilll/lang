#include "Frontend/Resolver.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>

namespace lang {

void Resolver::resolve(ModuleAST &module) {
  for (const auto &decl : module.decls) {
    ASTVisitor::visit(*decl);
  }
  deepResolution = true;
  for (const auto &decl : module.decls) {
    ASTVisitor::visit(*decl);
  }
}

void Resolver::visit(FunctionDeclAST &node) {
  if (!deepResolution) {
    functions.insert({node.ident, &node});
  } else {
    locals.emplace_back();
    for (auto *param : node.params) {
      visit(*param);
    }
    visit(*node.body);
    locals.pop_back();
  }
}

void Resolver::visit(ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(LocalStmtAST &node) {
  assert(!locals.empty());
  locals.back().insert({node.span, &node});
  if (node.expr != nullptr) {
    ASTVisitor::visit(*node.expr);
  }
}

void Resolver::visit(ReturnStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(BlockStmtAST &node) {
  locals.emplace_back();
  for (const auto &stmt : node.body) {
    ASTVisitor::visit(*stmt);
  }
  locals.pop_back();
}

void Resolver::visit(IdentifierExprAST &node) {
  const auto it = functions.find(node.span);
  if (it != functions.end()) {
    node.reference = it->second;
    return;
  }

  LocalStmtAST *local = lookupLocal(node.span);
  if (local != nullptr) {
    node.reference = local;
  }
}

void Resolver::visit(NumberExprAST &node) {}

void Resolver::visit(UnaryExprAST &node) { ASTVisitor::visit(*node.expr); }

void Resolver::visit(BinaryExprAST &node) {
  ASTVisitor::visit(*node.lhs);
  ASTVisitor::visit(*node.rhs);
}

void Resolver::visit(CallExprAST &node) {
  ASTVisitor::visit(*node.callee);
  ASTVisitor::visit(*node.arg);
}

void Resolver::visit(IndexExprAST &node) {
  ASTVisitor::visit(*node.base);
  ASTVisitor::visit(*node.index);
}

LocalStmtAST *Resolver::lookupLocal(std::string_view ident) const {
  for (auto it = locals.rbegin(); it != locals.rend(); ++it) {
    llvm::outs() << "Trying a scope\n";
    const auto &locals = *it;
    const auto found = locals.find(ident);
    if (found != locals.end()) {
      return found->second;
    }
  }
  llvm::outs() << "Didn't find any local for " << ident << "\n";
  return nullptr;
}

} // namespace lang
