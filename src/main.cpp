#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

#include "Support/Reporting.h"
#include "Support/SourceFile.h"

#include "Frontend/ASTPrinter.h"
#include "Frontend/Lexer.h"
#include "Frontend/Parser.h"
#include "Frontend/Resolver.h"

namespace {

enum class CompilerAction {
  None,
  EmitLex,
  EmitAst,
};

const llvm::cl::opt<std::string>
    inputFilename(llvm::cl::Positional, llvm::cl::desc("<file>"),
                  llvm::cl::Required, llvm::cl::value_desc("filename"));

const llvm::cl::opt<CompilerAction> compilerAction(
    "emit", llvm::cl::desc("Select the kind of output desired"),
    llvm::cl::values(
        clEnumValN(CompilerAction::EmitLex, "lex",
                   "Dump the lexed tokens of the input file"),
        clEnumValN(CompilerAction::EmitAst, "ast",
                   "Dump the abstract syntax tree of the input file")),
    llvm::cl::init(CompilerAction::None));

template <typename T>
void reportErrors(
    const lang::SourceFile &source, const std::vector<T> &errors,
    const std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
  std::size_t numErrors = 0;
  for (const auto &error : errors) {
    lang::reportError(source, error.toPretty());
    if (++numErrors >= maxErrors) {
      break;
    }
  }
}

} // namespace

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "Lang Compiler\n", nullptr,
                                    nullptr, true);

  const auto file = llvm::MemoryBuffer::getFile(inputFilename);

  if (!file) {
    llvm::errs() << "Error: " << file.getError().message() << "\n";
    return EXIT_FAILURE;
  }

  const lang::SourceFile source(inputFilename, file.get()->getBuffer());

  lang::Lexer lexer(file.get()->getBuffer());
  const auto lexResult = lexer.lexAll();

  if (lexResult.tokens.empty()) {
    llvm::errs() << "Error: empty file\n";
    return EXIT_FAILURE;
  }

  if (!lexResult.errors.empty()) {
    reportErrors(source, lexResult.errors, 1);
    return EXIT_FAILURE;
  }

  if (compilerAction == CompilerAction::EmitLex) {
    llvm::outs() << "== Tokens ==\n";
    for (const auto &token : lexResult.tokens) {
      llvm::outs() << token.toString() << "\n";
    }
    return EXIT_SUCCESS;
  }

  lang::Arena arena(KB(64));
  lang::TypeContext tcx(arena);

  lang::Parser parser(tcx, arena, lexResult.tokens);
  const auto parseResult = parser.parseModuleAST();

  if (!parseResult.errors.empty()) {
    reportErrors(source, parseResult.errors);
    return EXIT_FAILURE;
  }

  if (compilerAction == CompilerAction::EmitAst) {
    llvm::outs() << "== AST ==\n";
    lang::ASTPrinter printer(llvm::outs());
    printer.visit(*parseResult.node);
  }

  lang::Resolver resolver;
  const auto resolveResult = resolver.resolve(*parseResult.node);

  if (!resolveResult.errors.empty()) {
      reportErrors(source, resolveResult.errors);
      return EXIT_FAILURE;
  }

  if (compilerAction == CompilerAction::EmitAst) {
    llvm::outs() << "== Resolved AST ==\n";
    lang::ASTPrinter printer(llvm::outs());
    printer.visit(*parseResult.node);
  }
}
