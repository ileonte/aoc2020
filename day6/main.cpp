#include <aoc.h>

int main() {
    std::string line{};
    std::array<size_t, 26> answers{};
    size_t p1{0};
    size_t p2{0};
    size_t people_in_group{0};

    answers.fill(0);

    const auto update_sums = [&]() {
        p1 += std::accumulate(answers.begin(), answers.end(), size_t(0), [](size_t sum, size_t v) {
            return sum + (v != 0);
        });
        p2 += std::accumulate(answers.begin(), answers.end(), size_t(0), [people_in_group](size_t sum, size_t v) {
            return sum + (v == people_in_group);
        });
    };

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            update_sums();
            answers.fill(0);
            people_in_group = 0;
            continue;
        }
        for (auto ch : line) {
            answers[ch - 'a'] += 1;
        }
        people_in_group += 1;
    }
    if (!line.empty())
        update_sums();

    fmt::print("{}\n", p1);
    fmt::print("{}\n", p2);

    return 0;
}
