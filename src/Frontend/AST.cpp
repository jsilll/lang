#include "Frontend/AST.h"

#include <unordered_map>

namespace {

const std::unordered_map<lang::BinOpKind, std::string_view> binOpMap = {
    {lang::BinOpKind::Ne, "!="},
    {lang::BinOpKind::Le, "<="},
    {lang::BinOpKind::Ge, ">="},
    {lang::BinOpKind::Or, "||"},
};

} // namespace

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

    const auto it = binOpMap.find(kind);
    if (it == binOpMap.end()) {
        return "an unknown binary operator";
    }

    return std::string(it->second);
}

} // namespace lang
