#include "Reporting.h"

#include "llvm/Support/raw_ostream.h"

namespace {

unsigned getNumDigits(unsigned n) {
  unsigned digits = 0;
  while (n) {
    n /= 10;
    digits++;
  }
  return digits;
}

} // namespace

namespace lang {

// NOLINTNEXTLINE
void reportError(const SourceFile &file, const Error &error) {
  assert(!error.span.empty() && "span cannot be empty");
  SourceLoc loc = file.getLocation(error.span);
  unsigned lineNoWidth = getNumDigits(loc.line);
  std::string lineNoSpacesTitle(lineNoWidth, ' ');
  std::string lineNoSpacesBody(lineNoWidth + 1, ' ');
  std::string labelSpaces(loc.column - 1, ' ');
  std::string labelTildes(error.span.size() - 1, '~');
  llvm::errs() << lineNoSpacesTitle << " --> Error at " << loc.filename << ":"
               << loc.line << ":" << loc.column << ": " << error.title << "\n";
  llvm::errs() << lineNoSpacesBody << " |\n";
  llvm::errs() << " " << loc.line << " | " << loc.lineText << "\n";
  llvm::errs() << lineNoSpacesBody << " | " << labelSpaces << "^" << labelTildes
               << " " << error.label << "\n";
}

} // namespace lang