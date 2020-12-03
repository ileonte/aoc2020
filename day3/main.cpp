#include <aoc.h>

struct Slope {
    size_t dx{0};
    size_t dy{0};
};

struct Position {
    size_t world_x{0};
    size_t world_y{0};
    size_t sample_x{0};
    size_t sample_y{0};
};

struct Map {
    size_t width{0};
    size_t height{0};
    std::vector<char> data{};

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

    inline Position advance(Position pos, Slope slope) const {
        return Position {
            .world_x = pos.world_x + slope.dx,
            .world_y = pos.world_y + slope.dy,
            .sample_x = (pos.world_x + slope.dx) % width,
            .sample_y = pos.world_y + slope.dy
        };
    }

    inline std::optional<char> sample(Position pos) const {
        if (pos.sample_x < width && pos.sample_y < height)
            return data.at(pos.sample_y * width + pos.sample_x);
        return std::nullopt;
    }

    inline size_t trees_on_slope(Slope slope) {
        Position pos{};
        size_t trees{0};
        while (auto r = sample(pos)) {
            trees += (r.value() == '#');
            pos = advance(pos, slope);
        }
        return trees;
    }
};

int main() {
    Map map{};
    if (auto r = Map::read(); !r) {
        fmt::print(std::cerr, "Failed to read the map!\n");
        return 1;
    } else {
        map = std::move(r.value());
    }

    size_t s11 = map.trees_on_slope({1, 1});
    size_t s31 = map.trees_on_slope({3, 1});
    size_t s51 = map.trees_on_slope({5, 1});
    size_t s71 = map.trees_on_slope({7, 1});
    size_t s12 = map.trees_on_slope({1, 2});

    fmt::print("{}\n", s31);
    fmt::print("{}\n", s11 * s31 * s51 * s71 * s12);

    return 0;
}
