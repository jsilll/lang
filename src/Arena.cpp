#include "Arena.h"

#include <cstdlib>
#include <tuple>

namespace lang {

Arena::~Arena() {
  if (block != nullptr) {
    std::free(block);
  }
  for (auto &block : used) {
    std::free(block.first);
  }
  for (auto &block : avail) {
    std::free(block.first);
  }
}

std::size_t Arena::totalAllocated() const {
  std::size_t total = blockSize;
  for (const auto &block : used) {
    total += block.second;
  }
  for (const auto &block : avail) {
    total += block.second;
  }
  return total;
}

void Arena::reset() {
  allocSize = 0;
  avail.splice(avail.begin(), used);
}

void *Arena::alloc(std::size_t size) {
  if (allocSize + size > blockSize) {
    if (block != nullptr) {
      used.emplace_back(block, blockSize);
      block = nullptr;
    }
    for (auto it = avail.begin(); it != avail.end(); ++it) {
      if (it->second >= size) {
        std::tie(block, blockSize) = *it;
        avail.erase(it);
        break;
      }
    }
    if (block == nullptr) {
      blockSize = std::max(size, defaultSize);
      block = static_cast<std::byte *>(std::malloc(blockSize));
    }
    allocSize = 0;
  }
  void *result = block + allocSize;
  allocSize += size;
  return result;
}

} // namespace lang