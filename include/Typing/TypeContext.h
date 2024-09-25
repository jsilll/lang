#ifndef LANG_TYPE_CONTEXT
#define LANG_TYPE_CONTEXT

#include "Alloc/Arena.h"

#include "Type.h"

#include <unordered_map>

namespace lang {

class TypeContext {
  public:
    explicit TypeContext(Arena &arena) : arena(&arena) {
        tyVoid = arena.alloc<Type>(TypeKind::Void);
        tyNumber = arena.alloc<Type>(TypeKind::Number);
    }

    Type *getTypeVoid() const { return tyVoid; }

    Type *getTypeNumber() const { return tyNumber; }

    template <typename... Args> Type *make(Args &&...args) {
        Type *type = arena->alloc<Type>(std::forward<Args>(args)...);
        return type;
    }

  private:
    struct HashType {
        std::size_t operator()(const Type *type) const { return 0; }
    };

    struct EqualType {
        bool operator()(const Type *lhs, const Type *rhs) const {
            return false;
        }
    };

    Arena *arena;
    Type *tyVoid;
    Type *tyNumber;
    std::unordered_map<Type *, Type *, HashType, EqualType> typeMap;

    Type *hashCons(Type *type) {
        switch (type->kind) {
        case TypeKind::Void:
            return tyVoid;
        case TypeKind::Number:
            return tyNumber;
        case TypeKind::Pointer: {
            PointerType *pointerType = type->as<PointerType>();
            pointerType->pointee = hashCons(pointerType->pointee);
            const auto [it, inserted] = typeMap.emplace(type, pointerType);
            return it->second;
        }
        case TypeKind::Function:
            FunctionType *functionType = type->as<FunctionType>();
            for (Type *&arrow : functionType->arrows) {
                arrow = hashCons(arrow);
            }
            const auto [it, inserted] = typeMap.emplace(type, functionType);
            return it->second;
        }
    }
};

} // namespace lang

#endif // LANG_TYPE_CONTEXT
