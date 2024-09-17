#include "Support/Reporting.h"

namespace lang {

// NOLINTNEXTLINE
void reportTextError(const SourceFile &file, const TextError &error,
                     unsigned lineNoWidthHint) {
    assert(!error.span.empty() && "span cannot be empty");
    const SourceLocation loc = file.getLocation(error.span);
    const unsigned lineNoWidth = getNumDigits(loc.line);
    const unsigned lineNoMaxWidth = std::max(lineNoWidth, lineNoWidthHint);
    const std::string lineNoSpacesTitle(lineNoMaxWidth + 1, ' ');
    const std::string lineNoSpacesBody(lineNoMaxWidth + 2, ' ');
    const std::string lineNoSpacesLine(lineNoMaxWidth - lineNoWidth, ' ');
    const std::string labelSpaces(loc.column - 1, ' ');
    const std::string labelTildes(error.span.size() - 1, '~');
    llvm::errs() << lineNoSpacesTitle << "--> Error at " << loc.filename << ':'
                 << loc.line << ':' << loc.column << ": " << error.title
                 << '\n';
    llvm::errs() << lineNoSpacesBody << "|\n";
    llvm::errs() << ' ' << lineNoSpacesLine << loc.line << " | " << loc.lineText
                 << '\n';
    llvm::errs() << lineNoSpacesBody << "| " << labelSpaces << '^'
                 << labelTildes << ' ' << error.label << '\n';
}

void reportJSONError(const SourceFile &file, const JSONError &error) {
    assert(!error.span.empty() && "span cannot be empty");
    const SourceLocation loc = file.getLocation(error.span);
    llvm::errs() << "{ \"id\" : \"" << error.title << "\", \"location\" : \"" << loc.filename << ':'
                 << loc.line << ':' << loc.column << "\" },\n";
}

} // namespace lang
