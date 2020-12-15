#include <aoc.h>

struct Data {
    uint32_t appearances{0};
    uint32_t prev_turn{0};
};
std::ostream& operator<<(std::ostream& out, Data data) {
    return out << '{' << data.appearances << ", " << data.prev_turn << '}';
}

struct History {
    std::map<uint32_t, Data> history_{};
    Data* last_said{nullptr};

    inline size_t size() const {
        return history_.size();
    }

    inline uint32_t add(uint32_t number, uint32_t turn) {
        auto [it, inserted] = history_.insert({number, {}});
        last_said = &it->second;
        if (inserted) {
             *last_said = Data {
                .appearances = 1,
                .prev_turn = turn,
            };
            return 0;
        }
        std::swap(last_said->prev_turn, turn);
        last_said->appearances += 1;
        return last_said->prev_turn - turn;
    }

    inline uint32_t compute(uint32_t turn, uint32_t target_turn) {
        auto next_number{last_said->appearances == 1 ? uint32_t(0) : turn - 1 - last_said->prev_turn};
        while (turn < target_turn) {
            auto tmp = add(next_number, turn);
            next_number = tmp;
            turn += 1;
        }
        return next_number;
    }
};

int main() {
    History history{};

    std::string line{};
    uint32_t turn{1};
    if (!std::getline(std::cin, line)) return 1;
    for (auto sv : aoc::str_split(line, ',')) {
        auto rc = aoc::from_chars<uint32_t>(aoc::trim(sv));
        if (!rc) return 2;
        history.add(rc.value(), turn);
        turn += 1;
    }

    fmt::print("{}\n", History(history).compute(turn, 2020));
    fmt::print("{}\n", History(history).compute(turn, 30000000));

    return 0;
}
