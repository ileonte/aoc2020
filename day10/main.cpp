#include <aoc.h>

static inline void recurse(const size_t* begin, const size_t* end, const size_t* current, std::map<size_t, size_t>& counts) {
    size_t paths_forward{0};
    for (auto it = current + 1; it <= std::min(end, current + 3); it++) {
        if (*it - *current > 3) break;
        paths_forward += counts[*it];
    }
    counts.insert({*current, paths_forward});
    if (current > begin) recurse(begin, end, current - 1, counts);
}

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
    std::map<size_t, size_t> counts{};
    auto begin = &joltages.front();
    auto end = &joltages.back();
    counts.insert({*end, size_t(0)});
    counts.insert({*(end - 1), size_t(1)});
    recurse(begin, end, end - 2, counts);
    fmt::print("{}\n", counts[0]);

    return 0;
}
