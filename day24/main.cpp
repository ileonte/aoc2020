#include <aoc.h>

struct Delta {
    using int_type = int16_t;
    int_type dx{0};
    int_type dy{0};
};

static constexpr const Delta SE{ 1,  1};
static constexpr const Delta SW{-1,  1};
static constexpr const Delta  W{-2,  0};
static constexpr const Delta NW{-1, -1};
static constexpr const Delta NE{ 1, -1};
static constexpr const Delta  E{ 2,  0};

struct Position {
    using bigint_type = int32_t;
    using int_type = Delta::int_type;

    union {
        struct {
            int_type x;
            int_type y;
        };
        bigint_type _v;
    };

    Position() noexcept : x(0), y(0) {}
    Position(int_type ix, int_type iy)  noexcept : x(ix), y(iy) {}
    Position(bigint_type iv) noexcept : _v(iv) {}
    Position(const Position& other) noexcept : _v(other._v) {}
    Position(Position&& other) noexcept : _v(other._v) {}
    Position& operator=(const Position& other) noexcept {
        _v = other._v;
        return *this;
    }
    Position& operator=(Position&& other) noexcept {
        _v = other._v;
        return *this;
    }

    inline Position& apply_moves(const std::vector<Delta>& moves) noexcept {
        *this = aoc::accumulate(moves, *this, [](Position acc, const Delta& d) -> Position {
            return acc + d;
        });
        return *this;
    }

    inline Position operator+(Delta d) const noexcept {
        return Position(x + d.dx, y + d.dy);
    }

    inline const std::array<Position, 6> neighbours() const noexcept {
        std::array<Position, 6> ret{};
        ret[0] = *this + SE;
        ret[1] = *this + SW;
        ret[2] = *this + W;
        ret[3] = *this + NW;
        ret[4] = *this + NE;
        ret[5] = *this + E;
        return ret;
    }

    inline bool operator==(Position other) const noexcept {
        return _v == other._v;
    }
};
static_assert(sizeof(Position) == sizeof(Position::bigint_type));

namespace std {
    template<> struct hash<Position> {
        inline size_t operator()(Position p) const noexcept {
            return p._v;
        }
    };
}

static inline bool parse_movements(std::string_view line, std::vector<Delta>& moves) noexcept {
    auto* it = line.begin();

    moves.clear();

    while (it < line.end()) {
        switch (*it) {
            case 'e': {
                moves.push_back(E);
                break;
            }
            case 'w': {
                moves.push_back(W);
                break;
            }
            case 's': {
                it++;
                switch (*it) {
                    case 'e': {
                        moves.push_back(SE);
                        break;
                    }
                    case 'w': {
                        moves.push_back(SW);
                        break;
                    }
                    default: goto parse_error;
                }
                break;
            }
            case 'n': {
                it++;
                switch (*it) {
                    case 'e': {
                        moves.push_back(NE);
                        break;
                    }
                    case 'w': {
                        moves.push_back(NW);
                        break;
                    }
                    default: goto parse_error;
                }
                break;
            }
            default: goto parse_error;
        }
        it++;
    }
    return true;

parse_error:
    fmt::print(std::cerr, "Failed to parse '{}': '{}' at position {}\n", line, *it, it - line.begin() + 1);
    return false;
}

int main() {
    std::unordered_set<Position> black_tiles{};
    std::vector<Delta> moves{};
    std::string line{};
    auto [maxx, minx] = aoc::numeric_limits<Delta::int_type>();
    auto [maxy, miny] = aoc::numeric_limits<Delta::int_type>();

    line.reserve(256);
    moves.reserve(256);

    while (std::getline(std::cin, line)) {
        if (!parse_movements(aoc::trim(line), moves)) return 1;
        auto p = Position().apply_moves(moves);
        minx = std::min(minx, p.x); maxx = std::max(maxx, p.x);
        miny = std::min(miny, p.y); maxy = std::max(maxy, p.y);
        auto it = black_tiles.find(p);
        if (it != black_tiles.end()) black_tiles.erase(it);
        else black_tiles.insert(p);
    }
    fmt::print("{}\n", black_tiles.size());

    auto current = std::unordered_set<Position>(black_tiles);
    auto next = std::unordered_set<Position>();
    for (size_t day = 0; day < 100; day++) {
        auto [Mx, mx] = aoc::numeric_limits<Delta::int_type>();
        auto [My, my] = aoc::numeric_limits<Delta::int_type>();

        for (int y = miny - 1; y < maxy + 2; y++) {
            for (int x = minx - 1; x < maxx + 2; x++) {
                auto p = Position(x, y);
                auto n = aoc::accumulate(p.neighbours(), 0, [&current](int acc, const Position& p) -> int {
                    return acc + current.contains(p);
                });
                auto is_black = current.contains(p);
                auto i = ((is_black && (n == 1 || n == 2)) || (!is_black && (n == 2)));
                if (i) {
                    next.insert(p);
                    mx = std::min(mx, p.x); Mx = std::max(Mx, p.x);
                    my = std::min(my, p.y); My = std::max(My, p.y);
                }
            }
        }

        std::swap(current, next);
        next.clear();

        minx = mx; maxx = Mx;
        miny = my; maxy = My;
    }
    fmt::print("{}\n", current.size());

    return 0;
}
