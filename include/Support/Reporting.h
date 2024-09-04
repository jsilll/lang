#ifndef LANG_REPORTING_H
#define LANG_REPORTING_H

#include "SourceFile.h"

#include "llvm/Support/raw_ostream.h"

namespace lang {

struct PrettyError {
    std::string_view span;
    std::string_view title;
    std::string label;
};

void reportError(const SourceFile &file, const PrettyError &error,
                 unsigned lineNoWidthHint = 0);

constexpr unsigned getNumDigits(unsigned n) {
    unsigned digits = 0;
    while (n) {
        n /= 10;
        ++digits;
    }
    return digits;
}

template <typename T>
void reportErrors(
    const lang::SourceFile &file, const std::vector<T> &errors,
    const std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    const std::size_t numErrors = std::min(errors.size(), maxErrors);
    if (numErrors == 0) {
        return;
    }

    std::size_t maxLine = 0;
    for (std::size_t i = 0; i < numErrors; ++i) {
        const auto &error = errors[i];
        const SourceLocation loc = file.getLocation(error.span);
        maxLine = std::max(maxLine, loc.line);
    }

    const std::size_t lastButOne = numErrors - 1;
    const unsigned lineNoWidthMax = getNumDigits(maxLine);
    const std::string lineNoSpacesBody = std::string(lineNoWidthMax + 2, ' ');

    for (std::size_t i = 0; i < numErrors; ++i) {
        const auto &error = errors[i];
        lang::reportError(file, error.toPretty(), lineNoWidthMax);
        if (i < lastButOne) {
            llvm::outs() << lineNoSpacesBody << "|\n";
        }
    }
}

} // namespace lang

#endif // LANG_REPORTING_H
