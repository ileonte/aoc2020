#include <aoc.h>

struct Position {
    ssize_t x{0};
    ssize_t y{0};
};

struct Map {
    ssize_t width{0};
    ssize_t height{0};
    std::vector<char> data{};

    static inline const std::array<Position, 8> directions{{
        {-1, -1}, { 0,  -1}, { 1, -1},
        {-1,  0}, { 1,   0},
        {-1,  1}, { 0,   1}, { 1,  1}
    }};

    Map() = default;
    Map(const Map&) = default;
    Map(Map&& other)
        : width(other.width)
        , height(other.height)
        , data(std::move(other.data))
    {}
    Map& operator=(const Map&) = default;
    Map& operator=(Map&& other) {
        width = other.width;
        height = other.height;
        data = std::move(other.data);
        return *this;
    }

    static inline std::optional<Map> read() {
        std::string line{};
        line.reserve(1024);
        if (!std::getline(std::cin, line)) return std::nullopt;

        Map res{};
        res.width = line.size();
        res.height = 1;
        res.data.reserve(res.width * 1024);
        res.data.insert(res.data.end(), line.begin(), line.end());

        while (std::getline(std::cin, line)) {
            if (line.size() != res.width) return std::nullopt;
            res.data.insert(res.data.end(), line.begin(), line.end());
            res.height += 1;
        }

        return res;
    }

    inline bool position_valid(Position pos) const {
        return
            std::clamp(pos.x, ssize_t(0), width - 1) == pos.x &&
            std::clamp(pos.y, ssize_t(0), height - 1) == pos.y;
    }

    inline Position advance(Position pos) const {
        pos.x += 1;
        if (pos.x >= width) {
            pos.x = 0;
            pos.y += 1;
        }
        return pos;
    }

    template <bool adjacent_only>
    inline const std::vector<Position>& occupied_neighbours(Position pos, char occupied_val, char empty_val) const {
        static std::vector<Position> ret{};
        ret.reserve(8);
        ret.clear();
        for (auto delta : directions) {
            Position p{pos.x + delta.x, pos.y + delta.y};
            if constexpr (!adjacent_only) {
                while (position_valid(p) && (data.at(p.y * width + p.x) == empty_val))
                    p = Position{p.x + delta.x, p.y + delta.y};
            }
            if (position_valid(p) && (data.at(p.y * width + p.x) == occupied_val))
                ret.push_back(p);
        }
        return ret;
    }

    inline std::optional<char*> sample(Position pos) {
        if (position_valid(pos))
            return &data.at(pos.y * width + pos.x);
        return std::nullopt;
    }

    inline std::optional<const char*> sample(Position pos) const {
        if (position_valid(pos))
            return &data.at(pos.y * width + pos.x);
        return std::nullopt;
    }
};

template <bool adjacent_only, size_t occupied_threshhold>
static inline size_t search_for_steady_state(const Map& map) {
    Map current{map};
    Map next{map};
    size_t occupied{0};
    size_t changed{0};

    while (true) {
        Position pos{};
        while (current.position_valid(pos)) {
            auto current_value = *(current.sample(pos).value());
            auto next_value_p = next.sample(pos).value();
            const auto& neighbours = current.occupied_neighbours<adjacent_only>(pos, '#', '.');

            switch (current_value) {
                case 'L': {
                    if (neighbours.size()) {
                        *next_value_p = 'L';
                    } else {
                        changed += 1;
                        occupied += 1;
                        *next_value_p = '#';
                    }
                    break;
                }
                case '#': {
                    if (neighbours.size() < occupied_threshhold) {
                        *next_value_p = '#';
                    } else {
                        changed += 1;
                        occupied -= 1;
                        *next_value_p = 'L';
                    }
                    break;
                }
                default: {
                    break;
                }
            }

            pos = current.advance(pos);
        }

        if (!changed) break;

        changed = 0;
        std::swap(current, next);
    }

    return occupied;
}

int main() {
    Map seat_map{};

    if (auto r = Map::read(); !r) {
        fmt::print(std::cerr, "Failed to load map!\n");
        return 1;
    } else {
        seat_map = r.value();
    }

    fmt::print("{}\n", search_for_steady_state<true, 4>(seat_map));
    fmt::print("{}\n", search_for_steady_state<false, 5>(seat_map));

    return 0;
}
