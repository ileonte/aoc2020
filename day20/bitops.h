#pragma once

#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
constexpr inline T reverse_bits(T v) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
    auto s = sizeof(v) * 8;
    T mask = ~0;
    while ((s >>= 1) > 0) {
        mask ^= (mask << s);
        v = ((v >> s) & mask) | ((v << s) & ~mask);
    }
    return v;
}
