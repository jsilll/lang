#include "Frontend/Token.h"

#include <sstream>

namespace lang {

std::string tokenKindToString(lang::TokenKind kind) {
  const int kindInt = static_cast<int>(kind);
  if (kindInt >= static_cast<int>(lang::TokenKind::Amp) &&
      kindInt <= static_cast<int>(lang::TokenKind::RBrace)) {
    return "'" + std::string(1, static_cast<char>(kindInt)) + "'";
  }

  switch (kind) {
  case lang::TokenKind::AmpAmp:
    return "'&&'";
  case lang::TokenKind::KwFn:
    return "'fn'";
  case lang::TokenKind::KwVoid:
    return "'void'";
  case lang::TokenKind::KwNumber:
    return "'number'";
  case lang::TokenKind::KwLet:
    return "'let'";
  case lang::TokenKind::KwVar:
    return "'var'";
  case lang::TokenKind::KwIf:
    return "'if'";
  case lang::TokenKind::KwElse:
    return "'else'";
  case lang::TokenKind::KwWhile:
    return "'while'";
  case lang::TokenKind::KwReturn:
    return "'return'";
  case lang::TokenKind::Ident:
    return "an identifier";
  case lang::TokenKind::Number:
    return "a number";
  case lang::TokenKind::Comment:
    return "a comment";
  default:
    return "an unknown token";
  }
}

std::string Token::toString() const {
  std::ostringstream oss;

  const auto kindInt = static_cast<int>(this->kind);
  if (kindInt >= static_cast<int>(TokenKind::KwReturn) &&
      kindInt <= static_cast<int>(TokenKind::KwFn)) {
    oss << "Keyword(";
  } else {
    switch (kind) {
    case lang::TokenKind::Number:
      oss << "Number(";
      break;
    case lang::TokenKind::Ident:
      oss << "Identifier(";
      break;
    case lang::TokenKind::Comment:
      oss << "Comment(";
      break;
    default:
      return std::string(span);
    }
  }

  oss << span << ")";
  return oss.str();
}

} // namespace lang
