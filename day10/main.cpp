#include <aoc.h>

int main() {
    std::string line{};
    std::vector<size_t> joltages{};

    joltages.reserve(1024);
    joltages.push_back(0);

    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        size_t v{0};

        if (sv.empty()) continue;
        auto rc = std::from_chars(sv.begin(), sv.end(), v);
        if (rc.ptr != sv.end()) {
            fmt::print(std::cerr, "Invalid number on line {}: '{}'\n", joltages.size() + 1, line);
            return 1;
        }
        joltages.push_back(v);
    }
    std::sort(joltages.begin(), joltages.end());
    joltages.push_back(joltages.back() + 3);

    // part 1
    size_t diff_1{0};
    size_t diff_3{0};
    for (size_t i = 1; i < joltages.size(); i++) {
        size_t diff = joltages[i] - joltages[i - 1];
        diff_1 += (diff == 1);
        diff_3 += (diff == 3);
    }
    fmt::print("{}\n", diff_1 * diff_3);

    // part2
    std::vector<size_t> paths_forward(joltages.size(), size_t(0));
    paths_forward[joltages.size() - 2] = 1;
    for (size_t current_idx = joltages.size() - 3; current_idx < joltages.size(); current_idx--) {
        for (size_t it = current_idx + 1; it < joltages.size(); it++) {
            if (joltages[it] - joltages[current_idx] > 3) break;
            paths_forward[current_idx] += paths_forward[it];
        }
    }
    fmt::print("{}\n", paths_forward[0]);

    return 0;
}
