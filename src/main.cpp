#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

#include "Base.h"

#include "Reporting.h"
#include "SourceFile.h"

#include "Lexer.h"
#include "Parser.h"

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

} // namespace

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "Lang Compiler\n", nullptr,
                                    nullptr, true);

  auto file = llvm::MemoryBuffer::getFile(inputFilename);

  if (!file) {
    llvm::errs() << "Error: " << file.getError().message() << "\n";
    return EXIT_FAILURE;
  }

  lang::SourceFile source(inputFilename, file.get()->getBuffer());

  lang::Lexer lexer(file.get()->getBuffer());
  auto lexResult = lexer.lexAll();

  if (!lexResult.errors.empty()) {
    lang::Error error{lexResult.errors.front().span, "Invalid character",
                      "Invalid character"};
    lang::reportError(source, error);
    return EXIT_FAILURE;
  }

  if (compilerAction == CompilerAction::EmitLex) {
    for (const auto &token : lexResult.tokens) {
      llvm::outs() << token.toString() << "\n";
    }
    return EXIT_SUCCESS;
  }

  lang::Arena arena(KB(64));

  lang::Parser parser(arena, lexResult.tokens);
  auto *module = parser.parseModule();

  DEBUG("Arena size: %zu", arena.totalAllocated());
}