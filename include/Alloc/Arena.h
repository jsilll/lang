#ifndef LANG_ARENA_H
#define LANG_ARENA_H

#include <cstddef>
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
    explicit Arena(std::size_t bytes)
        : defaultSize(bytes), numAllocations(0), allocSize(0) {}

    Arena(const Arena &) = delete;
    Arena &operator=(const Arena &) = delete;

    Arena(Arena &&) = default;
    Arena &operator=(Arena &&) = default;

    [[nodiscard]] std::size_t totalAllocations() const {
        return numAllocations;
    }

    [[nodiscard]] std::size_t totalAllocated() const {
        std::size_t total = allocSize;
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

    template <typename T> void dealloc(T *ptr) {
        static_assert(std::is_trivially_destructible_v<T>,
                      "T must be trivially destructible");
        std::byte *start = block.data.get() + allocSize - sizeof(T);
        assert(reinterpret_cast<T *>(start) == ptr);
        allocSize -= sizeof(T);
    }

    template <typename T, typename... Args> T *alloc(Args &&...args) {
        static_assert(std::is_trivially_destructible_v<T>,
                      "T must be trivially destructible");
        ++numAllocations;
        return new (allocInternal(sizeof(T))) T(std::forward<Args>(args)...);
    }

  private:
    struct Block {
        std::size_t size = 0;
        std::unique_ptr<std::byte[]> data = nullptr;
    };

    std::size_t defaultSize;
    std::size_t numAllocations;
    std::size_t allocSize;
    Block block;
    std::list<Block> used;
    std::list<Block> avail;

    void *allocInternal(std::size_t bytes);
};

} // namespace lang

#endif // LANG_ARENA_H
