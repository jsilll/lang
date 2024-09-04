#ifndef LANG_RESOLVER_H
#define LANG_RESOLVER_H

#include "Frontend/AST.h"
#include "Frontend/ASTVisitor.h"

#include "Support/Reporting.h"

#include <unordered_map>
#include <vector>

namespace lang {

enum class ResolveErrorKind {
    UnresolvedIdentifier,
};

struct ResolveError {
    ResolveErrorKind kind;
    std::string_view span;
    ResolveError(ResolveErrorKind kind, std::string_view span)
        : kind(kind), span(span) {}
    PrettyError toPretty() const;
};

struct ResolveResult {
    std::vector<ResolveError> errors;
};

class Resolver : public MutableASTVisitor<Resolver> {
    friend class ASTVisitor<Resolver, false>;

  public:
    Resolver() : deepResolution(false) {}

    ResolveResult resolveModuleAST(ModuleAST &module);

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

    LocalStmtAST *lookupLocal(std::string_view ident) const;

    bool deepResolution;
    std::unordered_map<std::string_view, FunctionDeclAST *> functions;
    std::vector<std::unordered_map<std::string_view, LocalStmtAST *>> locals;
    std::vector<ResolveError> errors;
};

} // namespace lang

#endif // LANG_RESOLVER_H
