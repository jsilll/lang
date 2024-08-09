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
};

class TypeContext {
public:
  Type *typeVoid = &typeVoidValue;
  Type *typeNumber = &typeNumberValue;

  explicit TypeContext(Arena &arena)
      : arena(&arena), typeVoidValue({TypeKind::Void}),
        typeNumberValue({TypeKind::Number}) {}

private:
  [[maybe_unused]] Arena *arena;
  Type typeVoidValue;
  Type typeNumberValue;
};

} // namespace lang

#endif // LANG_TYPE_H