#include "AST/AST.h"

namespace lang {

std::string unOpKindToString(UnOpKind kind) {
    switch (kind) {
    case UnOpKind::Neg:
    case UnOpKind::Not:
        return std::string(1, static_cast<char>(kind));
    }
    return "unkown";
}

std::string binOpKindToString(BinOpKind kind) {
    switch (kind) {
    case BinOpKind::Mul:
    case BinOpKind::Add:
    case BinOpKind::Sub:
    case BinOpKind::Div:
    case BinOpKind::Lt:
    case BinOpKind::Eq:
    case BinOpKind::Gt:
        return std::string(1, static_cast<char>(kind));
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
    }
    return "unknown";
}

} // namespace lang
