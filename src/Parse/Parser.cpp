#include "Parse/Parser.h"

#include <unordered_map>

namespace {

struct BinOpPair {
    lang::BinOpKind bind;
    int precLHS;
    int precRHS;
};

const std::unordered_set<lang::TokenKind> declLevelSyncSet = {
    lang::TokenKind::KwFn,
};

const std::unordered_set<lang::TokenKind> stmtLevelSyncSet = {
    lang::TokenKind::Semicolon,
    lang::TokenKind::RBrace,
};

const std::unordered_map<lang::TokenKind, BinOpPair> binOpMap = {
    // Multiplication and division (highest precedence)
    {lang::TokenKind::Star, {lang::BinOpKind::Mul, 6, 6}},
    {lang::TokenKind::Slash, {lang::BinOpKind::Div, 6, 6}},

    // Addition and subtraction
    {lang::TokenKind::Plus, {lang::BinOpKind::Add, 5, 5}},
    {lang::TokenKind::Minus, {lang::BinOpKind::Sub, 5, 5}},

    // Relational operators (higher precedence than &&)
    {lang::TokenKind::Less, {lang::BinOpKind::Lt, 4, 4}},
    {lang::TokenKind::LessEqual, {lang::BinOpKind::Le, 4, 4}},
    {lang::TokenKind::Greater, {lang::BinOpKind::Gt, 4, 4}},
    {lang::TokenKind::GreaterEqual, {lang::BinOpKind::Ge, 4, 4}},

    // Logical AND (higher precedence than equality operators, but lower than
    // relational operators)
    {lang::TokenKind::AmpAmp, {lang::BinOpKind::And, 3, 3}},

    // Equality operators (higher precedence than ||)
    {lang::TokenKind::EqualEqual, {lang::BinOpKind::Eq, 2, 2}},
    {lang::TokenKind::BangEqual, {lang::BinOpKind::Ne, 2, 2}},

    // Logical OR (lowest precedence)
    {lang::TokenKind::PipePipe, {lang::BinOpKind::Or, 1, 1}},
};

} // namespace

#define EXPECT(kind)                                                           \
    if (expect(kind) == nullptr) {                                             \
        return nullptr;                                                        \
    }

#define BREAK_IF_NULL(expr)                                                    \
    if ((expr) == nullptr) {                                                   \
        break;                                                                 \
    }

#define RETURN_IF_NULL(expr)                                                   \
    if ((expr) == nullptr) {                                                   \
        return nullptr;                                                        \
    }

