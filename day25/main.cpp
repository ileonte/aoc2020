#include <aoc.h>

static inline uint64_t determine_loop_size(uint64_t public_key, uint64_t subject_number) noexcept {
    uint64_t value{1};
    uint64_t loop_size{1};
    for (loop_size = 0; value != public_key; loop_size++) {
        value = (value * subject_number) % 20201227llu;
    }
    return loop_size;
}

static inline uint64_t transform(uint64_t subject_number, uint64_t loop_size) noexcept {
    uint64_t value{1};
    for (uint64_t iter = 0; iter < loop_size; iter++) {
        value = (value * subject_number) % 20201227llu;
    }
    return value;
}

int main() {
    uint64_t pub_key_card{0};
    uint64_t pub_key_door{0};

    std::string line{};
    if (!std::getline(std::cin, line)) return 1;
    if (!aoc::from_chars(aoc::trim(line), pub_key_card)) return 1;
    if (!std::getline(std::cin, line)) return 1;
    if (!aoc::from_chars(aoc::trim(line), pub_key_door)) return 1;

    auto loop_size_card = determine_loop_size(pub_key_card, 7);
    auto loop_size_door = determine_loop_size(pub_key_door, 7);

    auto encryption_key = transform(pub_key_door, loop_size_card);
    fmt::print("{}\n", encryption_key);

    return 0;
}
