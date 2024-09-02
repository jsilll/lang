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
  Comma = ',',
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
  KwNumber = -4,
  KwLet = -5,
  KwVar = -6,
  KwIf = -7,
  KwElse = -8,
  KwWhile = -9,
  KwReturn = -10,

  Ident = -11,
  Number = -12,

  Comment = -13,
};

std::string tokenKindToString(TokenKind kind);

struct Token {
  TokenKind kind;
  std::string_view span;
  std::string toString() const;
};

} // namespace lang

#endif // LANG_TOKEN_H
