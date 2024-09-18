#include "Support/Reporting.h"

namespace lang {

// NOLINTNEXTLINE
void reportTextError(llvm::raw_ostream &os, const SourceFile &file,
                     const TextError &error, unsigned lineNoWidthHint) {
    assert(!error.span.empty() && "span cannot be empty");
    const SourceLocation loc = file.getLocation(error.span);
    const unsigned lineNoWidth = getNumDigits(loc.line);
    const unsigned lineNoMaxWidth = std::max(lineNoWidth, lineNoWidthHint);
    const std::string lineNoSpacesTitle(lineNoMaxWidth + 1, ' ');
    const std::string lineNoSpacesBody(lineNoMaxWidth + 2, ' ');
    const std::string lineNoSpacesLine(lineNoMaxWidth - lineNoWidth, ' ');
    const std::string labelSpaces(loc.column - 1, ' ');
    const std::string labelTildes(error.span.size() - 1, '~');
    os << lineNoSpacesTitle << "--> Error at " << loc.filename << ':'
       << loc.line << ':' << loc.column << ": " << error.title << '\n';
    os << lineNoSpacesBody << "|\n";
    os << ' ' << lineNoSpacesLine << loc.line << " | " << loc.lineText << '\n';
    os << lineNoSpacesBody << "| " << labelSpaces << '^' << labelTildes << ' '
       << error.label << '\n';
}

void reportJSONError(llvm::raw_ostream &os, const SourceFile &file,
                     const JSONError &error) {
    assert(!error.span.empty() && "span cannot be empty");
    const SourceLocation loc = file.getLocation(error.span);
    os << "{ \"id\" : \"" << error.title << "\", \"loc\" : \"" << loc.filename
       << ':' << loc.line << ':' << loc.column << "\" }\n";
}

} // namespace lang
