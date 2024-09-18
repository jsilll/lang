#ifndef LANG_TYPE_CONTEXT
#define LANG_TYPE_CONTEXT

#include "Alloc/Arena.h"

#include "Type.h"

namespace lang {

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

} // namespace lang

#endif // LANG_TYPE_CONTEXT
