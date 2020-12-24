#include <aoc.h>

struct Delta {
    int32_t dx{0};
    int32_t dy{0};
};

static constexpr const Delta SE{ 1,  1};
static constexpr const Delta SW{-1,  1};
static constexpr const Delta  W{-2,  0};
static constexpr const Delta NW{-1, -1};
static constexpr const Delta NE{ 1, -1};
static constexpr const Delta  E{ 2,  0};

struct Position {
    union {
        struct {
            int32_t x;
            int32_t y;
        };
        uint64_t _v;
    };

    Position() : x(0), y(0) {}
    Position(int ix, int iy) : x(ix), y(iy) {}
    Position(const Position& other) : _v(other._v) {}
    Position(Position&& other) : _v(other._v) {}
    Position& operator=(const Position& other) {
        _v = other._v;
        return *this;
    }
    Position& operator=(Position&& other) {
        _v = other._v;
        return *this;
    }

    inline Position& apply_moves(const std::vector<Delta>& moves) {
        for (auto m : moves) {
            x += m.dx;
            y += m.dy;
        }
        return *this;
    }

    inline Position operator+(Delta d) const {
        return Position(x + d.dx, y + d.dy);
    }

    inline const std::array<Position, 6> neighbours() const {
        std::array<Position, 6> ret{};
        ret[0] = *this + SE;
        ret[1] = *this + SW;
        ret[2] = *this + W;
        ret[3] = *this + NW;
        ret[4] = *this + NE;
        ret[5] = *this + E;
        return ret;
    }

    inline bool operator<(Position other) const {
        return _v < other._v;
    }
};

static inline bool parse_movements(std::string_view line, std::vector<Delta>& moves) {
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

inline size_t count_black_neighbours(const std::set<Position>& black_tiles, Position p) {
    size_t ret{0};
    for (auto n : p.neighbours()) ret += black_tiles.contains(n);
    return ret;
}

int main() {
    std::set<Position> black_tiles{};
    std::vector<Delta> moves{};
    std::string line{};
    auto minx{std::numeric_limits<int>::max()}, maxx{std::numeric_limits<int>::min()};
    auto miny{std::numeric_limits<int>::max()}, maxy{std::numeric_limits<int>::min()};

    moves.reserve(128);

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

    auto current = std::set<Position>(black_tiles);
    auto next = std::set<Position>();
    for (size_t day = 0; day < 100; day++) {
        auto mx{std::numeric_limits<int>::max()}, Mx{std::numeric_limits<int>::min()};
        auto my{std::numeric_limits<int>::max()}, My{std::numeric_limits<int>::min()};

        for (int y = miny - 1; y < maxy + 2; y++) {
            for (int x = minx - 1; x < maxx + 2; x++) {
                auto p = Position(x, y);
                auto n = count_black_neighbours(current, p);
                auto i = false;
                if (current.contains(p)) {
                    if (n == 1 || n == 2) i = true;
                } else {
                    if (n == 2) i = true;
                }
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
