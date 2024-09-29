#include "Alloc/Arena.h"

#include <cstdlib>

namespace lang {

void *Arena::allocInternal(std::size_t size) {
    constexpr auto alignMask = alignof(max_align_t) - 1;
    size = (size + alignMask) & ~alignMask;

    if (allocSize + size > block.size) {
        if (block.data != nullptr) {
            used.emplace_back(std::move(block));
        }

        for (auto it = avail.begin(); it != avail.end(); ++it) {
            if (it->size >= size) {
                block = std::move(*it);
                avail.erase(it);
                break;
            }
        }

        if (block.data == nullptr) {
            block = Block(std::max(size, defaultSize));
        }

        allocSize = 0;
    }

    void *result = block.data.get() + allocSize;
    allocSize += size;

    return result;
}

} // namespace lang
