#ifndef LANG_CODEGEN_H
#define LANG_CODEGEN_H

#include "AST/AST.h"
#include "AST/ASTVisitor.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

namespace lang {

class Codegen : public ConstASTVisitor<Codegen> {
    friend class ASTVisitor<Codegen, true>;

  public:
    Codegen()
        : deepCodegen(false), context(std::make_unique<llvm::LLVMContext>()),
          builder(std::make_unique<llvm::IRBuilder<>>(*context)) {}

    llvm::Module *generateModule(const ModuleAST &module);

  private:
    bool deepCodegen = false;
    llvm::Value *exprResult = nullptr;
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> llvmModule = nullptr;
    std::unordered_map<const StmtAST *, llvm::BasicBlock *> breakTargets;
    std::unordered_map<const LocalStmtAST *, llvm::AllocaInst *> namedValues;
    std::unordered_map<const FunctionDeclAST *, llvm::Function *> functionTable;

    void visit(const FunctionDeclAST &node);

    void visit(const ExprStmtAST &node);

    void visit(const BreakStmtAST &node);

    void visit(const ReturnStmtAST &node);

    void visit(const LocalStmtAST &node);

    void visit(const AssignStmtAST &node);

    void visit(const BlockStmtAST &node);

    void visit(const IfStmtAST &node);

    void visit(const WhileStmtAST &node);

    void visit(const IdentifierExprAST &node);

    void visit(const NumberExprAST &node);

    void visit(const UnaryExprAST &node);

    void visit(const BinaryExprAST &node);

    void visit(const CallExprAST &node);

    void visit(const IndexExprAST &node);

    void visit(const GroupedExprAST &node);
};

} // namespace lang

#endif // LANG_CODEGEN_H