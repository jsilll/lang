#ifndef LANG_REPORTING_H
#define LANG_REPORTING_H

#include "SourceFile.h"

namespace lang {

struct PrettyError {
  std::string_view span;
  std::string_view title;
  std::string label;
};

void reportError(const SourceFile &file, const PrettyError &error);

template <typename T>
void reportErrors(
    const lang::SourceFile &source, const std::vector<T> &errors,
    const std::size_t maxErrors = std::numeric_limits<std::size_t>::max()) {
  std::size_t numErrors = 0;
  for (const auto &error : errors) {
    lang::reportError(source, error.toPretty());
    if (++numErrors >= maxErrors) {
      break;
    }
  }
}

} // namespace lang

#endif // LANG_REPORTING_H
