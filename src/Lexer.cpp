#include "Lexer.h"

#include <unordered_map>

namespace {

const char charTokens[] = {'(', ')', ':', ';', '{', '}'};

const std::unordered_map<std::string_view, lang::TokenKind> keywordToTokenKind =
    {{"fn", lang::TokenKind::KwFn}, {"void", lang::TokenKind::KwVoid}};

bool isWhite(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isAlnum(char c) { return isAlpha(c) || isDigit(c); }

} // namespace

namespace lang {

LexResult Lexer::lexAll(bool includeComments) {
  LexResult result;
  while (idx < buffer.size()) {
    while (isWhite(buffer[idx])) {
      ++idx;
    }
    if (idx >= buffer.size()) {
      break;
    }
    std::size_t start = idx;
    if (isAlpha(buffer[idx])) {
      while (isAlnum(buffer[idx])) {
        ++idx;
      }
      std::string_view span = buffer.substr(start, idx - start);
      auto it = keywordToTokenKind.find(span);
      if (it != keywordToTokenKind.end()) {
        result.tokens.push_back({it->second, span});
      } else {
        result.tokens.push_back({TokenKind::Identifier, span});
      }
    } else if (isDigit(buffer[idx])) {
      while (isDigit(buffer[idx])) {
        ++idx;
      }
      result.tokens.push_back(
          {TokenKind::Number, buffer.substr(start, idx - start)});
    } else if (idx + 1 < buffer.size() && buffer[idx] == '/' &&
          buffer[idx + 1] == '/') {
        while (idx < buffer.size() && buffer[idx] != '\n') {
          ++idx;
        }
        if (includeComments) {
          result.tokens.push_back(
              {TokenKind::Comment, buffer.substr(start, idx - start)});
        }
      } else {
      bool found = false;
      for (char c : charTokens) {
        if (buffer[idx] == c) {
          found = true;
          result.tokens.push_back(
              {static_cast<TokenKind>(c), buffer.substr(idx++, 1)});
          break;
        }
      }
      if (!found) {
        result.errors.push_back(
            {LexErrorKind::InvalidCharacter, buffer.substr(idx++, 1)});
      }
    }
  }
  return result;
}

} // namespace lang
