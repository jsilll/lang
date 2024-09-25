#ifndef LANG_RESOLVER_H
#define LANG_RESOLVER_H

#include "Support/Reporting.h"

#include "AST/ASTVisitor.h"

#include <unordered_map>

namespace lang {

enum class ResolveErrorKind {
    UnknownIdentifier,
};

struct ResolveError {
    ResolveErrorKind kind;
    std::string_view span;
    ResolveError(ResolveErrorKind kind, std::string_view span)
        : kind(kind), span(span) {}
    TextError toTextError() const;
    JSONError toJSONError() const;
};

struct ResolveResult {
    std::vector<ResolveError> errors;
};

class Resolver : public MutableASTVisitor<Resolver> {
    friend class ASTVisitor<Resolver, false>;

  public:
    ResolveResult resolveModuleAST(ModuleAST &module);

  private:
    bool deepResolution = false;
    std::unordered_map<std::string_view, FunctionDeclAST *> functionsMap;
    std::vector<std::unordered_map<std::string_view, LocalStmtAST *>> localsMap;
    std::vector<ResolveError> errors;

    LocalStmtAST *lookupLocal(std::string_view ident) const;

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

    void visit(NumberExprAST &node) {}

    void visit(UnaryExprAST &node);

    void visit(BinaryExprAST &node);

    void visit(CallExprAST &node);

    void visit(IndexExprAST &node);

    void visit(GroupedExprAST &node);
};

} // namespace lang

#endif // LANG_RESOLVER_H
