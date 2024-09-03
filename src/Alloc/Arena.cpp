#include "Alloc/Arena.h"

#include <cstdlib>

namespace lang {

constexpr std::size_t alignUp(std::size_t size, std::size_t align) {
  return (size + align - 1) & ~(align - 1);
}

} // namespace lang

namespace lang {

void *Arena::alloc(std::size_t size) {
  size = alignUp(size, alignof(max_align_t));

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
      block = Block{std::max(size, defaultSize)};
    }

    allocSize = 0;
  }

  void *result = block.data.get() + allocSize;
  allocSize += size;

  return result;
}

} // namespace lang
