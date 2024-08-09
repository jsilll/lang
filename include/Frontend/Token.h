#ifndef LANG_TOKEN_H
#define LANG_TOKEN_H

#include <string_view>

namespace lang {

enum class TokenKind : int {
  Amp = '&',
  LParen = '(',
  RParen = ')',
  Star = '*',
  Plus = '+',
  Minus = '-',
  Dot = '.',
  Slash = '/',
  Colon = ':',
  Semicolon = ';',
  Less = '<',
  Equal = '=',
  Greater = '>',
  LBracket = '[',
  RBracket = ']',
  LBrace = '{',
  RBrace = '}',

  AmpAmp = -1,

  KwFn = -2,
  KwVoid = -3,
  KwLet = -4,
  KwVar = -5,
  KwIf = -6,
  KwElse = -7,
  KwWhile = -8,
  KwReturn = -9,

  Ident = -10,
  Number = -11,

  Comment = -12,
};

std::string tokenKindToString(TokenKind kind);

struct Token {
  TokenKind kind;
  std::string_view span;
  std::string toString() const;
};

} // namespace lang

#endif // LANG_TOKEN_H