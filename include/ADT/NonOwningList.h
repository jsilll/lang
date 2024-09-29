#ifndef LANG_NON_OWNING_LIST_H
#define LANG_NON_OWNING_LIST_H

#include <cstddef>
#include <iterator>

template <typename T> class NonOwningList {
  public:
    struct Node {
        T data;
        Node *next;

        explicit Node(const T &data) noexcept : data(data), next(nullptr) {}
    };

    // NOLINTNEXTLINE
    class iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        explicit iterator(Node *node) noexcept : cur(node) {}

        [[nodiscard]] reference operator*() const { return cur->data; }

        [[nodiscard]] pointer operator->() const { return &cur->data; }

        iterator &operator++() {
            cur = cur->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            cur = cur->next;
            return tmp;
        }

        [[nodiscard]] bool operator==(const iterator &other) const {
            return cur == other.cur;
        }

        [[nodiscard]] bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

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

        explicit const_iterator(const Node *node) noexcept : cur(node) {}

        [[nodiscard]] reference operator*() const { return cur->data; }
        [[nodiscard]] pointer operator->() const { return &cur->data; }

        const_iterator &operator++() {
            cur = cur->next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            cur = cur->next;
            return tmp;
        }

        [[nodiscard]] bool operator==(const const_iterator &other) const {
            return cur == other.cur;
        }

        [[nodiscard]] bool operator!=(const const_iterator &other) const {
            return !(*this == other);
        }

      private:
        const Node *cur;
    };

    NonOwningList() noexcept : mHead(nullptr), mTail(nullptr), mSize(0) {}
    ~NonOwningList() = default;

    NonOwningList(NonOwningList &&) = delete;
    NonOwningList &operator=(NonOwningList &&) = delete;

    NonOwningList(const NonOwningList &) = default;
    NonOwningList &operator=(const NonOwningList &) = default;

    [[nodiscard]] bool empty() const noexcept { return mSize == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return mSize; }

    [[nodiscard]] iterator begin() noexcept { return iterator(mHead); }
    [[nodiscard]] const_iterator begin() const noexcept {
        return const_iterator(mHead);
    }
    [[nodiscard]] const_iterator cbegin() const noexcept {
        return const_iterator(mHead);
    }

    [[nodiscard]] iterator end() noexcept { return iterator(nullptr); }
    [[nodiscard]] const_iterator end() const noexcept {
        return const_iterator(nullptr);
    }
    [[nodiscard]] const_iterator cend() const noexcept {
        return const_iterator(nullptr);
    }

    T &front() { return mHead->data; }
    [[nodiscard]] const T &front() const { return mHead->data; }
    T &back() { return mTail->data; }
    [[nodiscard]] const T &back() const { return mTail->data; }

    void clear() {
        mHead = mTail = nullptr;
        mSize = 0;
    }

    // NOLINTNEXTLINE
    void push_front(Node *node) {
        node->next = mHead;
        mHead = node;
        if (mTail == nullptr) {
            mTail = node;
        }
        ++mSize;
    }

    // NOLINTNEXTLINE
    void push_back(Node *node) {
        if (empty()) {
            mHead = mTail = node;
        } else {
            mTail->next = node;
            mTail = node;
        }
        ++mSize;
    }

    // NOLINTNEXTLINE
    void pop_front() {
        if (empty()) {
            return;
        }
        Node *tmp = mHead;
        mHead = mHead->next;
        if (mHead == nullptr) {
            mTail = nullptr;
        }
        --mSize;
        // NOTE: We don't delete the node, as it's managed by the caller.
    }

    template <typename Alloc, typename... Args>
    // NOLINTNEXTLINE
    void emplace_front(Alloc &alloc, Args &&...args) {
        push_front(alloc.template alloc<Node>(std::forward<Args>(args)...));
    }

    template <typename Alloc, typename... Args>
    // NOLINTNEXTLINE
    void emplace_back(Alloc &alloc, Args &&...args) {
        push_back(alloc->template alloc<Node>(std::forward<Args>(args)...));
    }

  private:
    Node *mHead;
    Node *mTail;
    // NOLINTNEXTLINE
    std::size_t mSize;
};

#endif // LANG_NON_OWNING_LIST_H
