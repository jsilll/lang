#ifndef LANG_TOKEN_H
#define LANG_TOKEN_H

#include <string_view>

namespace lang {

enum class TokenKind : int {
    Bang = '!',
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
    Pipe = '|',
    RBrace = '}',

    BangEqual = -1,
    AmpAmp = -2,
    LessEqual = -3,
    EqualEqual = -4,
    GreaterEqual = -5,
    PipePipe = -6,

    KwFn = -7,
    KwVoid = -8,
    KwNumber = -9,
    KwLet = -10,
    KwVar = -11,
    KwIf = -12,
    KwElse = -13,
    KwWhile = -14,
    KwBreak = -15,
    KwReturn = -16,

    Ident = -17,
    Number = -18,

    Comment = -19,
};

std::string tokenKindToString(TokenKind kind);

struct Token {
    TokenKind kind;
    std::string_view span;
    std::string toString() const;
};

} // namespace lang

#endif // LANG_TOKEN_H
