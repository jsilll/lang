#include "Codegen/Codegen.h"

namespace {

template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

} // namespace

namespace lang {

llvm::Module *Codegen::generate(const ModuleAST &module) {
    llvmModule = std::make_unique<llvm::Module>("main", *context);
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    deepCodegen = true;
    for (auto *decl : module.decls) {
        ASTVisitor::visit(*decl);
    }
    return llvmModule.get();
}

void Codegen::visit(const FunctionDeclAST &node) {
    if (!deepCodegen) {
        // TODO: implement proper function signatures
        auto *funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(*context), false /* isVarArg */);
        auto *func =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                   node.ident, llvmModule.get());
        functionTable[&node] = func;
    } else {
        auto *func = functionTable.at(&node);
        auto *entry = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(entry);
        // for (auto *arg : node.params) {
        //     auto *alloca =
        //     builder->CreateAlloca(llvm::Type::getFloatTy(*context),
        //                                          nullptr, arg->span);
        //     builder->CreateStore(arg->name, alloca);
        //     namedValues[arg] = alloca;
        // }
        ASTVisitor::visit(*node.body);
        builder->CreateRetVoid();
    }
}

void Codegen::visit(const ExprStmtAST &node) { ASTVisitor::visit(*node.expr); }

void Codegen::visit(const BreakStmtAST &node) {
    builder->CreateBr(breakTargets.at(node.target));
}

void Codegen::visit(const ReturnStmtAST &node) {
    if (node.expr) {
        ASTVisitor::visit(*node.expr);
        builder->CreateRet(exprResult);
    } else {
        builder->CreateRetVoid();
    }
}

void Codegen::visit(const LocalStmtAST &node) { /* TODO */
}

void Codegen::visit(const AssignStmtAST &node) { /* TODO */
}

void Codegen::visit(const BlockStmtAST &node) {
    for (auto *stmt : node.stmts) {
        ASTVisitor::visit(*stmt);
    }
}

void Codegen::visit(const IfStmtAST &node) {
    auto *func = builder->GetInsertBlock()->getParent();
    auto *thenBB = llvm::BasicBlock::Create(*context, "then", func);
    auto *elseBB = llvm::BasicBlock::Create(*context, "else", func);
    auto *mergeBB = llvm::BasicBlock::Create(*context, "ifcont", func);

    ASTVisitor::visit(*node.cond);
    builder->CreateCondBr(exprResult, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    ASTVisitor::visit(*node.thenStmt);
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(elseBB);
    if (node.elseStmt) {
        ASTVisitor::visit(*node.elseStmt);
    }
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(mergeBB);
}

void Codegen::visit(const WhileStmtAST &node) {
    auto *func = builder->GetInsertBlock()->getParent();
    auto *condBB = llvm::BasicBlock::Create(*context, "cond", func);
    auto *bodyBB = llvm::BasicBlock::Create(*context, "body", func);
    auto *afterBB = llvm::BasicBlock::Create(*context, "after", func);
    breakTargets[&node] = afterBB;

    builder->CreateBr(condBB);

    builder->SetInsertPoint(condBB);
    ASTVisitor::visit(*node.cond);
    builder->CreateCondBr(exprResult, bodyBB, afterBB);

    builder->SetInsertPoint(bodyBB);
    ASTVisitor::visit(*node.body);
    builder->CreateBr(condBB);

    builder->SetInsertPoint(afterBB);
}

void Codegen::visit(const IdentifierExprAST &node) {
    std::visit(Overloaded{
                   [&](const std::monostate &decl) { exprResult = nullptr; },
                   [&](const LocalStmtAST *decl) {
                       exprResult = namedValues.at(decl);
                   },
                   [&](const FunctionDeclAST *decl) { exprResult = nullptr; },
               },
               node.decl);
}

void Codegen::visit(const NumberExprAST &node) {
    exprResult = llvm::ConstantFP::get(
        *context, llvm::APFloat(std::stof(std::string(node.span))));
}

void Codegen::visit(const UnaryExprAST &node) {}

void Codegen::visit(const BinaryExprAST &node) {
    ASTVisitor::visit(*node.lhs);
    auto *lhs = exprResult;

    ASTVisitor::visit(*node.rhs);
    auto *rhs = exprResult;

    switch (node.op) {
    case BinOpKind::Mul:
        exprResult = builder->CreateFMul(lhs, rhs);
        break;
    case BinOpKind::Add:
        exprResult = builder->CreateFAdd(lhs, rhs);
        break;
    case BinOpKind::Sub:
        exprResult = builder->CreateFSub(lhs, rhs);
        break;
    case BinOpKind::Div:
        exprResult = builder->CreateFDiv(lhs, rhs);
        break;
    case BinOpKind::Lt:
        exprResult = builder->CreateFCmpULT(lhs, rhs);
        break;
    case BinOpKind::Eq:
        exprResult = builder->CreateFCmpUEQ(lhs, rhs);
        break;
    case BinOpKind::Gt:
        exprResult = builder->CreateFCmpUGT(lhs, rhs);
        break;
    case BinOpKind::Ne:
        exprResult = builder->CreateFCmpUNE(lhs, rhs);
        break;
    case BinOpKind::Le:
        exprResult = builder->CreateFCmpULE(lhs, rhs);
        break;
    case BinOpKind::Ge:
        exprResult = builder->CreateFCmpUGE(lhs, rhs);
        break;
    case BinOpKind::And:
        exprResult = builder->CreateAnd(lhs, rhs);
        break;
    case BinOpKind::Or:
        exprResult = builder->CreateOr(lhs, rhs);
        break;
    }
}

void Codegen::visit(const CallExprAST &node) {
    if (node.callee->kind == ExprASTKind::Identifier) {
        auto *callee = static_cast<IdentifierExprAST *>(node.callee);
        std::visit(
            Overloaded{
                [&](const std::monostate &decl) { exprResult = nullptr; },
                [&](const LocalStmtAST *decl) { exprResult = nullptr; },
                [&](const FunctionDeclAST *decl) {
                    exprResult = builder->CreateCall(
                        llvmModule->getFunction(decl->ident),
                        std::vector<llvm::Value *>(1, exprResult));
                },
            },
            callee->decl);
    } else {
        // TODO:
        // ASTVisitor::visit(*node.callee);
        // auto *callee = exprResult;
        // ASTVisitor::visit(*node.arg);
        // auto *arg = exprResult;
        // exprResult = builder->CreateCall(callee, std::vector<llvm::Value
        // *>(1, arg));
    }
}

void Codegen::visit(const IndexExprAST &node) { /* TODO */
}

void Codegen::visit(const GroupedExprAST &node) {
    ASTVisitor::visit(*node.expr);
}

} // namespace lang