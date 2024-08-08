#ifndef LANG_ARENA_H
#define LANG_ARENA_H

#include <list>
#include <utility>

namespace lang {

class Arena {
public:
  Arena(std::size_t defaultSize)
      : defaultSize(defaultSize), allocSize(0), blockSize(0), block(nullptr) {}
  ~Arena();

  Arena(const Arena &) = delete;
  Arena &operator=(const Arena &) = delete;

  Arena(Arena &&) = default;
  Arena &operator=(Arena &&) = default;

  std::size_t totalAllocated() const;

  template <typename T> T *alloc(std::size_t count = 1) {
    return static_cast<T *>(alloc(sizeof(T) * count));
  }
  
  void reset();

private:
  void *alloc(std::size_t bytes);

  std::size_t defaultSize;
  std::size_t allocSize;
  std::size_t blockSize;
  std::byte *block;
  std::list<std::pair<std::byte *, std::size_t>> used;
  std::list<std::pair<std::byte *, std::size_t>> avail;
};

} // namespace lang

#endif // LANG_ARENA_H