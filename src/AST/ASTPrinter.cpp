#include "AST/ASTPrinter.h"

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
    Identation identation(level);                                              \
    ident();

namespace lang {

void ASTPrinter::visit(const ModuleAST &node) {
    INDENT();
    os << "ModuleAST: " << node.ident << '\n';
    for (const DeclAST *decl : node.decls) {
        ASTVisitor::visit(*decl);
    }
}

void ASTPrinter::visit(const FunctionDeclAST &node) {
    INDENT();
    os << "FunctionDeclAST: " << node.ident << ": " << node.retType->toString()
       << '\n';
    for (auto *param : node.params) {
        visit(*param);
    }
    ASTVisitor::visit(*node.body);
}

void ASTPrinter::visit(const ExprStmtAST &node) {
    INDENT();
    os << "ExprStmtAST\n";
    ASTVisitor::visit(*node.expr);
}

void ASTPrinter::visit(const BreakStmtAST &node) {
    INDENT();
    os << "BreakStmtAST: ";
    if (node.stmt == nullptr) {
        os << "(unresolved)\n";
    } else {
        os << "StmtAST(" << static_cast<const void *>(node.stmt) << ")\n";
    }
}

void ASTPrinter::visit(const ReturnStmtAST &node) {
    INDENT();
    os << "ReturnStmtAST\n";
    if (node.expr != nullptr) {
        ASTVisitor::visit(*node.expr);
    }
}

void ASTPrinter::visit(const LocalStmtAST &node) {
    INDENT();
    os << "LocalStmtAST: " << (node.isConst ? "let" : "mut") << ' '
       << node.span;
    if (node.type) {
        os << ": " << node.type->toString();
    }
    os << '\n';
    if (node.init) {
        ASTVisitor::visit(*node.init);
    }
}

void ASTPrinter::visit(const AssignStmtAST &node) {
    INDENT();
    os << "AssignStmtAST\n";
    ASTVisitor::visit(*node.lhs);
    ASTVisitor::visit(*node.rhs);
}

void ASTPrinter::visit(const BlockStmtAST &node) {
    INDENT();
    os << "BlockStmtAST\n";
    for (const StmtAST *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
}

void ASTPrinter::visit(const IfStmtAST &node) {
    INDENT();
    os << "IfStmtAST\n";
    ASTVisitor::visit(*node.cond);
    visit(*node.thenBranch);
    if (node.elseBranch != nullptr) {
        ASTVisitor::visit(*node.elseBranch);
    }
}

void ASTPrinter::visit(const WhileStmtAST &node) {
    INDENT();
    os << "WhileStmtAST\n";
    ASTVisitor::visit(*node.cond);
    visit(*node.body);
}

void ASTPrinter::visit(const IdentifierExprAST &node) {
    INDENT();
    os << "IdentifierExprAST: " << node.span;
    std::visit(Overloaded{[&](const std::monostate) {},
                          [&](const LocalStmtAST *stmt) {
                              os << " => LocalStmtAST("
                                 << static_cast<const void *>(stmt) << ')';
                          },
                          [&](const FunctionDeclAST *decl) {
                              os << " => FunctionDeclAST("
                                 << static_cast<const void *>(decl) << ')';
                          }},
               node.decl);
    os << '\n';
}

void ASTPrinter::visit(const NumberExprAST &node) {
    INDENT();
    os << "NumberExprAST: " << node.span << '\n';
}

void ASTPrinter::visit(const UnaryExprAST &node) {
    INDENT();
    os << "UnaryExprAST: " << unOpKindToString(node.op) << '\n';
    ASTVisitor::visit(*node.expr);
}

void ASTPrinter::visit(const BinaryExprAST &node) {
    INDENT();
    os << "BinaryExprAST: " << binOpKindToString(node.op) << '\n';
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

void ASTPrinter::visit(const GroupedExprAST &node) {
    INDENT();
    os << "GroupedExprAST\n";
    ASTVisitor::visit(*node.expr);
}

void ASTPrinter::ident() {
    for (int i = 0; i < level; ++i) {
        os << "  ";
    }
}

} // namespace lang
