#include "Lex/Token.h"

#include <sstream>
#include <unordered_map>

namespace {

const std::unordered_map<lang::TokenKind, std::string_view> tokenKindMap = {
    {lang::TokenKind::BangEqual, "'!='"},
    {lang::TokenKind::AmpAmp, "'&&'"},
    {lang::TokenKind::LessEqual, "'<='"},
    {lang::TokenKind::EqualEqual, "'=='"},
    {lang::TokenKind::GreaterEqual, "'>='"},
    {lang::TokenKind::PipePipe, "||"},
    {lang::TokenKind::KwFn, "'fn'"},
    {lang::TokenKind::KwVoid, "'void'"},
    {lang::TokenKind::KwNumber, "'number'"},
    {lang::TokenKind::KwLet, "'let'"},
    {lang::TokenKind::KwVar, "'var'"},
    {lang::TokenKind::KwIf, "'if'"},
    {lang::TokenKind::KwElse, "'else'"},
    {lang::TokenKind::KwWhile, "'while'"},
    {lang::TokenKind::KwReturn, "'return'"},
    {lang::TokenKind::Ident, "an identifier"},
    {lang::TokenKind::Number, "a number"},
    {lang::TokenKind::Comment, "a comment"},
};

} // namespace

namespace lang {

std::string tokenKindToString(TokenKind kind) {
    const int kindInt = static_cast<int>(kind);
    if (kindInt >= static_cast<int>(TokenKind::Bang) &&
        kindInt <= static_cast<int>(TokenKind::RBrace)) {
        return "'" + std::string(1, static_cast<char>(kindInt)) + "'";
    }

    const auto it = tokenKindMap.find(kind);
    if (it == tokenKindMap.end()) {
        return "an unknown token";
    }

    return std::string(it->second);
}

std::string Token::toString() const {
    std::ostringstream oss;

    const auto kindInt = static_cast<int>(this->kind);
    if (kindInt >= static_cast<int>(TokenKind::KwReturn) &&
        kindInt <= static_cast<int>(TokenKind::KwFn)) {
        oss << "Keyword(";
    } else {
        switch (kind) {
        case TokenKind::Number:
            oss << "Number(";
            break;
        case TokenKind::Ident:
            oss << "Identifier(";
            break;
        case TokenKind::Comment:
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
