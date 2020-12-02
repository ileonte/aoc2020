#include <aoc.h>

struct Password {
    std::array<size_t, 256> chars{};

    static inline Password parse(std::string_view sv) {
        sv = aoc::trim(sv);

        Password ret{};
        std::fill(ret.chars.begin(), ret.chars.end(), 0);
        for (auto ch : sv)
            ret.chars[static_cast<size_t>(ch)] += 1;
        return ret;
    }
};
std::ostream& operator<<(std::ostream& out, const Password& p) {
    out << '{';
    bool first_print = true;
    for (size_t ch = 32; ch < 128; ch++) {
        if (!p.chars[ch]) continue;
        if (first_print)
            first_print = false;
        else
            out << ',' << ' ';
        out << static_cast<char>(ch) << ':' << ' ' << p.chars[ch];
    }
    out << '}';
    return out;
}

struct Policy {
    size_t min = 0;
    size_t max = 0;
    size_t ch = 0;

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
        ret.ch = static_cast<size_t>(*match[3].first);

        return ret;
    }

    inline size_t matches_p1(const Password& passwd) const {
        return passwd.chars[ch] == std::clamp(passwd.chars[ch], min, max);
    }

    inline size_t matches_p2(std::string_view passwd) const {
        auto ch1 = min <= passwd.size() ? static_cast<size_t>(passwd[min - 1]) : 0;
        auto ch2 = max <= passwd.size() ? static_cast<size_t>(passwd[max - 1]) : 0;
        return ((ch == ch1) && (ch != ch2)) || ((ch != ch1) && (ch == ch2));
    }
};
std::ostream& operator<<(std::ostream& out, const Policy& p) {
    return out << '{' << p.min << "-" << p.max << " " << static_cast<char>(p.ch) << '}';
}

int main() {
    static std::regex line_re(
        "^(.+):\\s*(.+)$",
        std::regex_constants::ECMAScript | std::regex_constants::optimize);

    size_t count_p1 = 0;
    size_t count_p2 = 0;

    std::string line{};
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        auto line_sv = aoc::trim(line);
        std::cmatch match{};
        if (!std::regex_match(line_sv.begin(), line_sv.end(), match, line_re))
            continue;

        if (auto r = Policy::parse(aoc::to_sv(match[1])); r.has_value()) {
            auto passwd_sv = aoc::to_sv(match[2]);
            auto policy = r.value();
            auto passwd = Password::parse(passwd_sv);
            count_p1 += policy.matches_p1(passwd);
            count_p2 += policy.matches_p2(passwd_sv);
        }
    }

    fmt::print("{}\n", count_p1);
    fmt::print("{}\n", count_p2);

    return 0;
}
