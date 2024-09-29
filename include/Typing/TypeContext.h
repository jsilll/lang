#ifndef LANG_TYPE_CONTEXT
#define LANG_TYPE_CONTEXT

#include "Alloc/Arena.h"

#include "Type.h"

#include <unordered_set>

namespace lang {

struct HashType {
    std::size_t operator()(const Type *type) const {
        switch (type->kind) {
        case TypeKind::Void:
            return 0;
        case TypeKind::Number:
            return 1;
        case TypeKind::Pointer: {
            const PointerType *ptrType = type->as<PointerType>();
            return 2 * operator()(ptrType->pointee);
        }
        case TypeKind::Function: {
            const FunctionType *fnType = type->as<FunctionType>();
            std::size_t hash = 3;
            for (const Type *param : fnType->arrows) {
                hash = hash * 31 + operator()(param);
            }
            return hash;
        }
        }
        return 0;
    }
};

struct EqualType {
    bool operator()(const Type *lhs, const Type *rhs) const {
        if (lhs->kind != rhs->kind) {
            return false;
        }
        switch (lhs->kind) {
        case TypeKind::Void:
        case TypeKind::Number:
            return true;
        case TypeKind::Pointer: {
            const PointerType *lhsPtr = lhs->as<PointerType>();
            const PointerType *rhsPtr = rhs->as<PointerType>();
            return operator()(lhsPtr->pointee, rhsPtr->pointee);
        }
        case TypeKind::Function: {
            const FunctionType *lhsFn = lhs->as<FunctionType>();
            const FunctionType *rhsFn = rhs->as<FunctionType>();
            if (lhsFn->arrows.size() != rhsFn->arrows.size()) {
                return false;
            }
            auto iterLhs = lhsFn->arrows.begin();
            auto iterRhs = rhsFn->arrows.begin();
            for (; iterLhs != lhsFn->arrows.end(); ++iterLhs, ++iterRhs) {
                if (!operator()(*iterLhs, *iterRhs)) {
                    return false;
                }
            }
            return true;
        }
        }
        return false;
    }
};

class TypeContext {
  public:
    explicit TypeContext(Arena &arena) : arena(&arena) {
        tyVoid = arena.alloc<Type>(TypeKind::Void);
        tyNumber = arena.alloc<Type>(TypeKind::Number);
    }

    Arena *getArena() const { return arena; }

    Type *getTypeVoid() const { return tyVoid; }

    Type *getTypeNumber() const { return tyNumber; }

    std::size_t getNumTypes() const { return typeSet.size(); }

    template <typename T, typename... Args> Type *make(Args &&...args) {
        auto [type, inserted] =
            hashCons(arena->alloc<T>(std::forward<Args>(args)...));
        if (!inserted) {
            arena->dealloc(type);
        }
        return type;
    }

  private:
    Arena *arena;
    Type *tyVoid;
    Type *tyNumber;
    std::unordered_set<Type *, HashType, EqualType> typeSet;

    std::pair<Type *, bool> hashCons(Type *type) {
        switch (type->kind) {
        case TypeKind::Void:
            return {tyVoid, false};
        case TypeKind::Number:
            return {tyNumber, false};
        case TypeKind::Pointer: {
            PointerType *ptrType = type->as<PointerType>();
            ptrType->pointee = hashCons(ptrType->pointee).first;
            const auto [it, inserted] = typeSet.insert(type);
            return {*it, inserted};
        }
        case TypeKind::Function:
            FunctionType *fnType = type->as<FunctionType>();
            for (Type *&param : fnType->arrows) {
                param = hashCons(param).first;
            }
            const auto [it, inserted] = typeSet.insert(type);
            return {*it, inserted};
        }
        return {nullptr, false};
    }
};

} // namespace lang

#endif // LANG_TYPE_CONTEXT
