#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include "Alloc/Arena.h"

#include "Sema/Type.h"
#include "Support/Reporting.h"

#include "AST/AST.h"

#include "Lex/Token.h"

#include <unordered_set>

namespace lang {

enum class ParseErrorKind {
    UnexpectedEOF,
    UnexpectedToken,
    ExpectedTypeAnnotation,
    ExpectedPrimaryExpression,
};

struct ParseError {
    ParseErrorKind kind;
    std::string_view span;
    TokenKind expected;
    ParseError(ParseErrorKind kind, std::string_view span, TokenKind expected)
        : kind(kind), span(span), expected(expected) {}
    PrettyError toPretty() const;
};

struct ParseResult {
    ModuleAST *module;
    std::vector<ParseError> errors;
};

class Parser {
  public:
    Parser(Arena &arena, TypeContext &typeCtx, const std::vector<Token> &tokens)
        : arena(&arena), typeCtx(&typeCtx), cur(tokens.begin()), end(tokens.end()) {}

    ParseResult parseModuleAST();

  private:
    Arena *arena;
    TypeContext *typeCtx;
    std::vector<Token>::const_iterator cur;
    std::vector<Token>::const_iterator end;
    std::vector<ParseError> errors;

    const Token *peek();
    const Token *next();
    const Token *expect(TokenKind kind);
    void sync(const std::unordered_set<TokenKind> &syncSet);

    FunctionDeclAST *parseFunctionDeclAST();

    Type *parseTypeAnnotation();

    BlockStmtAST *parseBlockStmtAST();

    LocalStmtAST *parseLocalStmtAST(bool isConst);

    IfStmtAST *parseIfStmtAST();

    WhileStmtAST *parseWhileStmtAST();

    StmtAST *parseExprStmtOrAssignStmtAST();

    ExprAST *parseExprAST(int prec = 0);

    ExprAST *parsePrimaryExprAST();
};

} // namespace lang

#endif // LANG_PARSER_H
