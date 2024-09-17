#include "Support/Debug.h"
#include "Support/Reporting.h"
#include "Support/SourceFile.h"

#include "AST/AST.h"
#include "AST/ASTPrinter.h"

#include "Lex/Lexer.h"

#include "Parse/Parser.h"

#include "Sema/Resolver.h"
#include "Sema/Sema.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"

namespace {

enum class CompilerErrorFormat {
    Text,
    JSON,
};

enum class CompilerUntilStage {
    None,
    Lex,
    AST,
};

enum class CompilerEmitAction {
    None,
    Lex,
    Src,
    AST,
};

const llvm::cl::opt<std::string>
    inputFilename(llvm::cl::Positional, llvm::cl::desc("<file>"),
                  llvm::cl::Required, llvm::cl::value_desc("filename"));

const llvm::cl::opt<CompilerErrorFormat> compilerErrorFormat(
    "error-format", llvm::cl::desc("Select the error format to be emitted"),
    llvm::cl::values(clEnumValN(CompilerErrorFormat::Text, "text",
                                "Use plain text format for errors"),
                     clEnumValN(CompilerErrorFormat::JSON, "json",
                                "Use JSON format for errors")),
    llvm::cl::init(CompilerErrorFormat::Text));

const llvm::cl::opt<CompilerUntilStage> compilerUntilStage(
    "until", llvm::cl::desc("Select the stage until which the compiler run"),
    llvm::cl::values(clEnumValN(CompilerUntilStage::Lex, "lex",
                                "Run the compiler until the lexing stage"),
                     clEnumValN(CompilerUntilStage::AST, "ast",
                                "Run the compiler until the parsing stage")),
    llvm::cl::init(CompilerUntilStage::None));

const llvm::cl::opt<CompilerEmitAction> compilerEmitAction(
    "emit", llvm::cl::desc("Select the kind of output desired"),
    llvm::cl::values(
        clEnumValN(CompilerEmitAction::Lex, "lex",
                   "Emit the lexed tokens of the input file"),
        clEnumValN(CompilerEmitAction::Src, "src",
                   "Emit the original source code of the input file"),
        clEnumValN(CompilerEmitAction::AST, "ast",
                   "Emit the abstract syntax tree of the input file")),
    llvm::cl::init(CompilerEmitAction::None));

template <typename T>
void reportErrors(
    CompilerErrorFormat format, const lang::SourceFile &file,
    const std::vector<T> &errors,
    std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    switch (format) {
    case CompilerErrorFormat::Text:
        lang::reportTextErrors(file, errors, maxErrors);
        break;
    case CompilerErrorFormat::JSON:
        lang::reportJSONErrors(file, errors, maxErrors);
        break;
    }
}

} // namespace

int main(int argc, char **argv) {
    llvm::cl::ParseCommandLineOptions(argc, argv, "Lang Compiler\n", nullptr,
                                      nullptr, true);

    const auto file = llvm::MemoryBuffer::getFile(inputFilename);

    if (!file) {
        llvm::errs() << "Error: " << file.getError().message() << '\n';
        return EXIT_FAILURE;
    }

    const lang::SourceFile source(inputFilename, file.get()->getBuffer());

    lang::Lexer lexer(file.get()->getBuffer());
    const auto lexResult = lexer.lexAll();

    if (compilerEmitAction == CompilerEmitAction::Lex) {
        for (const auto &token : lexResult.tokens) {
            llvm::outs() << token.toString() << '\n';
        }
    }

    if (lexResult.tokens.empty()) {
        llvm::errs() << "Error: empty file provided\n";
        return EXIT_FAILURE;
    }

    if (!lexResult.errors.empty()) {
        reportErrors(compilerErrorFormat, source, lexResult.errors);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::Lex) {
        return EXIT_SUCCESS;
    }

    lang::Arena arena(lang::kiloBytes(32));
    lang::TypeContext typeCtx(arena);

    lang::ASTPrinter astPrinter(llvm::outs());
    lang::Parser parser(arena, typeCtx, lexResult.tokens);

    const auto parseResult = parser.parseModuleAST();
    DEBUG("%lu allocations in %lu bytes", arena.totalAllocations(),
          arena.totalAllocated());

    if (compilerEmitAction == CompilerEmitAction::Src) {
        // TODO: Implement
        DEBUG("--emit=src is not implement yet");
    }

    if (compilerEmitAction == CompilerEmitAction::AST) {
        astPrinter.visit(*parseResult.module);
    }

    if (!parseResult.errors.empty()) {
        reportErrors(compilerErrorFormat, source, parseResult.errors);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::AST) {
        return EXIT_SUCCESS;
    }

    lang::ModuleAST *module = parseResult.module;

    lang::Resolver resolver;
    const auto resolveResult = resolver.resolveModuleAST(*module);

    if (compilerEmitAction == CompilerEmitAction::AST) {
        astPrinter.visit(*module);
    }

    if (!resolveResult.errors.empty()) {
        reportErrors(compilerErrorFormat, source, resolveResult.errors);
        return EXIT_FAILURE;
    }

    lang::Sema sema(typeCtx);
    const auto semaResult = sema.analyzeModuleAST(*module);

    if (compilerEmitAction == CompilerEmitAction::AST) {
        astPrinter.visit(*module);
    }

    if (!resolveResult.errors.empty()) {
        reportErrors(compilerErrorFormat, source, semaResult.errors);
        return EXIT_FAILURE;
    }
}
