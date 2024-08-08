#include "Token.h"

#include <sstream>

namespace lang {

std::string Token::toString() const {
  std::ostringstream oss;

  switch (kind) {
  case TokenKind::KwFn:
  case TokenKind::KwVoid:
    oss << "Keyword(";
    break;
  case lang::TokenKind::Number:
    oss << "Number(";
    break;
  case lang::TokenKind::Identifier:
    oss << "Identifier(";
    break;
  case lang::TokenKind::Comment:
    oss << "Comment(";
    break;
  default:
    return std::string(span);
  }

  oss << span << ")";
  return oss.str();
}

} // namespace lang