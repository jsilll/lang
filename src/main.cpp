#include "Support/Debug.h"
#include "Support/Reporting.h"
#include "Support/SourceFile.h"

#include "AST/ASTPrinter.h"

#include "Lex/Lexer.h"

#include "Parse/Parser.h"

#include "Analysis/CFA.h"
#include "Analysis/Resolver.h"
#include "Analysis/TypeChecker.h"

#include "Codegen/Codegen.h"

#include "llvm/IR/Verifier.h"
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
    Sema,
};

enum class CompilerEmitAction {
    None,
    Lex,
    Src,
    AST,
    LLVM,
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
    llvm::cl::values(
        clEnumValN(CompilerUntilStage::Lex, "lex",
                   "Run the compiler until the lexing stage"),
        clEnumValN(CompilerUntilStage::AST, "ast",
                   "Run the compiler until the parsing stage"),
        clEnumValN(CompilerUntilStage::Sema, "sema",
                   "Run the compiler until the semantic analysis stage")),
    llvm::cl::init(CompilerUntilStage::None));

const llvm::cl::opt<CompilerEmitAction> compilerEmitAction(
    "emit", llvm::cl::desc("Select the kind of output desired"),
    llvm::cl::values(
        clEnumValN(CompilerEmitAction::Lex, "lex",
                   "Emit the lexed tokens of the input file"),
        clEnumValN(CompilerEmitAction::Src, "src",
                   "Emit the original source code of the input file"),
        clEnumValN(CompilerEmitAction::AST, "ast",
                   "Emit the abstract syntax tree of the input file"),
        clEnumValN(CompilerEmitAction::LLVM, "llvm",
                   "Emit the LLVM IR of the input file")),
    llvm::cl::init(CompilerEmitAction::None));

template <typename T>
void reportErrors(
    llvm::raw_ostream &os, CompilerErrorFormat format,
    const lang::SourceFile &file, const std::vector<T> &errors,
    std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    switch (format) {
    case CompilerErrorFormat::Text:
        lang::reportTextErrors(os, file, errors, maxErrors);
        break;
    case CompilerErrorFormat::JSON:
        lang::reportJSONErrors(os, file, errors, maxErrors);
        break;
    }
}

} // namespace

int main(int argc, char **argv) try {
    llvm::cl::ParseCommandLineOptions(argc, argv, "Lang Compiler\n", nullptr,
                                      nullptr, true);

    const auto file = llvm::MemoryBuffer::getFile(inputFilename);

    if (!file) {
        llvm::errs() << "Error: while opening file " << inputFilename << ": "
                     << file.getError().message() << '\n';
        return EXIT_FAILURE;
    }

    const lang::SourceFile source(inputFilename, file.get()->getBuffer());

    // -------------------------------------------------------------------------
    // Lexing
    // -------------------------------------------------------------------------

    lang::Lexer lexer(file.get()->getBuffer());
    const auto lexResult = lexer.lexAll();

    if (compilerEmitAction == CompilerEmitAction::Lex) {
        for (const auto &token : lexResult.tokens) {
            llvm::outs() << token.toString() << '\n';
        }
    }

    if (lexResult.hasErrors()) {
        reportErrors(llvm::errs(), compilerErrorFormat, source,
                     lexResult.errors);
        return EXIT_FAILURE;
    }

    if (lexResult.tokens.empty()) {
        llvm::errs() << "Error: empty file provided\n";
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::Lex) {
        return EXIT_SUCCESS;
    }

    // -------------------------------------------------------------------------
    // Parsing
    // -------------------------------------------------------------------------

    lang::Arena arena(lang::kiloBytes(32));
    lang::ASTPrinter astPrinter(llvm::outs());

    lang::TypeContext typeCtx(arena);

    lang::Parser parser(arena, typeCtx, lexResult.tokens);
    const auto parseResult = parser.parseModuleAST();

    DEBUG("%lu allocation(s) with %lu bytes", arena.totalAllocations(),
          arena.totalAllocated());

    if (compilerEmitAction == CompilerEmitAction::Src) {
        // TODO: Implement
        DEBUG("--emit=src is not implement yet");
    }

    if (compilerEmitAction == CompilerEmitAction::AST) {
        llvm::outs() << "Parsed AST:\n";
        astPrinter.visit(*parseResult.module);
    }

    if (parseResult.hasErrors()) {
        reportErrors(llvm::errs(), compilerErrorFormat, source,
                     parseResult.errors);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::AST) {
        return EXIT_SUCCESS;
    }

    // -------------------------------------------------------------------------
    // Control Flow Analysis
    // -------------------------------------------------------------------------

    lang::CFA controlFlowAnalyzer;
    const auto cfaResult =
        controlFlowAnalyzer.analyzeModuleAST(*parseResult.module);

    if (cfaResult.hasErrors()) {
        reportErrors(llvm::errs(), compilerErrorFormat, source,
                     cfaResult.errors);
        return EXIT_FAILURE;
    }

    // -------------------------------------------------------------------------
    // Resolution
    // -------------------------------------------------------------------------

    lang::Resolver resolver;
    const auto resolveResult = resolver.resolveModuleAST(*parseResult.module);

    if (compilerEmitAction == CompilerEmitAction::AST) {
        llvm::outs() << "Resolved AST:\n";
        astPrinter.visit(*parseResult.module);
    }

    if (resolveResult.hasErrors()) {
        reportErrors(llvm::errs(), compilerErrorFormat, source,
                     resolveResult.errors);
        return EXIT_FAILURE;
    }

    // -------------------------------------------------------------------------
    // Type Checking
    // -------------------------------------------------------------------------

    lang::TypeChecker typeChecker(typeCtx);
    const auto typeCheckerResult =
        typeChecker.analyzeModuleAST(*parseResult.module);

    DEBUG("%lu custom type(s) created", typeCtx.getNumTypes());

    if (compilerEmitAction == CompilerEmitAction::AST) {
        llvm::outs() << "Type-checked AST:\n";
        astPrinter.visit(*parseResult.module);
    }

    if (resolveResult.hasErrors()) {
        reportErrors(llvm::errs(), compilerErrorFormat, source,
                     typeCheckerResult.errors);
        return EXIT_FAILURE;
    }

    if (compilerUntilStage == CompilerUntilStage::Sema) {
        return EXIT_SUCCESS;
    }

    // -------------------------------------------------------------------------
    // Code Generation
    // -------------------------------------------------------------------------

    lang::Codegen codegen;
    const llvm::Module *llvmModule =
        codegen.generateModule(*parseResult.module);

    if (compilerEmitAction == CompilerEmitAction::LLVM) {
        llvmModule->print(llvm::outs(), nullptr);
    }

    if (llvm::verifyModule(*llvmModule, &llvm::errs())) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} catch (const std::exception &e) {
    llvm::errs() << "Error: " << e.what() << '\n';
    return EXIT_FAILURE;
}
