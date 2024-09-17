#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include "Alloc/Arena.h"

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

class TypeContext {
  public:
    explicit TypeContext(Arena &arena) : arena(&arena) {
        tyVoid = arena.alloc<Type>(TypeKind::Void);
        tyNumber = arena.alloc<Type>(TypeKind::Number);
    }

    Type *getTypeVoid() const { return tyVoid; }

    Type *getTypeNumber() const { return tyNumber; }

    // TODO: Implement hash-consing
    template <typename... Args> Type *make(Args &&...args) {
        return arena->alloc<Type>(std::forward<Args>(args)...);
    }

  private:
    Arena *arena;

    Type *tyVoid;
    Type *tyNumber;

    // TODO: declare hashCons(Type &type);
};

struct PointerType : public Type {
    PointerType() : Type(TypeKind::Pointer) {}
};

struct FunctionType : public Type {
    FunctionType() : Type(TypeKind::Function) {}
};

} // namespace lang

#endif // LANG_TYPE_H
