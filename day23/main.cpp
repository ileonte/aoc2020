#include <aoc.h>

using Labels = std::list<int>;
using IteratorCache = std::vector<Labels::iterator>;

static inline Labels::iterator circular_next(Labels& labels, Labels::iterator current) noexcept {
    auto ret = aoc::advance(current);
    if (ret == labels.end()) ret = labels.begin();
    return ret;
}

static inline Labels::const_iterator circular_next(const Labels& labels, Labels::const_iterator current) noexcept {
    auto ret = aoc::advance(current);
    if (ret == labels.end()) ret = labels.begin();
    return ret;
}

static inline Labels::iterator move(Labels& labels, Labels::iterator current, IteratorCache& cache) noexcept {
    auto i1 = circular_next(labels, current);
    auto i2 = circular_next(labels, i1);
    auto i3 = circular_next(labels, i2);
    int next_val = *current - 1;
    if (next_val < 1) next_val = labels.size();
    while (next_val == *i1 || next_val == *i2 || next_val == *i3) {
        next_val -= 1;
        if (next_val < 1) next_val = labels.size();
    }
    auto v1{*i1}, v2{*i2}, v3{*i3};
    auto ins_it = aoc::advance(cache[next_val]);
    labels.erase(i1);
    labels.erase(i2);
    labels.erase(i3);
    cache[v1] = labels.insert(ins_it, v1);
    cache[v2] = labels.insert(ins_it, v2);
    cache[v3] = labels.insert(ins_it, v3);
    return circular_next(labels, current);
}

static inline void part1(Labels labels) noexcept {
    IteratorCache cache(labels.size() + 1);
    for (auto it = labels.begin(); it != labels.end(); it++) {
        cache[*it] = it;
    }

    auto it = labels.begin();
    for (size_t i = 0; i < 100; i++)
        it = move(labels, it, cache);

    std::vector<int> result{};
    result.reserve(labels.size());
    auto sentinel = std::find(labels.begin(), labels.end(), 1);
    for (auto it = circular_next(labels, sentinel); it != sentinel; it = circular_next(labels, it))
        result.push_back(*it);

    fmt::print("{}\n", fmt::join(result, ""));
}

static inline void part2(Labels labels) noexcept {
    IteratorCache cache(labels.size() + 1);
    for (auto it = labels.begin(); it != labels.end(); it++) {
        cache[*it] = it;
    }

    cache.resize(1000001);
    for (int val = labels.size() + 1; labels.size() < 1000000; val++)
        cache[val] = labels.insert(labels.end(), val);

    auto it = labels.begin();
    for (size_t i = 0; i < 10000000; i++)
        it = move(labels, it, cache);

    auto anchor = std::find(labels.begin(), labels.end(), 1);
    auto i1 = circular_next(labels, anchor);
    auto i2 = circular_next(labels, i1);
    auto result{uint64_t(*i1) * uint64_t(*i2)};
    fmt::print("{}\n", result);
}

int main() {
    std::string line{};
    if (!std::getline(std::cin, line)) return 1;
    auto sv = aoc::trim(line);
    if (sv.empty()) return 1;

    Labels labels{};
    for (auto c : sv) labels.push_back(c - '0');

    part1(labels);
    part2(labels);

    return 0;
}
