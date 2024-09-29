#ifndef LANG_UTILITIES_H
#define LANG_UTILITIES_H

#include <cstdlib>
#include <type_traits>

namespace lang {

template <typename T>
int numDigits(T number)
{
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    int digits = 0;
    if constexpr (std::is_signed_v<T>) {
        if (number < 0) digits = 1;
    }
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}


} // namespace lang

#endif // LANG_UTILITIES_H