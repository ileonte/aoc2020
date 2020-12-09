#include <aoc.h>

static inline size_t part1(const std::vector<size_t>& numbers, size_t window_size = 25) {
    size_t base_idx{0};
    size_t test_idx{window_size};
    std::vector<size_t> sums{};

    sums.reserve(1024);

    while (test_idx < numbers.size()) {
        sums.clear();
        for (auto i = base_idx; i < test_idx; i++) {
            for (auto j = i + 1; j < test_idx; j++) {
                sums.push_back(numbers[i] + numbers[j]);
            }
        }
        if (std::find(sums.begin(), sums.end(), numbers[test_idx]) == sums.end())
            return test_idx;

        test_idx += 1;
        base_idx += 1;
    }

    return 0;
}

static inline size_t part2(const std::vector<size_t>& numbers, size_t target_idx) {
    for (size_t window_size = 2; window_size < target_idx; window_size++) {
        size_t base_idx{0};
        size_t top_idx{window_size};
        while (top_idx < target_idx) {
            auto base_it = numbers.begin() + base_idx;
            auto top_it = numbers.begin() + top_idx;
            auto sum = std::accumulate(base_it, top_it, size_t(0));
            if (sum == numbers[target_idx]) {
                auto min = std::min_element(base_it, top_it);
                auto max = std::max_element(base_it, top_it);
                return *min + *max;
            }
            base_idx += 1;
            top_idx += 1;
        }
    }
    return 0;
}

int main() {
    std::string line{};
    std::vector<size_t> numbers{};

    numbers.reserve(1024);
    while (std::getline(std::cin, line)) {
        size_t n{0};
        auto sv = aoc::trim(line);
        auto rc = std::from_chars(sv.begin(), sv.end(), n);
        if (rc.ptr != sv.end()) {
            fmt::print(std::cerr, "Invalid number on line {}: '{}'\n", numbers.size() + 1, line);
            return 1;
        }
        numbers.push_back(n);
    }

    auto target_idx = part1(numbers);
    fmt::print("{}\n", numbers[target_idx]);
    fmt::print("{}\n", part2(numbers, target_idx));

    return 0;
}
