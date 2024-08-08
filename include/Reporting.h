#ifndef LANG_REPORTING_H
#define LANG_REPORTING_H

#include "SourceFile.h"

namespace lang {

struct Error {
    std::string_view span;
    std::string_view title;
    std::string_view label;
};

void reportError(const SourceFile &file, const Error &error);

} // namespace lang

#endif // LANG_REPORTING_H