#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include "Alloc/Arena.h"

namespace lang {

enum class TypeKind {
  Void,
  Number,
};

struct Type {
  TypeKind kind;
  Type(TypeKind kind) : kind(kind) {}
};

class TypeContext {
public:
  explicit TypeContext(Arena &arena) : arena(&arena) {
    tyVoid = arena.make<Type>(TypeKind::Void);
    tyNumber = arena.make<Type>(TypeKind::Number);
  }

  Type *getTyVoid() const { return tyVoid; }

  Type *getTyNumber() const { return tyNumber; }

  template <typename... Args> Type *make(Args &&...args) {
    return arena->make<Type>(std::forward<Args>(args)...);
  }

private:
  Arena *arena;
  Type *tyVoid;
  Type *tyNumber;
};

} // namespace lang

#endif // LANG_TYPE_H
