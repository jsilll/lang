#include "Lex/Token.h"

#include <sstream>

namespace lang {

std::string tokenKindToString(TokenKind kind) {
    switch (kind) {
    case TokenKind::Bang:
    case TokenKind::Amp:
    case TokenKind::LParen:
    case TokenKind::RParen:
    case TokenKind::Star:
    case TokenKind::Plus:
    case TokenKind::Minus:
    case TokenKind::Comma:
    case TokenKind::Dot:
    case TokenKind::Slash:
    case TokenKind::Colon:
    case TokenKind::Semicolon:
    case TokenKind::Less:
    case TokenKind::Equal:
    case TokenKind::Greater:
    case TokenKind::LBracket:
    case TokenKind::RBracket:
    case TokenKind::LBrace:
    case TokenKind::Pipe:
    case TokenKind::RBrace:
        return "'" + std::string(1, static_cast<char>(kind)) + "'";
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
    case TokenKind::KwBreak:
        return "'break'";
    case TokenKind::KwReturn:
        return "'return'";
    case TokenKind::Ident:
        return "an identifier";
    case TokenKind::Number:
        return "a number";
    case TokenKind::Comment:
        return "a comment";
    }
    return "unknown";
}

std::string Token::toString() const {
    std::ostringstream oss;
    switch (kind) {
    case TokenKind::Bang:
    case TokenKind::Amp:
    case TokenKind::LParen:
    case TokenKind::RParen:
    case TokenKind::Star:
    case TokenKind::Plus:
    case TokenKind::Minus:
    case TokenKind::Comma:
    case TokenKind::Dot:
    case TokenKind::Slash:
    case TokenKind::Colon:
    case TokenKind::Semicolon:
    case TokenKind::Less:
    case TokenKind::Equal:
    case TokenKind::Greater:
    case TokenKind::LBracket:
    case TokenKind::RBracket:
    case TokenKind::LBrace:
    case TokenKind::Pipe:
    case TokenKind::RBrace:
        return std::string(1, static_cast<char>(kind));
    case TokenKind::BangEqual:
    case TokenKind::AmpAmp:
    case TokenKind::LessEqual:
    case TokenKind::EqualEqual:
    case TokenKind::GreaterEqual:
    case TokenKind::PipePipe:
        return std::string(span);
    case TokenKind::KwFn:
    case TokenKind::KwVoid:
    case TokenKind::KwNumber:
    case TokenKind::KwLet:
    case TokenKind::KwVar:
    case TokenKind::KwIf:
    case TokenKind::KwElse:
    case TokenKind::KwWhile:
    case TokenKind::KwBreak:
    case TokenKind::KwReturn:
        oss << "Keyword(";
        break;
    case TokenKind::Number:
        oss << "Number(";
        break;
    case TokenKind::Ident:
        oss << "Identifier(";
        break;
    case TokenKind::Comment:
        oss << "Comment(";
        break;
    }
    oss << span << ")";
    return oss.str();
}

} // namespace lang
