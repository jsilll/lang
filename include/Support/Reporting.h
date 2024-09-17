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

struct TextError {
    std::string_view span;
    std::string_view title;
    std::string label;
};

void reportTextError(const SourceFile &file, const TextError &error,
                     unsigned lineNoWidthHint = 0);

struct JSONError {
    std::string_view span;
    std::string_view title;
};

void reportJSONError(const SourceFile &file, const JSONError &error);

/// @brief Reports a vector of errors in batch in plain text
/// @pre errors only contains errors from the same file
/// @pre errors is sorted in the order of appearence within the file
template <typename T>
void reportTextErrors(
    const SourceFile &file, const std::vector<T> &errors,
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
        lang::reportTextError(file, error.toTextError(), lineNoMaxWidth);
        if (i < lastButOne) {
            llvm::errs() << lineNoSpacesBody << "|\n";
        }
    }
}

/// @brief Reports a vector of errors in batch in JSON format
template <typename T>
void reportJSONErrors(
    const SourceFile &file, const std::vector<T> &errors,
    std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
    maxErrors = std::min(errors.size(), maxErrors);
    if (maxErrors == 0) {
        return;
    }

    llvm::errs() << "[\n";
    for (const auto &error : errors) {
        llvm::errs() << "  ";
        lang::reportJSONError(file, error.toJSONError());
    }
    llvm::errs() << "]\n";
}

} // namespace lang

#endif // LANG_REPORTING_H
