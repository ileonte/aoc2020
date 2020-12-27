#pragma once

#include "basedefs.h"

class SquareBorders {
public:
    inline SquareBorders() : _v(0), shift(0) {}
    inline SquareBorders(size_t dimension)
        : _v(0), shift(16 - dimension)
    {}
    inline SquareBorders(size_t dimension, uint16_t it, uint16_t il, uint16_t ib, uint16_t ir)
        : t(it), l(il), b(ib), r(ir), shift(16 - dimension)
    {}
    inline SquareBorders(const SquareBorders&) = default;
    inline SquareBorders(SquareBorders&&) = default;
    inline SquareBorders& operator=(const SquareBorders& other) = default;
    inline SquareBorders& operator=(SquareBorders&& other) = default;
    inline operator uint64_t() const {
        return _v;
    }
    inline bool operator<(const SquareBorders& other) const {
        return _v < other._v;
    }
    inline bool operator==(const SquareBorders& other) const {
         return _v == other._v;
    }

    inline auto top() const { return t; }
    inline auto left() const { return l; }
    inline auto bottom() const { return b; }
    inline auto right() const { return r; }
    inline auto value() const { return _v; }
    inline auto dimension() const { return size_t(16) - shift; }
    inline auto width() const { return dimension(); }
    inline auto height() const { return dimension(); }

    inline SquareBorders transformed(Transform transform) const {
        switch (transform) {
            case T_VFlip: return SquareBorders(dimension(), b, reverse_bits(l) >> shift, t, reverse_bits(r) >> shift);
            case T_HFlip: return SquareBorders(dimension(), reverse_bits(t) >> shift, r, reverse_bits(b) >> shift, l);
            case T_Rot90: return SquareBorders(dimension(), r, reverse_bits(t) >> shift, l, reverse_bits(b) >> shift);
            case T_Rot180: return SquareBorders(dimension(), reverse_bits(b) >> shift, reverse_bits(r) >> shift,
                                                reverse_bits(t) >> shift, reverse_bits(l) >> shift);
            case T_Rot270: return SquareBorders(dimension(), reverse_bits(l) >> shift, b, reverse_bits(r) >> shift, t);
            default: return *this;
        }
    }

    template <size_t N>
    inline SquareBorders transformed(const Transform (&transforms)[N]) const {
        return SquareBorders(*this).transform(transforms);
    }

    inline SquareBorders& transform(Transform i_transform) {
        return *this = transformed(i_transform);
    }

    template <size_t N>
    inline SquareBorders& transform(const Transform (&transforms)[N]) {
        for (auto t : transforms) transform(t);
        return *this;
    }

private:
    union {
        struct {
            uint16_t t;
            uint16_t l;
            uint16_t b;
            uint16_t r;
        };
        uint64_t _v;
    };
    size_t shift;
};
namespace std {
    template<> struct hash<SquareBorders> {
        inline size_t operator()(const SquareBorders& m) const {
            return std::hash<uint64_t>()(m);
        }
    };
}

inline std::ostream& operator<<(std::ostream& out, SquareBorders m) {
    fmt::print(out, "{{{}x{}: {:010b},{:010b},{:010b},{:010b}}}",
        m.dimension(), m.dimension(),
        m.top(), m.left(), m.bottom(), m.right());
    return out;
}
