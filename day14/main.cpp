#include <aoc.h>

struct Computer {
    std::unordered_map<size_t, size_t> memory_1{};
    std::unordered_map<size_t, size_t> memory_2{};

    inline bool read_input() {
        static std::regex mask_re("^mask\\s*=\\s*([X01]{36})$");
        static std::regex mem_re("^mem\\[(\\d+)\\]\\s*=\\s*(\\d+)$");

        std::string mask_s{};
        std::string line{};
        size_t line_no{0};

        mask_s.reserve(128);
        line.reserve(128);

        while (std::getline(std::cin, line)) {
            line_no += 1;
            auto sv = aoc::trim(line);
            std::cmatch match{};
            if (std::regex_match(sv.begin(), sv.end(), match, mask_re)) {
                mask_s = aoc::to_sv(match[1]);
            } else if (std::regex_match(sv.begin(), sv.end(), match, mem_re)) {
                size_t a = aoc::from_chars<size_t>(aoc::to_sv(match[1])).value();
                size_t v = aoc::from_chars<size_t>(aoc::to_sv(match[2])).value();
                part1_set(mask_s, a, v);
                part2_set(mask_s, a, v);
            } else {
                fmt::print(std::cerr, "Invalid format on line {}: '{}'\n", line_no, line);
                return false;
            }
        }

        return true;
    }

    inline void part1_set(std::string_view msv, size_t a, size_t v) {
        size_t mask_1{0};
        size_t mask_0{0x0fffffffff};
        for (size_t i = 0; i < 36; i++) {
            switch (msv[35 - i]) {
                case '1': {
                    mask_1 = aoc::set_bit(mask_1, i, true);
                    break;
                }
                case '0': {
                    mask_0 = aoc::set_bit(mask_0, i, false);
                    break;
                }
                default: continue;
            }
        }
        memory_1[a] = ((v | mask_1) & mask_0);
    }

    inline void part2_set(std::string_view msv, size_t a, size_t v, size_t start = 0) {
        for (size_t it = start; it < msv.size(); it++) {
            auto bit = msv.size() - 1 - it;
            switch (msv[it]) {
                case '1': {
                    a = aoc::set_bit(a, bit, true);
                    break;
                }
                case 'X': {
                    part2_set(msv, aoc::set_bit(a, bit, true),  v, it + 1);
                    part2_set(msv, aoc::set_bit(a, bit, false), v, it + 1);
                    return;
                }
                default: continue;
            }
        }
        memory_2[a] = v;
    }

    inline size_t part1() const {
        return std::accumulate(
            memory_1.begin(), memory_1.end(),
            size_t(0),
            [](size_t acc, const std::pair<size_t, size_t>& it) -> size_t {
                return acc + it.second;
            }
        );
    }

    inline size_t part2() const {
        return std::accumulate(
            memory_2.begin(), memory_2.end(),
            size_t(0),
            [](size_t acc, const std::pair<size_t, size_t>& it) -> size_t {
                return acc + it.second;
            }
        );
    }
};

int main() {
    Computer comp{};
    if (!comp.read_input()) return 1;

    fmt::print("{}\n", comp.part1());
    fmt::print("{}\n", comp.part2());

    return 0;
}
