#include "Sema/Type.h"

namespace lang {

std::string Type::toString() const {
    switch (kind) {
    case TypeKind::Void:
        return "void";
    case TypeKind::Number:
        return "number";
    }
    return "unknown";
}

} // namespace lang
