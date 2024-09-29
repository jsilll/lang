#ifndef LANG_REPORTING_H
#define LANG_REPORTING_H

#include "SourceFile.h"
#include "Utilities.h"

#include "llvm/Support/raw_ostream.h"

namespace lang {

struct TextError {
    std::string_view span;
    std::string_view title;
    std::string label;

    // NOLINTNEXTLINE
    TextError(std::string_view span, std::string_view title,
              std::string_view label)
        : span(span), title(title), label(label) {}
};

struct JSONError {
    std::string_view span;
    std::string_view title;

    // NOLINTNEXTLINE
    JSONError(std::string_view span, std::string_view title)
        : span(span), title(title) {}
};

void reportTextError(llvm::raw_ostream &os, const SourceFile &file,
                     const TextError &error, unsigned lineNoWidthHint = 0);

void reportJSONError(llvm::raw_ostream &os, const SourceFile &file,
                     const JSONError &error);

/// @brief Reports a vector of errors in batch in plain text
/// @note To make proper use of this function, the following preconditions must
/// be met:
///        - errors only contains errors from the same file
///        - errors is sorted in the order of appearence within the file
template <typename T>
void reportTextErrors(
    llvm::raw_ostream &os, const SourceFile &file, const std::vector<T> &errors,
    std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    maxErrors = std::min(errors.size(), maxErrors);
    if (maxErrors == 0) {
        return;
    }

    const std::size_t maxLine = file.getLocation(errors.back().span).line;
    const unsigned lineNoMaxWidth = getNumDigits(maxLine);
    const std::string lineNoSpacesBody = std::string(lineNoMaxWidth + 2, ' ');

    const std::size_t lastButOne = maxErrors - 1;
    for (std::size_t i = 0; i < maxErrors; ++i) {
        const auto &error = errors[i];
        lang::reportTextError(os, file, error.toTextError(), lineNoMaxWidth);
        if (i < lastButOne) {
            os << lineNoSpacesBody << "|\n";
        }
    }
}

/// @brief Reports a vector of errors in batch in JSON format
template <typename T>
void reportJSONErrors(
    llvm::raw_ostream &os, const SourceFile &file, const std::vector<T> &errors,
    std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    maxErrors = std::min(errors.size(), maxErrors);
    if (maxErrors == 0) {
        return;
    }

    llvm::errs() << "[\n";
    const std::size_t lastButOne = maxErrors - 1;
    for (std::size_t i = 0; i < maxErrors; ++i) {
        const auto &error = errors[i];

        os << "  ";
        lang::reportJSONError(os, file, error.toJSONError());

        if (i < lastButOne) {
            os << ",\n";
        }
    }
    llvm::errs() << "]\n";
}

} // namespace lang

#endif // LANG_REPORTING_H
