#include "Frontend/AST.h"

namespace lang {

std::string unOpKindToString(UnOpKind kind) {
    return std::string(1, static_cast<char>(kind));
}

std::string binOpKindToString(BinOpKind kind) {
    const int kindInt = static_cast<int>(kind);
    if (kindInt >= static_cast<int>(BinOpKind::Mul) &&
        kindInt <= static_cast<int>(BinOpKind::Gt)) {
        return std::string(1, static_cast<char>(kind));
    }

    switch (kind) {
    case BinOpKind::Ne:
        return "!=";
    case BinOpKind::Le:
        return "<=";
    case BinOpKind::Ge:
        return ">=";
    case BinOpKind::And:
        return "&&";
    case BinOpKind::Or:
        return "||";
    default:
        return "an unknown binary operator";
    }
}

} // namespace lang
