#ifndef LANG_SOURCE_FILE_H
#define LANG_SOURCE_FILE_H

#include "llvm/ADT/StringRef.h"

#include <vector>

namespace lang {

struct SourceLocation {
    std::size_t line;
    std::size_t column;
    std::string_view filename;
    std::string_view lineText;
};

struct SourceFile {
  public:
    SourceFile(std::string_view filename, llvm::StringRef buffer);

    SourceLocation getLocation(std::string_view span) const;

  private:
    std::string_view filename;
    std::vector<std::string_view> lines;
};

} // namespace lang

#endif // LANG_SOURCE_FILE_H
