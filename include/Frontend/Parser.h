#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include "Alloc/Arena.h"

#include "Support/Reporting.h"

#include "AST.h"
#include "Token.h"

#include <unordered_set>
#include <vector>

namespace lang {

enum class ParseErrorKind {
  UnexpectedEOF,
  UnexpectedToken,
};

struct ParseError {
  ParseErrorKind kind;
  std::string_view span;
  TokenKind expected;
  PrettyError toPretty() const;
};

template <typename T> struct ParseResult {
  T *node;
  std::vector<ParseError> errors;
};

class Parser {
public:
  Parser(TypeContext &tcx, Arena &arena, const std::vector<Token> &tokens)
      : tcx(&tcx), arena(&arena), cur(tokens.begin()), end(tokens.end()) {}

  ParseResult<ModuleAST> parseModuleAST();

private:
  const Token *peek();
  const Token *next();
  const Token *expect(TokenKind kind);

  void sync(TokenKind syncToken);
  void sync(const std::unordered_set<TokenKind> &syncSet);

  FunctionDeclAST *parseFunctionDeclAST();

  BlockStmtAST *parseBlockStmtAST();

  LocalStmtAST *parseLocalStmtAST();

  ExprStmtAST *parseExprStmtAST();

  ExprAST *parseExprAST(int prec = 0);

  ExprAST *parsePrimaryExprAST();

  TypeContext *tcx;
  Arena *arena;
  std::vector<Token>::const_iterator cur;
  std::vector<Token>::const_iterator end;
  std::vector<ParseError> errors;
};

} // namespace lang

#endif // LANG_PARSER_H
