#include <aoc.h>

struct Policy {
    size_t min = 0;
    size_t max = 0;
    char ch = 0;

    static inline std::optional<Policy> parse(std::string_view sv) {
        static std::regex policy_re(
            "^\\s*(\\d+)-(\\d+)\\s+(.)\\s*$",
            std::regex_constants::ECMAScript | std::regex_constants::optimize);

        std::cmatch match{};
        if (!std::regex_match(sv.begin(), sv.end(), match, policy_re))
           return std::nullopt;

        Policy ret{};
        std::from_chars(match[1].first, match[1].second, ret.min);
        std::from_chars(match[2].first, match[2].second, ret.max);
        ret.ch = *match[3].first;

        return ret;
    }

    inline size_t matches_p1(std::string_view passwd) const {
        auto count = std::accumulate(passwd.begin(), passwd.end(), size_t(0), [this](size_t acc, char ch) -> size_t {
            return acc + (ch == this->ch);
        });
        return std::clamp(count, min, max) == count;
    }

    inline size_t matches_p2(std::string_view passwd) const {
        auto ch1 = min <= passwd.size() ? passwd[min - 1] : char(0);
        auto ch2 = max <= passwd.size() ? passwd[max - 1] : char(0);
        return ((ch == ch1) && (ch != ch2)) || ((ch != ch1) && (ch == ch2));
    }
};
std::ostream& operator<<(std::ostream& out, const Policy& p) {
    return out << '{' << p.min << "-" << p.max << " " << p.ch << '}';
}

int main() {
    size_t count_p1 = 0;
    size_t count_p2 = 0;

    std::string line{};
    line.reserve(1024);

    std::vector<std::string_view> pieces{};
    pieces.reserve(2);

    while (std::getline(std::cin, line)) {
        pieces = aoc::str_split(aoc::trim(line), ':');
        if (pieces.size() != 2) continue;

        if (auto r = Policy::parse(pieces[0]); r.has_value()) {
            auto policy = r.value();
            auto passwd = aoc::trim(pieces[1]);
            count_p1 += policy.matches_p1(passwd);
            count_p2 += policy.matches_p2(passwd);
        }
    }

    fmt::print("{}\n", count_p1);
    fmt::print("{}\n", count_p2);

    return 0;
}
