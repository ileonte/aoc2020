#include <aoc.h>

static inline size_t set_bit(size_t n, size_t shift, bool on_off) {
    size_t mask = 1 << shift;
    return on_off ?  (n | mask) : (n & ~mask);
}

template <char upper_half>
static inline size_t convert(std::string_view sv) {
    size_t ret{0};
    for (size_t i = 0; i < sv.size(); i++)
        ret = set_bit(ret, sv.size() - i - 1, sv.at(i) == upper_half);
    return ret;
}

int main() {
    std::string line{};
    size_t max{0};
    size_t min{std::numeric_limits<size_t>::max()};

    constexpr const size_t max_seat_id = 8 * 127 + 7;
    std::array<unsigned char, max_seat_id + 1> seat_map{};
    seat_map.fill(0);

    while (std::getline(std::cin, line)) {
        size_t row = convert<'B'>(aoc::substr(line, 0, 7));
        size_t col = convert<'R'>(aoc::substr(line, 7));
        size_t seat_id = 8 * row + col;
        max = std::max(max, seat_id);
        min = std::min(min, seat_id);
        seat_map.at(seat_id) = 1;
    }

    auto it = std::find(seat_map.begin() + min, seat_map.begin() + max, 0);

    fmt::print("{}\n", max);
    fmt::print("{}\n", it - seat_map.begin());

    return 0;
}
