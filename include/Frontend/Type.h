#ifndef LANG_TYPE_H
#define LANG_TYPE_H

#include "Alloc/Arena.h"

#include <unordered_map>

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
        typeNumberValue({TypeKind::Number}),
        typeNames({{"void", typeVoid}, {"number", typeNumber}}) {}

  Type* lookupType(std::string_view name) const {
      const auto it = typeNames.find(std::string(name));
      return it != typeNames.cend() ? it->second : nullptr;
  }

private:
  // TODO: Use arena field
  [[maybe_unused]] Arena *arena;
  Type typeVoidValue;
  Type typeNumberValue;
  std::unordered_map<std::string, Type *> typeNames;
};

} // namespace lang

#endif // LANG_TYPE_H
