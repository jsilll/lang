#ifndef LANG_LIST_H
#define LANG_LIST_H

#include <cstddef>
#include <iterator>

/// @brief A singly linked list.
/// @tparam T The type of the elements in the list.
///
/// @note This is a raw implementation of a singly linked list. It is meant to
/// replace allocating alternatives like std::vector and std::list. This way,
/// the caller can decide how to allocate the nodes, and the list will not
/// delete them on destruction. For now this is used in the AST representation
/// since the whole AST is allocated in one contiguous block of memory with no
/// RAII semantics. This discards having nodes that are not trivially
/// destructible, as would happen if std::vector or std::list were used in nodes
/// with a variable number of children (e.g. BlockStmtAST).
template <typename T> class List {
public:
  struct Node {
    T data;
    Node *next;
    Node(const T &data) noexcept : data(data), next(nullptr) {}
  };

  // NOLINTNEXTLINE
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    iterator(Node *node) noexcept : cur(node) {}

    reference operator*() const { return cur->data; }

    pointer operator->() const { return &cur->data; }

    iterator &operator++() {
      cur = cur->next;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      cur = cur->next;
      return tmp;
    }

    bool operator==(const iterator &other) const {
      return cur == other.cur;
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }

  private:
    Node *cur;
  };

  // NOLINTNEXTLINE
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T *;
    using reference = const T &;

    const_iterator(const Node *node) noexcept : cur(node) {}

    reference operator*() const { return cur->data; }
    pointer operator->() const { return &cur->data; }

    const_iterator &operator++() {
      cur = cur->next;
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      cur = cur->next;
      return tmp;
    }

    bool operator==(const const_iterator &other) const {
      return cur == other.cur;
    }

    bool operator!=(const const_iterator &other) const {
      return !(*this == other);
    }

  private:
    const Node *cur;
  };

  List() noexcept : head(nullptr), tail(nullptr), size_(0) {}
  ~List() = default;

  List(List &&) = delete;
  List &operator=(List &&) = delete;

  List(const List &) = default;
  List &operator=(const List &) = default;

  bool empty() const noexcept { return size_ == 0; }
  std::size_t size() const noexcept { return size_; }

  iterator begin() noexcept { return iterator(head); }
  const_iterator begin() const noexcept { return const_iterator(head); }
  const_iterator cbegin() const noexcept { return const_iterator(head); }

  iterator end() noexcept { return iterator(nullptr); }
  const_iterator end() const noexcept { return const_iterator(nullptr); }
  const_iterator cend() const noexcept { return const_iterator(nullptr); }

  T &front() { return head->data; }
  const T &front() const { return head->data; }
  T &back() { return tail->data; }
  const T &back() const { return tail->data; }

  void clear() {
    head = tail = nullptr;
    size_ = 0;
  }

  // NOLINTNEXTLINE
  void push_front(Node *node) {
    node->next = head;
    head = node;
    if (tail == nullptr) {
      tail = node;
    }
    ++size_;
  }

  // NOLINTNEXTLINE
  void push_back(Node *node) {
    if (empty()) {
      head = tail = node;
    } else {
      tail->next = node;
      tail = node;
    }
    ++size_;
  }

  // NOLINTNEXTLINE
  void pop_front() {
    if (empty()) {
      return;
    }
    Node *tmp = head;
    head = head->next;
    if (head == nullptr) {
      tail = nullptr;
    }
    --size_;
    // NOTE: We don't delete the node, as it's managed by the caller.
  }

  template <typename Alloc, typename... Args>
  // NOLINTNEXTLINE
  Node *emplace_front(Alloc &alloc, Args &&...args) {
    Node *node = alloc.template make<Node>(std::forward<Args>(args)...);
    push_front(node);
    return node;
  }

  template <typename Alloc, typename... Args>
  // NOLINTNEXTLINE
  Node *emplace_back(Alloc &alloc, Args &&...args) {
    Node *node = alloc->template make<Node>(std::forward<Args>(args)...);
    push_back(node);
    return node;
  }

private:
  Node *head;
  Node *tail;
  // NOLINTNEXTLINE
  std::size_t size_;
};

#endif // LANG_LIST_H
