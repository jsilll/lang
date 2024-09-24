#include "Lex/Lexer.h"

#include <unordered_map>

namespace {

bool isWhite(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isAlnum(char c) { return isAlpha(c) || isDigit(c); }

const std::unordered_map<std::string_view, lang::TokenKind> keywordToTokenKind =
    {{"fn", lang::TokenKind::KwFn},
     {"void", lang::TokenKind::KwVoid},
     {"number", lang::TokenKind::KwNumber},
     {"let", lang::TokenKind::KwLet},
     {"var", lang::TokenKind::KwVar},
     {"if", lang::TokenKind::KwIf},
     {"else", lang::TokenKind::KwElse},
     {"while", lang::TokenKind::KwWhile},
     {"break", lang::TokenKind::KwBreak},
     {"return", lang::TokenKind::KwReturn}};

} // namespace

namespace lang {

TextError LexError::toTextError() const {
    switch (kind) {
    case LexErrorKind::InvalidCharacter:
        return {span, "Invalid character", "Invalid character"};
    }
    return {span, "Unknown lexing error title", "Unknown lex error label"};
}

JSONError LexError::toJSONError() const {
    switch (kind) {
    case LexErrorKind::InvalidCharacter:
        return {span, "lex-invalid-char"};
    }
    return {span, "lex-unknown-error"};
}

// NOLINTNEXTLINE
Token Lexer::lexAlt(char c, TokenKind altKind, TokenKind defaultKind) {
    if (buffer[idx + 1] == c) {
        idx += 2;
        return {altKind, buffer.substr(idx - 2, 2)};
    }
    ++idx;
    return {defaultKind, buffer.substr(idx - 1, 1)};
}

LexResult Lexer::lexAll(bool includeComments) {
    LexResult result;

    while (idx != buffer.size()) {
        while (isWhite(buffer[idx])) {
            ++idx;
        }
        if (idx == buffer.size()) {
            break;
        }

        const std::size_t start = idx;
        if (isAlpha(buffer[idx])) {
            while (isAlnum(buffer[idx])) { // TokenKind::Ident
                ++idx;
            }
            const std::string_view span = buffer.substr(start, idx - start);
            const auto it = keywordToTokenKind.find(span);
            if (it != keywordToTokenKind.end()) {
                result.tokens.push_back({it->second, span});
            } else {
                result.tokens.push_back({TokenKind::Ident, span});
            }
        } else if (isDigit(buffer[idx])) { // TokenKind::Number
            while (isDigit(buffer[idx])) {
                ++idx;
            }
            if (buffer[idx] == '.') {
                ++idx;
                while (isDigit(buffer[idx])) {
                    ++idx;
                }
                result.tokens.push_back(
                    {TokenKind::Number, buffer.substr(start, idx - start)});
            } else {
                result.tokens.push_back(
                    {TokenKind::Number, buffer.substr(start, idx - start)});
            }
        } else {
            switch (buffer[idx]) {
            case '/': // Comments and TokenKind::Slash
                if (buffer[idx + 1] == '/') {
                    idx += 2;
                    while (idx < buffer.size() && buffer[idx] != '\n') {
                        ++idx;
                    }
                    if (includeComments) {
                        result.tokens.push_back(
                            {TokenKind::Comment,
                             buffer.substr(start, idx - start)});
                    }
                } else {
                    result.tokens.push_back(
                        {TokenKind::Slash, buffer.substr(idx++, 1)});
                }
                break;
            case '(':
                result.tokens.push_back(
                    {TokenKind::LParen, buffer.substr(idx++, 1)});
                break;
            case ')':
                result.tokens.push_back(
                    {TokenKind::RParen, buffer.substr(idx++, 1)});
                break;
            case '*':
                result.tokens.push_back(
                    {TokenKind::Star, buffer.substr(idx++, 1)});
                break;
            case '+':
                result.tokens.push_back(
                    {TokenKind::Plus, buffer.substr(idx++, 1)});
                break;
            case '-':
                result.tokens.push_back(
                    {TokenKind::Minus, buffer.substr(idx++, 1)});
                break;
            case ',':
                result.tokens.push_back(
                    {TokenKind::Comma, buffer.substr(idx++, 1)});
                break;
            case '.':
                result.tokens.push_back(
                    {TokenKind::Dot, buffer.substr(idx++, 1)});
                break;
            case ':':
                result.tokens.push_back(
                    {TokenKind::Colon, buffer.substr(idx++, 1)});
                break;
            case ';':
                result.tokens.push_back(
                    {TokenKind::Semicolon, buffer.substr(idx++, 1)});
                break;
            case '{':
                result.tokens.push_back(
                    {TokenKind::LBrace, buffer.substr(idx++, 1)});
                break;
            case '}':
                result.tokens.push_back(
                    {TokenKind::RBrace, buffer.substr(idx++, 1)});
                break;
            case '=':
                result.tokens.push_back(
                    lexAlt('=', TokenKind::EqualEqual, TokenKind::Equal));
                break;
            case '<':
                result.tokens.push_back(
                    lexAlt('=', TokenKind::LessEqual, TokenKind::Less));
                break;
            case '>':
                result.tokens.push_back(
                    lexAlt('=', TokenKind::GreaterEqual, TokenKind::Greater));
                break;
            case '&':
                result.tokens.push_back(
                    lexAlt('&', TokenKind::AmpAmp, TokenKind::Amp));
                break;
            default:
                result.errors.emplace_back(LexErrorKind::InvalidCharacter,
                                           buffer.substr(idx++, 1));
            }
        }
    }

    return result;
}

} // namespace lang
