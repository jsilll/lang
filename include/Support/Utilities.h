#ifndef LANG_UTILITIES_H
#define LANG_UTILITIES_H

namespace lang {

constexpr unsigned getNumDigits(unsigned n) {
    unsigned digits = 0;
    while (n) {
        n /= 10;
        ++digits;
    }
    return digits;
}

} // namespace lang

#endif // LANG_UTILITIES_H