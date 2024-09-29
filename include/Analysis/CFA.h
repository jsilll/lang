#ifndef LANG_CONTROL_FLOW_ANALYZER
#define LANG_CONTROL_FLOW_ANALYZER

#include "Support/Reporting.h"

#include "AST/ASTVisitor.h"

#include <stack>

namespace lang {

enum class CFAErrorKind {
    EarlyBreakStmt,
    EarlyReturnStmt,
    InvalidBreakStmt,
};

struct CFAError {
    CFAErrorKind kind;
    std::string_view span;

    CFAError(CFAErrorKind kind, std::string_view span)
        : kind(kind), span(span) {}

    TextError toTextError() const;
    JSONError toJSONError() const;
};

struct CFAResult {
    std::vector<CFAError> errors;

    CFAResult(std::vector<CFAError> errors) : errors(std::move(errors)) {}

    [[nodiscard]] bool hasErrors() const { return !errors.empty(); }
};

class CFA : public MutableASTVisitor<CFA> {
    friend class ASTVisitor<CFA, false>;

  public:
    CFAResult analyzeModuleAST(ModuleAST &module);

  private:
    std::stack<StmtAST *> breakableStack;
    std::vector<CFAError> errors;

    void visit(FunctionDeclAST &node);

    void visit(ExprStmtAST &node) {}

    void visit(BreakStmtAST &node);

    void visit(ReturnStmtAST &node) {}

    void visit(LocalStmtAST &node) {}

    void visit(AssignStmtAST &node) {}

    void visit(BlockStmtAST &node);

    void visit(IfStmtAST &node);

    void visit(WhileStmtAST &node);

    void visit(IdentifierExprAST &node) {}

    void visit(NumberExprAST &node) {}

    void visit(UnaryExprAST &node) {}

    void visit(BinaryExprAST &node) {}

    void visit(CallExprAST &node) {}

    void visit(IndexExprAST &node) {}

    void visit(GroupedExprAST &node) {}
};

} // namespace lang

#endif // LANG_CONTROL_FLOW_ANALYZER
