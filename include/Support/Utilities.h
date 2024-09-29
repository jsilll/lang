#ifndef LANG_UTILITIES_H
#define LANG_UTILITIES_H

#include <cstdlib>
#include <type_traits>

namespace lang {

template <typename T> constexpr T getNumDigits(T n) {
    static_assert(std::is_integral_v<T>, "T must be an integral type");

    if constexpr (std::is_signed_v<T>) {
        n = std::abs(n);
    }

    T digits = 0;
    do {
        n /= 10;
        ++digits;
    } while (n);

    return digits;
}

constexpr std::size_t alignUp(std::size_t size, std::size_t align) {
    return (size + align - 1) & ~(align - 1);
}

} // namespace lang

#endif // LANG_UTILITIES_H