#include "Support/SourceFile.h"

#include <algorithm>

namespace lang {

// NOLINTNEXTLINE
SourceFile::SourceFile(std::string_view filename, llvm::StringRef buffer)
    : filename(filename) {
    lines.reserve(std::count(buffer.begin(), buffer.end(), '\n') + 1);
    while (!buffer.empty()) {
        const auto [line, remaining] = buffer.split('\n');
        lines.push_back(line);
        buffer = remaining;
    }
}

SourceLocation SourceFile::getLocation(std::string_view span) const {
    auto it = std::upper_bound(
        lines.begin(), lines.end(), span,
        [](const std::string_view &target, const std::string_view &line) {
            return target.data() < line.data();
        });
    if (it != lines.begin()) {
        --it;
    }
    const std::size_t line = it - lines.begin() + 1;
    const std::size_t column = span.data() - it->data() + 1;
    return {line, column, filename, *it};
}

} // namespace lang
