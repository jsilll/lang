#include "Frontend/Token.h"

#include <sstream>

namespace lang {

std::string tokenKindToString(TokenKind kind) {
    const int kindInt = static_cast<int>(kind);
    if (kindInt >= static_cast<int>(TokenKind::Bang) &&
        kindInt <= static_cast<int>(TokenKind::RBrace)) {
        return "'" + std::string(1, static_cast<char>(kindInt)) + "'";
    }

    switch (kind) {
    case TokenKind::BangEqual:
        return "'!='";
    case TokenKind::AmpAmp:
        return "'&&'";
    case TokenKind::LessEqual:
        return "'<='";
    case TokenKind::EqualEqual:
        return "'=='";
    case TokenKind::GreaterEqual:
        return "'>='";
    case TokenKind::PipePipe:
        return "||";
    case TokenKind::KwFn:
        return "'fn'";
    case TokenKind::KwVoid:
        return "'void'";
    case TokenKind::KwNumber:
        return "'number'";
    case TokenKind::KwLet:
        return "'let'";
    case TokenKind::KwVar:
        return "'var'";
    case TokenKind::KwIf:
        return "'if'";
    case TokenKind::KwElse:
        return "'else'";
    case TokenKind::KwWhile:
        return "'while'";
    case TokenKind::KwReturn:
        return "'return'";
    case TokenKind::Ident:
        return "an identifier";
    case TokenKind::Number:
        return "a number";
    case TokenKind::Comment:
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
