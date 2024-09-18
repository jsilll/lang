#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include <string>

namespace lang {

enum class TypeKind {
    Void,
    Number,
    Pointer,
    Function,
};

struct Type {
    TypeKind kind;
    explicit Type(TypeKind kind) : kind(kind) {}
    std::string toString() const;
    template<typename T> T& as() { return static_cast<T&>(this); }
    template<typename T> const T& as() { return static_cast<const T&>(this); }
};

struct PointerType : public Type {
    PointerType() : Type(TypeKind::Pointer) {}
};

struct FunctionType : public Type {
    FunctionType() : Type(TypeKind::Function) {}
};

} // namespace lang

#endif // LANG_TYPE_H
