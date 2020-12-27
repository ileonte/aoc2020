#include <aoc.h>
#include <fstream>
#include <cassert>

#include "basedefs.h"
#include "bitops.h"
#include "borders.h"
#include "tile.h"
#include "image.h"

static inline std::vector<Position> match_pattern(const std::vector<std::string_view>& pattern, const std::vector<char>& img, size_t stride) {
    std::vector<Position> ret{};
    for (size_t line = 0; line < img.size() / stride - pattern.size(); line++) {
        for (size_t col = 0; col < stride - pattern[0].size(); col++) {
            auto match{true};
            for (size_t py = 0; py < pattern.size(); py++) {
                for (size_t px = 0; px < pattern[0].size(); px++) {
                    if (pattern[py][px] != '#') continue;
                    if (img[(line + py) * stride + col + px] != '#') {
                        match = false;
                        break;
                    }
                }
                if (!match) break;
            }
            if (match) ret.push_back({col, line});
        }
    }
    return ret;
}

static inline void color_pattern(std::vector<char>& img, size_t stride, const std::vector<std::string_view>& pattern, const std::vector<Position>& matches) {
    for (auto pos : matches) {
        for (size_t py = 0; py < pattern.size(); py++) {
            for (size_t px = 0; px < pattern[0].size(); px++) {
                if (pattern[py][px] != '#') continue;
                img[(pos.y + py) * stride + pos.x + px] = 'O';
            }
        }
    }
}

static inline std::istream& open_input(int argc, char* argv[]) {
    static std::fstream file{};
    if (argc != 2) return std::cin;
    file.open(argv[1], std::ios_base::in);
    if (!file.is_open()) {
        fmt::print(std::cerr, "Failed to open '{}'\n", argv[1]);
        std::exit(1);
    }
    return file;
}

int main(int argc, char* argv[]) {
    SquareImage image{};
    std::unordered_map<size_t, SquareTile> tiles{};

    std::istream& in = open_input(argc, argv);
    std::optional<SquareTile> maybe_tile{std::nullopt};
    while (maybe_tile = SquareTile::read(in)) {
        if (!image.add_tile(std::move(maybe_tile.value()))) return 1;
    }
    if (!image.assemble()) return 1;

    auto corners = image.corners();
    if (corners.empty()) return 1;
    auto unique_corners = std::set<size_t>(corners[0].begin(), corners[0].end());
    for (auto& o : corners) {
        auto s = std::set<size_t>(o.begin(), o.end());
        if (unique_corners != s) return 1;
    }
    auto part1 = std::accumulate(unique_corners.begin(), unique_corners.end(), size_t(1), [](size_t acc, size_t id) -> size_t {
        return acc * id;
    });
    fmt::print("{}\n", part1);

    std::vector<std::string_view> pattern{{
        "                  # ",
        "#    ##    ##    ###",
        " #  #  #  #  #  #   ",
    }};
    std::vector<char> img{};
    size_t stride{0};
    for (auto& o : image.orderings()) {
        image.draw_ordering(o, img, stride);
        auto matches = match_pattern(pattern, img, stride);
        if (!matches.size()) continue;

        color_pattern(img, stride, pattern, matches);
        auto part2 = aoc::accumulate(img, size_t(0), [](size_t acc, char v) -> size_t {
            return acc + (v == '#');
        });
        fmt::print("{}\n", part2);
        break;
    }

    return 0;
}
