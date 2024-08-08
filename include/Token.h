#ifndef LANG_TOKEN_H
#define LANG_TOKEN_H

#include <string_view>

namespace lang {

enum class TokenKind : int {
  LParen = '(',
  RParen = ')',
  Colon = ':',
  Semi = ';',
  LBrace = '{',
  RBrace = '}',

  KwFn = -1,
  KwVoid = -2,

  Number = -3,
  Identifier = -4,

  Comment = -5,
};

struct Token {
  TokenKind kind;
  std::string_view span;
  std::string toString() const;
};

} // namespace lang

#endif // LANG_TOKEN_H