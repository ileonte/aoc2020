#include <aoc.h>

static inline bool parse_rules(const std::vector<std::string>& raw_rules, std::map<std::string_view, std::string_view>& rules) {
    static const std::regex complex_re("([^:\\s]+)\\s*:\\s*([^\"]+)");
    static const std::regex simple_re("([^:\\s]+)\\s*:\\s*(\"[^\"]+\")");
    for (auto& raw : raw_rules) {
        auto sv = aoc::to_sv(raw);
        if (auto m = std::cmatch{}; std::regex_match(sv.begin(), sv.end(), m, complex_re))
            rules.insert_or_assign(aoc::to_sv(m[1]), aoc::to_sv(m[2]));
        else if (auto m = std::cmatch{}; std::regex_match(sv.begin(), sv.end(), m, simple_re))
            rules.insert_or_assign(aoc::to_sv(m[1]), aoc::to_sv(m[2]));
        else {
            fmt::print(std::cerr, "Failed to parse rule: '{}'\n", raw);
            return false;
        }
    }
    return true;
}

static inline std::optional<std::string> rule_to_re(const std::map<std::string_view, std::string_view>& rules, std::string_view id, size_t level = 0) {
    static std::map<std::string_view, std::string> resolved_rules{};

    auto it = rules.find(id);
    if (it == rules.end()) return std::nullopt;

    std::string ret{};
    auto v = it->second;
    if (v.empty()) return ret;

    if (v[0] == '"') {
        v.remove_prefix(1);
        v.remove_suffix(1);
        ret = std::string(v);
    } else {
        static const std::regex space_re("\\s+");
        static const std::regex pipe_re("\\s*\\|\\s*");
        auto opts = aoc::str_split(v, pipe_re);
        if (opts.size() > 1) ret.append("(?:");
        for (size_t i = 0; i < opts.size(); i++) {
            auto opt = opts[i];
            auto ids = aoc::str_split(opt, space_re);
            for (auto _id : ids) {
                bool recurse{_id.ends_with('+')};
                if (recurse) _id.remove_suffix(1);
                if (auto rit = resolved_rules.find(_id); rit != resolved_rules.end()) {
                    ret.append(rit->second);
                } else {
                    auto r = rule_to_re(rules, _id, level + 1);
                    if (!r) return std::nullopt;
                    ret.append(r.value());
                }
                if (recurse) ret.append("+");
            }
            if (i < opts.size() - 1) ret.append("|");
        }
        if (opts.size() > 1) ret.append(")");
    }

    if (!level) {
        resolved_rules.clear();
        ret = fmt::format("^{}$", ret);
    } else {
        resolved_rules.insert({id, ret});
    }

    return ret;
}

int main() {
    std::vector<std::string> raw_rules{};
    std::map<std::string_view, std::string_view> rules{};
    std::string line{};
    std::regex part1_re{};
    std::regex part2_re{};

    while (std::getline(std::cin, line)) {
        if (aoc::trim(line).empty()) break;
        raw_rules.push_back(line);
    }

    if (!parse_rules(raw_rules, rules)) return 1;
    if (auto r = rule_to_re(rules, "0"); r) {
        part1_re = std::regex(r.value());
    } else {
        fmt::print(std::cerr, "Failed to generate string for part 1\n");
        return 1;
    }

    rules.clear();
    raw_rules.push_back("8: 42+");
    raw_rules.push_back("11: 42 31 | 42 9000 31");
    for (size_t id = 9000; id < 9004; id++)
        raw_rules.push_back(fmt::format("{}: 42 31 | 42 {} 31", id, id + 1));
    raw_rules.push_back("9004: 42 31");
    if (!parse_rules(raw_rules, rules)) return 1;
    if (auto r = rule_to_re(rules, "0"); r) {
        part2_re = std::regex(r.value());
    } else {
        fmt::print(std::cerr, "Failed to generate string for part 2\n");
        return 1;
    }

    size_t part1{0};
    size_t part2{0};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        if (auto m = std::cmatch{}; std::regex_match(sv.begin(), sv.end(), m, part1_re))
            part1 += 1;
        if (auto m = std::cmatch{}; std::regex_match(sv.begin(), sv.end(), m, part2_re))
            part2 += 1;
    }
    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}
