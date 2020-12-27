#pragma once

#include <aoc.h>
#include "bitops.h"

struct Position {
    size_t x{0};
    size_t y{0};
};

enum Transform {
    T_Ident,
    T_Rot90,
    T_Rot180,
    T_Rot270,
    T_HFlip,
    T_VFlip,
};
inline std::ostream& operator<<(std::ostream& out, Transform t) {
    switch (t) {
        case T_Ident: return out << "Ident";
        case T_Rot90: return out << "Rot90";
        case T_Rot180: return out << "Rot180";
        case T_Rot270: return out << "Rot270";
        case T_HFlip: return out << "HFlip";
        case T_VFlip: return out << "VFlip";
        default: return out;
    }
}

inline const Transform g_transforms[][2] {
    {T_Ident,  T_Ident},
    {T_HFlip,  T_Ident},
    {T_Rot90,  T_Ident},
    {T_Rot90,  T_HFlip},
    {T_Rot180, T_Ident},
    {T_Rot180, T_HFlip},
    {T_Rot270, T_Ident},
    {T_Rot270, T_HFlip},
};

inline void draw_image(std::ostream& out, const std::vector<char>& data, size_t stride) {
    size_t pos{0};
    while (pos < data.size()) {
        auto sv = std::string_view(data.data() + pos, stride);
        out << sv << "\n";
        pos += stride;
    }
}
