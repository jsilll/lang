#include "Frontend/Parser.h"

#include "Frontend/AST.h"
#include "Frontend/Token.h"

#include <unordered_map>

namespace {

struct BinOpPair {
  lang::BinOpKind bind;
  int precLhs;
  int precRhs;
};

const std::unordered_set<lang::TokenKind> declLevelSyncSet = {
    lang::TokenKind::KwFn,
};

const std::unordered_set<lang::TokenKind> stmtLevelSyncSet = {
    lang::TokenKind::Semicolon,
    lang::TokenKind::RBrace,
};

const std::unordered_map<lang::TokenKind, BinOpPair> binOpMap = {
    {lang::TokenKind::Plus, {lang::BinOpKind::Add, 1, 1}},
    {lang::TokenKind::Minus, {lang::BinOpKind::Sub, 1, 1}},
    {lang::TokenKind::Star, {lang::BinOpKind::Mul, 2, 2}},
    {lang::TokenKind::Slash, {lang::BinOpKind::Div, 2, 2}},
};

} // namespace

#define EXPECT(kind)                                                           \
  if (expect(kind) == nullptr) {                                               \
    return nullptr;                                                            \
  }

#define BREAK_IF_NULL(expr)                                                    \
  if ((expr) == nullptr) {                                                     \
    break;                                                                     \
  }

#define RETURN_IF_NULL(expr)                                                   \
  if ((expr) == nullptr) {                                                     \
    return nullptr;                                                            \
  }

namespace lang {

PrettyError ParseError::toPretty() const {
  switch (kind) {
  case ParseErrorKind::UnexpectedEOF:
    return {span, "Unexpected end of file", "Unexpected end of file"};
  case ParseErrorKind::UnexpectedToken:
    return {span, "Unexpected token",
            "Expected " + tokenKindToString(expected) + " instead"};
  case ParseErrorKind::ExpectedTypeAnnotation:
    return {span, "Unexpected token", "Expected a type annotation instead"};
  case ParseErrorKind::ExpectedPrimaryExpression:
    return {span, "Unexpected token", "Expected a primary expression instead"};
  }
  return {span, "Unknown parse error title", "Unknown parse error label"};
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
    errors.push_back({
        ParseErrorKind::UnexpectedEOF,
        std::prev(cur)->span,
        kind,
    });
    return nullptr;
  }

  ++cur;

  if (tok->kind != kind) {
    errors.push_back({ParseErrorKind::UnexpectedToken, tok->span, kind});
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
  List<DeclAST *> decls;
  DeclAST *decl = nullptr;

  const Token *tok = peek();
  while (tok != nullptr) {
    switch (tok->kind) {
    case TokenKind::KwFn: {
      decl = parseFunctionDeclAST();
      if (decl != nullptr) {
        decls.emplace_back(arena, decl);
      } else {
        sync(declLevelSyncSet);
      }
    } break;
    default:
      errors.push_back(
          {ParseErrorKind::UnexpectedToken, tok->span, TokenKind::KwFn});
      ++cur;
      break;
    }
    tok = peek();
  }

  return {arena->make<ModuleAST>("main", decls), std::move(errors)};
}

FunctionDeclAST *Parser::parseFunctionDeclAST() {
  EXPECT(TokenKind::KwFn);

  const Token *ident = expect(TokenKind::Ident);
  RETURN_IF_NULL(ident);

  EXPECT(TokenKind::LParen);

  List<LocalStmtAST *> params;
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

  return arena->make<FunctionDeclAST>(ident->span, params, type, body);
}

Type *Parser::parseTypeAnnotation() {
  const Token *tok = next();
  RETURN_IF_NULL(tok);

  switch (tok->kind) {
  case TokenKind::KwVoid:
    return tcx->getTyVoid();
  case TokenKind::KwNumber:
    return tcx->getTyNumber();
  default:
    errors.push_back(
        {ParseErrorKind::ExpectedTypeAnnotation, tok->span, TokenKind::Amp});
    return nullptr;
  }
}

BlockStmtAST *Parser::parseBlockStmtAST() {
  const Token *lBrace = expect(TokenKind::LBrace);
  RETURN_IF_NULL(lBrace);

  List<StmtAST *> body;
  StmtAST *stmt = nullptr;

  const Token *tok = peek();
  while (tok != nullptr && tok->kind != TokenKind::RBrace) {
    switch (tok->kind) {
    case TokenKind::KwLet:
    case TokenKind::KwVar:
      ++cur;
      stmt = parseLocalStmtAST(tok->kind == TokenKind::KwLet);
      BREAK_IF_NULL(stmt);
      EXPECT(TokenKind::Semicolon);
      break;
    case TokenKind::KwReturn:
      ++cur;
      stmt = arena->make<ReturnStmtAST>(
          tok->span, cur != end && cur->kind != TokenKind::Semicolon
                         ? parseExprAST()
                         : nullptr);
      EXPECT(TokenKind::Semicolon);
      break;
    case TokenKind::LBrace:
      stmt = parseBlockStmtAST();
      BREAK_IF_NULL(stmt);
      break;
    default:
      stmt = parseExprStmtAST();
      break;
    }
    if (stmt != nullptr) {
      body.emplace_back(arena, stmt);
    } else {
      sync(stmtLevelSyncSet);
      next();
    }
    tok = peek();
  }

  EXPECT(TokenKind::RBrace);

  return arena->make<BlockStmtAST>(lBrace->span, body);
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

  ExprAST *expr = nullptr;

  tok = peek();
  RETURN_IF_NULL(tok);

  if (tok->kind == TokenKind::Equal) {
    ++cur;

    expr = parseExprAST();
    RETURN_IF_NULL(expr);
  }

  return arena->make<LocalStmtAST>(isConst, ident->span, type, expr);
}

ExprStmtAST *Parser::parseExprStmtAST() {
  ExprAST *expr = parseExprAST();
  RETURN_IF_NULL(expr);

  const Token *tok = expect(TokenKind::Semicolon);
  RETURN_IF_NULL(tok);

  return arena->make<ExprStmtAST>(tok->span, expr);
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

      lhs = arena->make<CallExprAST>(tok->span, lhs, arg);

      EXPECT(TokenKind::RParen);
    } break;
    case TokenKind::LBracket: {
      ++cur;

      ExprAST *index = parseExprAST();
      RETURN_IF_NULL(index);

      lhs = arena->make<IndexExprAST>(tok->span, lhs, index);

      EXPECT(TokenKind::RBracket);
    } break;
    default:
      const auto it = binOpMap.find(tok->kind);
      if (it == binOpMap.end()) {
        return lhs;
      }
      if (it->second.precLhs <= prec) {
        return lhs;
      }
      ++cur;
      lhs = arena->make<BinaryExprAST>(tok->span, it->second.bind, lhs,
                                       parseExprAST(it->second.precRhs));
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
    return arena->make<NumberExprAST>(tok->span);
  case TokenKind::Ident:
    return arena->make<IdentifierExprAST>(tok->span);
  case TokenKind::Minus:
    expr = parsePrimaryExprAST();
    RETURN_IF_NULL(expr);
    return arena->make<UnaryExprAST>(tok->span, UnOpKind::Neg, expr);
  case TokenKind::LParen:
    expr = parseExprAST();
    RETURN_IF_NULL(expr);
    EXPECT(TokenKind::RParen);
    return arena->make<GroupedExprAST>(tok->span, expr);
  default:
    break;
  }

  errors.push_back(
      {ParseErrorKind::ExpectedPrimaryExpression, tok->span, TokenKind::Amp});

  return nullptr;
}

} // namespace lang
