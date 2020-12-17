#include <aoc.h>

struct Position {
    using val_type = uint64_t;
    using comp_type = int16_t;

    union {
        val_type val;
        struct {
            comp_type x;
            comp_type y;
            comp_type z;
            comp_type w;
        };
    };

    Position() : x(0), y(0), z(0), w(0) {}
    Position(comp_type ix, comp_type iy, comp_type iz, comp_type iw = 0) : x(ix), y(iy), z(iz), w(iw) {}
    Position(const Position&) = default;
    Position(Position&&) = default;
    Position& operator=(const Position&) = default;

    inline bool operator<(Position other) const { return val < other.val; }
    inline bool operator==(Position other) { return val == other.val; }

    template <bool four_D>
    inline void neighbours(std::vector<Position>& neigh) const {
        neigh.clear();
        if constexpr (four_D) {
            for (comp_type dw = -1; dw < 2; dw++) {
                for (comp_type dz = -1; dz < 2; dz++) {
                    for (comp_type dy = -1; dy < 2; dy++) {
                        for (comp_type dx = -1; dx < 2; dx++) {
                            if (!dx && !dy && !dz && !dw) continue;
                            neigh.emplace_back(x + dx, y + dy, z + dz, w + dw);
                        }
                    }
                }
            }
        } else {
            for (comp_type dz = -1; dz < 2; dz++) {
                for (comp_type dy = -1; dy < 2; dy++) {
                    for (comp_type dx = -1; dx < 2; dx++) {
                        if (!dx && !dy && !dz) continue;
                        neigh.emplace_back(x + dx, y + dy, z + dz, 0);
                    }
                }
            }
        }
    }
};
std::ostream& operator<<(std::ostream& out, Position pos) {
    fmt::print(out, "{{{},{},{},{} {:#018x}}}", pos.x, pos.y, pos.z, pos.w, pos.val);
    return out;
}

template <bool four_D>
static inline void step(
    std::set<Position>& map,
    Position::comp_type& min_x, Position::comp_type& max_x,
    Position::comp_type& min_y, Position::comp_type& max_y,
    Position::comp_type& min_z, Position::comp_type& max_z,
    Position::comp_type& min_w, Position::comp_type& max_w)
{
    auto neigh_f = &Position::neighbours<four_D>;
    std::vector<Position> neighbours{};

    neighbours.reserve(80);

    std::set<Position> ret{};
    for (auto w = min_w - 1; w <= max_w + 1; w++) {
        for (auto z = min_z - 1; z <= max_z + 1; z++) {
            for (auto y = min_y - 1; y <= max_y + 1; y++) {
                for (auto x = min_x - 1; x <= max_x + 1; x++) {
                    auto pos = Position(x, y, z, w);
                    auto active = map.contains(pos);
                    int count{0};
                    (pos.*neigh_f)(neighbours);
                    for (auto n : neighbours) count += map.contains(n);
                    if ((active && (count == 2 || count == 3)) || (!active && count ==3))
                        ret.insert(pos);
                }
            }
        }
    }

    map = std::move(ret);
    min_x -= 1; max_x += 1;
    min_y -= 1; max_y += 1;
    min_z -= 1; max_z += 1;
    min_w -= 1; max_w += 1;
}

int main() {
    std::set<Position> map{};

    std::string line{};
    Position::comp_type x{0}, y{0}, z{0};
    Position::comp_type min_x{0}, min_y{0};
    Position::comp_type max_x{0}, max_y{0};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        for (x = 0; x < sv.size(); x++) {
            if (sv[x] != '#') continue;
            map.insert(Position(x, y, z));
            max_x = std::max(x, max_x);
        }
        y += 1;
        max_y = y;
    }

    {
        auto tmp_map = map;
        Position::comp_type x1{min_x}, x2{max_x}, y1{min_y}, y2{max_y}, z1{0}, z2{0};
        for (size_t i = 0; i < 6; i++) {
            Position::comp_type w1{1}, w2{-1};
            step<false>(tmp_map, x1, x2, y1, y2, z1, z2, w1, w2);
        }
        fmt::print("{}\n", tmp_map.size());
    }

    {
        auto tmp_map = map;
        Position::comp_type  x1{min_x}, x2{max_x}, y1{min_y}, y2{max_y}, z1{0}, z2{0}, w1{0}, w2{0};
        for (size_t i = 0; i < 6; i++)
            step<true>(tmp_map, x1, x2, y1, y2, z1, z2, w1, w2);
        fmt::print("{}\n", tmp_map.size());
    }

    return 0;
}
