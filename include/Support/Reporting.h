#ifndef LANG_REPORTING_H
#define LANG_REPORTING_H

#include "SourceFile.h"

#include "llvm/Support/raw_ostream.h"

namespace lang {

// TODO: Move this to a different file
constexpr unsigned getNumDigits(unsigned n) {
    unsigned digits = 0;
    while (n) {
        n /= 10;
        ++digits;
    }
    return digits;
}

struct PrettyError {
    std::string_view span;
    std::string_view title;
    std::string label;
};

void reportError(const SourceFile &file, const PrettyError &error,
                 unsigned lineNoWidthHint = 0);

/// @brief Reports a vector of errors in batch
/// @pre errors only contains errors from the same file
/// @pre errors is sorted in the order of appearence within the file
template <typename T>
void reportErrors(
    const lang::SourceFile &file, const std::vector<T> &errors,
    const std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    const std::size_t numErrors = std::min(errors.size(), maxErrors);
    if (numErrors == 0) {
        return;
    }

    const std::size_t maxLine = file.getLocation(errors.back().span).line;
    const unsigned lineNoMaxWidth = getNumDigits(maxLine);
    const std::string lineNoSpacesBody = std::string(lineNoMaxWidth + 2, ' ');

    const std::size_t lastButOne = numErrors - 1;
    for (std::size_t i = 0; i < numErrors; ++i) {
        const auto &error = errors[i];
        lang::reportError(file, error.toPretty(), lineNoMaxWidth);
        if (i < lastButOne) {
            llvm::outs() << lineNoSpacesBody << "|\n";
        }
    }
}

} // namespace lang

#endif // LANG_REPORTING_H
