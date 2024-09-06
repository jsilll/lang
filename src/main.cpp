#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

#include "Support/Debug.h"
#include "Support/Reporting.h"
#include "Support/SourceFile.h"

#include "AST/ASTPrinter.h"

#include "Lex/Lexer.h"

#include "Parse/Parser.h"

#include "Sema/Resolver.h"

namespace {

enum class CompilerUntilStage {
    None,
    UntilLex,
    UntilAST,
};

enum class CompilerEmitAction {
    None,
    EmitLex,
    EmitSrc,
    EmitAST,
};

const llvm::cl::opt<std::string>
    inputFilename(llvm::cl::Positional, llvm::cl::desc("<file>"),
                  llvm::cl::Required, llvm::cl::value_desc("filename"));

const llvm::cl::opt<CompilerUntilStage> compilerUntilStage(
    "until", llvm::cl::desc("Select the stage until which the compiler run"),
    llvm::cl::values(clEnumValN(CompilerUntilStage::UntilLex, "lex",
                                "Run the compiler until the lexing stage"),
                     clEnumValN(CompilerUntilStage::UntilAST, "ast",
                                "Run the compiler until the parsing stage")),
    llvm::cl::init(CompilerUntilStage::None));

const llvm::cl::opt<CompilerEmitAction> compilerEmitAction(
    "emit", llvm::cl::desc("Select the kind of output desired"),
    llvm::cl::values(
        clEnumValN(CompilerEmitAction::EmitLex, "lex",
                   "Dump the lexed tokens of the input file"),
        clEnumValN(CompilerEmitAction::EmitSrc, "src",
                   "Dump the original source code of the input file"),
        clEnumValN(CompilerEmitAction::EmitAST, "ast",
                   "Dump the abstract syntax tree of the input file")),
    llvm::cl::init(CompilerEmitAction::None));

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

    if (compilerEmitAction == CompilerEmitAction::EmitLex) {
        for (const auto &token : lexResult.tokens) {
            llvm::outs() << token.toString() << "\n";
        }
    }

    if (lexResult.tokens.empty()) {
        llvm::errs() << "Error: empty file\n";
        return EXIT_FAILURE;
    }

    if (!lexResult.errors.empty()) {
        reportErrors(source, lexResult.errors, 1);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::UntilLex) {
        return EXIT_SUCCESS;
    }

    lang::Arena arena(lang::kiloBytes(32));
    lang::TypeContext tcx(arena);

    lang::ASTPrinter astPrinter(llvm::outs());
    lang::Parser parser(tcx, arena, lexResult.tokens);

    const auto parseResult = parser.parseModuleAST();
    DEBUG("%lu allocations in %lu bytes", arena.totalAllocations(),
          arena.totalAllocated());

    if (compilerEmitAction == CompilerEmitAction::EmitSrc) {
        // TODO: Implement
    }

    if (compilerEmitAction == CompilerEmitAction::EmitAST) {
        astPrinter.visit(*parseResult.module);
    }

    if (!parseResult.errors.empty()) {
        reportErrors(source, parseResult.errors);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::UntilAST) {
        return EXIT_SUCCESS;
    }

    lang::Resolver resolver;
    const auto resolveResult = resolver.resolveModuleAST(*parseResult.module);

    if (compilerEmitAction == CompilerEmitAction::EmitAST) {
        astPrinter.visit(*parseResult.module);
    }

    if (!resolveResult.errors.empty()) {
        reportErrors(source, resolveResult.errors);
        return EXIT_FAILURE;
    }
}
