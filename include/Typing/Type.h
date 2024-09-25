#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include "ADT/NonOwningList.h"

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

    template <typename T> T *as() { return static_cast<T *>(this); }
    template <typename T> const T *as() const {
        return static_cast<const T *>(this);
    }
};

struct PointerType : public Type {
    Type *pointee;
    PointerType(Type *pointee) : Type(TypeKind::Pointer), pointee(pointee) {}
};

struct FunctionType : public Type {
    NonOwningList<Type *> arrows;
    FunctionType(NonOwningList<Type *> arrows)
        : Type(TypeKind::Function), arrows(arrows) {}
};

} // namespace lang

#endif // LANG_TYPE_H
