#include "Support/SourceFile.h"

#include <tuple>

namespace lang {

// NOLINTNEXTLINE
SourceFile::SourceFile(std::string_view filename, llvm::StringRef buffer)
    : filename(filename) {
  llvm::StringRef line;
  while (!buffer.empty()) {
    std::tie(line, buffer) = buffer.split('\n');
    lines.push_back(line);
  }
}

SourceLocation SourceFile::getLocation(std::string_view span) const {
  std::size_t begin = 0;
  std::size_t end = lines.size();
  std::size_t mid = begin + (end - begin) / 2;
  while (begin < end) {
    if (span.data() < lines[mid].data()) {
      end = mid;
    } else if (span.data() >= lines[mid].data() + lines[mid].size()) {
      begin = mid + 1;
    } else {
      break;
    }
    mid = begin + (end - begin) / 2;
  }
  const std::size_t column = span.data() - lines[mid].data();
  return {mid + 1, column + 1, filename, lines[mid]};
}

} // namespace lang