#include <aoc.h>

int main() {
    std::vector<uint64_t> numbers{};
    while (!std::cin.eof()) {
        int tmp{};
        std::cin >> tmp;
        numbers.push_back(tmp);
    }

    uint64_t r1{};
    uint64_t r2{};

    for (size_t i = 0; i < numbers.size(); i++) {
        if (numbers[i] >= 2020) continue;

        for (size_t j = 0; j < numbers.size(); j++) {
            if (numbers[j] >= 2020) continue;

            if ((i != j) && (numbers[i] + numbers[j] == 2020))
                r1 = numbers[i] * numbers[j];

            for (size_t k = 0; k < numbers.size(); k++) {
                if (numbers[k] >= 2020) continue;

                if ((k != j) && (k != i) && (numbers[i] + numbers[j] + numbers[k] ==  2020))
                r2 = numbers[i] * numbers[j] * numbers[k];
            }
        }
    }

    fmt::print("{}\n", r1);
    fmt::print("{}\n", r2);

    return 0;
}