namespace lang {

TextError ParseError::toTextError() const {
    switch (kind) {
    case ParseErrorKind::UnexpectedEOF:
        return {span, "Unexpected end of file", "Unexpected end of file"};
    case ParseErrorKind::UnexpectedToken:
        return {span, "Unexpected token",
                "Expected " + tokenKindToString(expected) + " instead"};
    case ParseErrorKind::ExpectedType:
        return {span, "Unexpected token", "Expected a type instead"};
    case ParseErrorKind::ExpectedPrimaryExpression:
        return {span, "Unexpected token",
                "Expected a primary expression instead"};
    }
    return {span, "Unknown parsing error title", "Unknown parse error label"};
}

JSONError ParseError::toJSONError() const {
    switch (kind) {
    case ParseErrorKind::UnexpectedEOF:
        return {span, "parse-unexpected-eof"};
    case ParseErrorKind::UnexpectedToken:
        return {span, "parse-unexpeected-token"};
    case ParseErrorKind::ExpectedType:
        return {span, "parse-expected-type"};
    case ParseErrorKind::ExpectedPrimaryExpression:
        return {span, "parse-expected-primary-expr"};
    }
    return {span, "parser-unknown-error"};
}

const Token *Parser::peek() {
    if (cur == end) {
        return nullptr;
    }
    return &*cur;
}

const Token *Parser::next() {
    if (cur == end) {
        return nullptr;
    }
    return &*cur++;
}

const Token *Parser::expect(TokenKind kind) {
    const Token *tok = peek();
    if (!tok) {
        errors.emplace_back(ParseErrorKind::UnexpectedEOF, std::prev(cur)->span,
                            kind);
        return nullptr;
    }

    ++cur;

    if (tok->kind != kind) {
        errors.emplace_back(ParseErrorKind::UnexpectedToken, tok->span, kind);
        return nullptr;
    }

    return tok;
}

void Parser::sync(const std::unordered_set<TokenKind> &syncSet) {
    cur = std::find_if(cur, end, [&syncSet](const Token &token) {
        return syncSet.find(token.kind) != syncSet.end();
    });

    // Return if we are at end or end - 1
    if (cur == end || cur + 1 == end) {
        return;
    }

    ++cur;
    cur = std::find_if(cur, end, [&syncSet](const Token &token) {
        return syncSet.find(token.kind) != syncSet.end();
    });
}

ParseResult Parser::parseModuleAST() {
    NonOwningList<DeclAST *> decls;
    DeclAST *decl = nullptr;

    const Token *tok = peek();
    while (tok != nullptr) {
        switch (tok->kind) {
        case TokenKind::KwFn:
            decl = parseFunctionDeclAST();

            if (decl != nullptr) {
                decls.emplace_back(arena, decl);
            } else {
                sync(declLevelSyncSet);
            }

            break;

        default:
            ++cur;
            errors.emplace_back(ParseErrorKind::UnexpectedToken, tok->span,
                                TokenKind::KwFn);
        }

        tok = peek();
    }

    return {arena->alloc<ModuleAST>("main", decls), std::move(errors)};
}

FunctionDeclAST *Parser::parseFunctionDeclAST() {
    EXPECT(TokenKind::KwFn);

    const Token *ident = expect(TokenKind::Ident);
    RETURN_IF_NULL(ident);

    EXPECT(TokenKind::LParen);

    NonOwningList<LocalStmtAST *> params;
    LocalStmtAST *param = nullptr;

    const Token *tok = peek();
    while (tok != nullptr && tok->kind != TokenKind::RParen) {
        param = parseLocalStmtAST(true);
        RETURN_IF_NULL(param);

        params.emplace_back(arena, param);

        tok = peek();
        RETURN_IF_NULL(tok);

        if (tok == nullptr || tok->kind != TokenKind::Comma) {
            break;
        }

        ++cur;
        tok = peek();
    }

    EXPECT(TokenKind::RParen);

    EXPECT(TokenKind::Colon);
    Type *type = parseTypeAnnotation();
    RETURN_IF_NULL(type);

    BlockStmtAST *body = parseBlockStmtAST();
    RETURN_IF_NULL(body);

    return arena->alloc<FunctionDeclAST>(ident->span, params, type, body);
}

Type *Parser::parseTypeAnnotation() {
    const Token *tok = next();
    RETURN_IF_NULL(tok);

    switch (tok->kind) {
    case TokenKind::KwVoid:
        return typeCtx->getTypeVoid();
    case TokenKind::KwNumber:
        return typeCtx->getTypeNumber();
    default:
        errors.emplace_back(ParseErrorKind::ExpectedType, tok->span,
                            TokenKind::Amp);
    }

    return nullptr;
}

BlockStmtAST *Parser::parseBlockStmtAST() {
    const Token *lBrace = expect(TokenKind::LBrace);
    RETURN_IF_NULL(lBrace);

    NonOwningList<StmtAST *> stmts;
    StmtAST *stmt = nullptr;

    const Token *tok = peek();
    while (tok != nullptr && tok->kind != TokenKind::RBrace) {
        switch (tok->kind) {
        case TokenKind::LBrace:
            stmt = parseBlockStmtAST();
            break;

        case TokenKind::KwBreak:
            ++cur;
            stmt = arena->alloc<BreakStmtAST>(tok->span);
            EXPECT(TokenKind::Semicolon);
            break;

        case TokenKind::KwReturn:
            ++cur;
            stmt = arena->alloc<ReturnStmtAST>(
                tok->span, cur != end && cur->kind != TokenKind::Semicolon
                               ? parseExprAST()
                               : nullptr);
            EXPECT(TokenKind::Semicolon);
            break;

        case TokenKind::KwLet:
            ++cur;

            stmt = parseLocalStmtAST(true);
            BREAK_IF_NULL(stmt);

            EXPECT(TokenKind::Semicolon);
            break;

        case TokenKind::KwVar:
            ++cur;

            stmt = parseLocalStmtAST(false);
            BREAK_IF_NULL(stmt);

            EXPECT(TokenKind::Semicolon);
            break;

        case TokenKind::KwIf:
            stmt = parseIfStmtAST();
            break;

        case TokenKind::KwWhile:
            stmt = parseWhileStmtAST();
            break;

        default:
            stmt = parseExprStmtOrAssignStmtAST();
        }

        if (stmt != nullptr) {
            stmts.emplace_back(arena, stmt);
            stmt = nullptr;
        } else {
            sync(stmtLevelSyncSet);
            next();
        }

        tok = peek();
    }

    EXPECT(TokenKind::RBrace);

    return arena->alloc<BlockStmtAST>(lBrace->span, stmts);
}

LocalStmtAST *Parser::parseLocalStmtAST(bool isConst) {
    const Token *ident = expect(TokenKind::Ident);
    RETURN_IF_NULL(ident);

    Type *type = nullptr;

    const Token *tok = peek();
    RETURN_IF_NULL(tok);

    if (tok->kind == TokenKind::Colon) {
        ++cur;

        type = parseTypeAnnotation();
        RETURN_IF_NULL(type);
    }

    ExprAST *init = nullptr;

    tok = peek();
    RETURN_IF_NULL(tok);

    if (tok->kind == TokenKind::Equal) {
        ++cur;

        init = parseExprAST();
        RETURN_IF_NULL(init);
    }

    return arena->alloc<LocalStmtAST>(isConst, ident->span, type, init);
}

IfStmtAST *Parser::parseIfStmtAST() {
    const Token *tok = expect(TokenKind::KwIf);
    RETURN_IF_NULL(tok);

    ExprAST *cond = parseExprAST();
    RETURN_IF_NULL(cond);

    BlockStmtAST *thenBranch = parseBlockStmtAST();
    RETURN_IF_NULL(thenBranch);

    StmtAST *elseBranch = nullptr;

    const Token *elseToken = peek();
    if (elseToken != nullptr && elseToken->kind == TokenKind::KwElse) {
        ++cur;

        const Token *ifToken = peek();
        if (ifToken != nullptr && ifToken->kind == TokenKind::KwIf) {
            elseBranch = parseIfStmtAST();
        } else {
            elseBranch = parseBlockStmtAST();
        }

        RETURN_IF_NULL(elseBranch);
    }

    return arena->alloc<IfStmtAST>(tok->span, cond, thenBranch, elseBranch);
}

WhileStmtAST *Parser::parseWhileStmtAST() {
    const Token *tok = expect(TokenKind::KwWhile);
    RETURN_IF_NULL(tok);

    ExprAST *cond = parseExprAST();
    RETURN_IF_NULL(cond);

    BlockStmtAST *body = parseBlockStmtAST();
    RETURN_IF_NULL(body);

    return arena->alloc<WhileStmtAST>(tok->span, cond, body);
}

StmtAST *Parser::parseExprStmtOrAssignStmtAST() {
    ExprAST *lhs = parseExprAST();
    RETURN_IF_NULL(lhs);

    const Token *tok = next();
    RETURN_IF_NULL(tok);

    switch (tok->kind) {
    case TokenKind::Semicolon:
        return arena->alloc<ExprStmtAST>(tok->span, lhs);

    case TokenKind::Equal: {
        ExprAST *rhs = parseExprAST();
        RETURN_IF_NULL(rhs);

        EXPECT(TokenKind::Semicolon);
        return arena->alloc<AssignStmtAST>(tok->span, lhs, rhs);

    } break;
    default:
        errors.emplace_back(ParseErrorKind::ExpectedPrimaryExpression,
                            tok->span, TokenKind::Amp);
    }

    return nullptr;
}

ExprAST *Parser::parseExprAST(int prec) {
    ExprAST *lhs = parsePrimaryExprAST();
    RETURN_IF_NULL(lhs);

    const Token *tok = peek();
    while (tok != nullptr) {
        switch (tok->kind) {
        case TokenKind::LParen: {
            ++cur;

            ExprAST *arg = parseExprAST();
            RETURN_IF_NULL(arg);

            lhs = arena->alloc<CallExprAST>(tok->span, lhs, arg);

            EXPECT(TokenKind::RParen);
        } break;

        case TokenKind::LBracket: {
            ++cur;

            ExprAST *index = parseExprAST();
            RETURN_IF_NULL(index);

            lhs = arena->alloc<IndexExprAST>(tok->span, lhs, index);

            EXPECT(TokenKind::RBracket);
        } break;

        default:
            const auto it = binOpMap.find(tok->kind);
            if (it == binOpMap.end()) {
                return lhs;
            }
            if (it->second.precLHS <= prec) {
                return lhs;
            }
            ++cur;
            lhs = arena->alloc<BinaryExprAST>(tok->span, it->second.bind, lhs,
                                              parseExprAST(it->second.precRHS));
        }

        tok = peek();
    }

    return lhs;
}

ExprAST *Parser::parsePrimaryExprAST() {
    ExprAST *expr = nullptr;
    const Token *tok = next();

    switch (tok->kind) {
    case TokenKind::Number:
        return arena->alloc<NumberExprAST>(tok->span);

    case TokenKind::Ident:
        return arena->alloc<IdentifierExprAST>(tok->span);

    case TokenKind::Minus:
        expr = parsePrimaryExprAST();
        RETURN_IF_NULL(expr);

        return arena->alloc<UnaryExprAST>(tok->span, UnOpKind::Neg, expr);

    case TokenKind::LParen:
        expr = parseExprAST();
        RETURN_IF_NULL(expr);

        EXPECT(TokenKind::RParen);

        return arena->alloc<GroupedExprAST>(tok->span, expr);

    default:
        errors.emplace_back(ParseErrorKind::ExpectedPrimaryExpression,
                            tok->span, TokenKind::Amp);
    }

    return nullptr;
}

} // namespace lang
