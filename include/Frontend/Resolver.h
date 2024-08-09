#ifndef LANG_RESOLVER_H
#define LANG_RESOLVER_H

#include "Frontend/AST.h"
#include "Frontend/ASTVisitor.h"

#include <unordered_map>
#include <vector>

namespace lang {

class Resolver : public MutableASTVisitor<Resolver> {
  friend class MutableASTVisitor<Resolver>;

public:
  void resolve(ModuleAST &module);

private:
  void visit(FunctionDeclAST &node);

  void visit(ExprStmtAST &node);

  void visit(LocalStmtAST &node);

  void visit(BlockStmtAST &node);

  void visit(IdentifierExprAST &node);

  void visit(NumberExprAST &node);

  void visit(UnaryExprAST &node);

  void visit(BinaryExprAST &node);

  void visit(CallExprAST &node);

  void visit(IndexExprAST &node);

  LocalStmtAST* lookupLocal(std::string_view ident) const;

  std::unordered_map<std::string_view, FunctionDeclAST*> functions;
  std::vector<std::unordered_map<std::string_view, LocalStmtAST*>> locals;
};

} // namespace lang

#endif // LANG_RESOLVER_H
