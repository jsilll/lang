#ifndef LANG_LEXER_H
#define LANG_LEXER_H

#include "Support/Reporting.h"

#include "Token.h"

#include <vector>

namespace lang {

enum class LexErrorKind {
    InvalidCharacter,
};

struct LexError {
    LexErrorKind kind;
    std::string_view span;

    LexError(LexErrorKind kind, std::string_view span)
        : kind(kind), span(span) {}

    TextError toTextError() const;

    JSONError toJSONError() const;
};

struct LexResult {
    std::vector<Token> tokens;
    std::vector<LexError> errors;

    LexResult() = default;

    [[nodiscard]] bool hasErrors() const { return !errors.empty(); }
};

class Lexer {
  public:
    explicit Lexer(std::string_view buffer) : idx(0), buffer(buffer) {}

    LexResult lexAll(bool includeComments = false);

  private:
    size_t idx;
    std::string_view buffer;

    Token lexAlt(char c, TokenKind altKind, TokenKind defaultKind);
};

} // namespace lang

#endif // LANG_LEXER_H
