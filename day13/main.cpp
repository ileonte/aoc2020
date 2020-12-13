#include <aoc.h>

struct Pair {
    size_t i{0};
    size_t v{0};
};

int main() {
    size_t timestamp{};
    std::vector<Pair> ids{};
    std::string line{};

    if (!std::getline(std::cin, line)) return 1;
    auto sv = aoc::trim(line);
    auto rc = std::from_chars(sv.begin(), sv.end(), timestamp);
    if (rc.ptr != sv.end()) {
        fmt::print(std::cerr, "Failed to parse timestamp: '{}'\n", line);
        return 1;
    }

    if (!std::getline(std::cin, line)) return 1;
    size_t idx{0};
    for (auto it : aoc::str_split(aoc::trim(line), ',')) {
        size_t tmp{0};
        rc = std::from_chars(it.begin(), it.end(), tmp);
        if (rc.ptr == it.end()) ids.push_back({idx, tmp});
        idx += 1;
    }

    auto part1_id{std::numeric_limits<size_t>::max()};
    auto part1_dif{std::numeric_limits<size_t>::max()};
    for (auto [idx, id] : ids) {
        auto div = timestamp / id;
        auto dif = (div + 1) * id - timestamp;
        if ((dif < part1_dif) || (dif == part1_dif && id < part1_id)) {
            part1_id = id;
            part1_dif = dif;
        }
    }
    fmt::print("{}\n", part1_id * part1_dif);

    auto min = *std::min_element(ids.begin(), ids.end(), [](Pair p1, Pair p2) -> bool { return p1.v < p2.v; });
    auto max = *std::max_element(ids.begin(), ids.end(), [](Pair p1, Pair p2) -> bool { return p1.v < p2.v; });
    fmt::print("{} {}\n", min.v, max.v);

    return 0;
}
