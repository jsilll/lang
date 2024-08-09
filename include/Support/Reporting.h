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

} // namespace lang

#endif // LANG_REPORTING_H
