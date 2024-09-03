#ifndef LANG_ARENA_H
#define LANG_ARENA_H

#include <list>
#include <memory>

namespace lang {

constexpr std::size_t kiloBytes(const std::size_t bytes) {
  return bytes * 1024;
}

constexpr std::size_t megaBytes(const std::size_t bytes) {
  return bytes * 1024 * 1024;
}

constexpr std::size_t gigaBytes(const std::size_t bytes) {
  return bytes * 1024 * 1024 * 1024;
}

class Arena {
public:
  explicit Arena(std::size_t bytes) : defaultSize(bytes), allocSize(0) {}

  Arena(const Arena &) = delete;
  Arena &operator=(const Arena &) = delete;

  Arena(Arena &&) = default;
  Arena &operator=(Arena &&) = default;

  [[nodiscard]] std::size_t totalAllocated() const {
    std::size_t total = block.size;
    for (const auto &block : used) {
      total += block.size;
    }
    for (const auto &block : avail) {
      total += block.size;
    }
    return total;
  }

  void reset() {
    allocSize = 0;
    avail.splice(avail.begin(), used);
  }

  template <typename T, typename... Args> T *make(Args &&...args) {
    static_assert(std::is_trivially_destructible_v<T>,
                  "T must be trivially destructible");
    return new (alloc(sizeof(T))) T(std::forward<Args>(args)...);
  }

private:
  struct Block {
    std::unique_ptr<std::byte[]> data;
    std::size_t size;
    Block() : data(nullptr), size(0) {}
    Block(std::size_t size)
        : data(std::make_unique<std::byte[]>(size)), size(size) {}
  };

  void *alloc(std::size_t bytes);

  std::size_t defaultSize;
  std::size_t allocSize;
  Block block;
  std::list<Block> used;
  std::list<Block> avail;
};

} // namespace lang

#endif // LANG_ARENA_H
