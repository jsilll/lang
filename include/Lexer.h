#ifndef LANG_LEXER_H
#define LANG_LEXER_H

#include "Token.h"

#include <vector>

namespace lang {

enum class LexErrorKind {
  InvalidCharacter,
};

struct LexError {
  LexErrorKind kind;
  std::string_view span;
};

struct LexResult {
  std::vector<Token> tokens;
  std::vector<LexError> errors;
};

class Lexer {
public:
  Lexer(std::string_view buffer) : idx(0), buffer(buffer) {}

  LexResult lexAll(bool includeComments = false);

private:
  size_t idx;
  std::string_view buffer;
};

} // namespace lang

#endif // LANG_LEXER_H
