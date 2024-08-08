#ifndef LANG_AST_H
#define LANG_AST_H

#include <type_traits>

namespace lang {

class ExprAST {};

class StmtAST {};

class DeclAST {};

class ModuleAST {};

} // namespace lang

static_assert(std::is_trivially_destructible<lang::ExprAST>::value,
              "ExprAST must be trivially destructible");

static_assert(std::is_trivially_destructible<lang::StmtAST>::value,
              "StmtAST must be trivially destructible");

static_assert(std::is_trivially_destructible<lang::DeclAST>::value,
              "DeclAST must be trivially destructible");

static_assert(std::is_trivially_destructible<lang::ModuleAST>::value,
              "ModuleAST must be trivially destructible");

#endif // LANG_AST_H
