#include "Sema/Type.h"

namespace lang {

std::string Type::toString() const {
    switch (kind) {
    case TypeKind::Void:
        return "void";
    case TypeKind::Number:
        return "number";
    case TypeKind::Pointer:
        return "pointer";
    case TypeKind::Function:
        return "function";
    }
    return "unknown";
}

} // namespace lang
