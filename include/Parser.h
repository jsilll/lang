#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include "Token.h"

#include "AST.h"
#include "Arena.h"

#include <vector>

namespace lang {

class Parser {
public:
  Parser(Arena &arena, const std::vector<Token> &tokens)
      : arena(&arena), cur(tokens.begin()), end(tokens.end()) {}

  ModuleAST *parseModule() { return arena->alloc<ModuleAST>(); }

private:
  const Token *peek() {
    if (cur == end) {
      return nullptr;
    }
    return &*cur;
  }

  const Token *consume() {
    if (cur == end) {
      return nullptr;
    }
    return &*cur++;
  }

  Arena *arena;
  std::vector<Token>::const_iterator cur;
  std::vector<Token>::const_iterator end;
};

} // namespace lang

#endif // LANG_PARSER_H
