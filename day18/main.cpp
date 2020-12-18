#include <aoc.h>

inline std::string_view extract_term(std::string_view& expr) {
    auto* start = expr.begin();
    auto* end = expr.end();
    while ((start < end) && std::isspace(*start)) start++;
    if (start >= end) {
        expr = std::string_view(end, end);
        return expr;
    }
    if (std::isdigit(*start)) {
        auto* tmp = start;
        while ((tmp < end) &&std::isdigit(*tmp)) tmp++;
        expr = std::string_view(tmp, end);
        return std::string_view(start, tmp);
    } else if (*start == '(') {
        size_t count = 1;
        auto* tmp = start + 1;
        while (tmp < end) {
            switch (*tmp) {
                case '(': {
                    count += 1;
                    break;
                }
                case ')': {
                    if (!count) {
                        expr = std::string_view(end, end);
                        return expr;
                    }
                    count -= 1;
                    if (!count) {
                        expr = std::string_view(tmp + 1, end);
                        return std::string_view(start, tmp + 1);
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            tmp++;
        }
    }
    expr = std::string_view(end, end);
    return expr;
}

inline std::string_view extract_op(std::string_view& expr) {
    auto* start = expr.begin();
    auto* end = expr.end();
    while ((start < end) && std::isspace(*start)) start++;
    if (start >= end) {
        expr = std::string_view(end, end);
        return expr;
    }
    if (*start == '+' || *start == '*') {
        expr = std::string_view(start + 1, end);
        return std::string_view(start, start + 1);
    }
    expr = std::string_view(end, end);
    return expr;
}

inline void split(std::string_view expr, std::vector<std::string_view>& parts) {
    parts.clear();
    auto term = extract_term(expr);
    parts.push_back(term);
    while (!expr.empty()) {
        parts.push_back(extract_op(expr));
        parts.push_back(extract_term(expr));
    }
}

inline size_t p1_eval(const std::vector<std::string_view>& parts) {
    static const auto do_eval = [](std::string_view sv) -> size_t {
        if (sv[0] == '(') {
            std::vector<std::string_view> parts{};
            sv.remove_prefix(1);
            sv.remove_suffix(1);
            split(sv, parts);
            return p1_eval(parts);
        }
        return aoc::from_chars<size_t>(sv).value();
    };
    auto ret{do_eval(parts[0])};
    for (size_t i = 1; i < parts.size() - 1; i += 2) {
        auto rhs{do_eval(parts[i + 1])};
        switch (parts[i][0]) {
            case '+': {
                ret += rhs;
                break;
            }
            case '*': {
                ret *= rhs;
                break;
            }
        }
    }
    return ret;
}

inline size_t p2_eval(const std::vector<std::string_view>& parts) {
    static const auto do_eval = [](std::string_view sv) -> size_t {
        if (sv[0] == '(') {
            std::vector<std::string_view> parts{};
            sv.remove_prefix(1);
            sv.remove_suffix(1);
            split(sv, parts);
            return p2_eval(parts);
        }
        return aoc::from_chars<size_t>(sv).value();
    };
    std::vector<size_t> values{};
    for (size_t i = 0; i < parts.size(); i += 2) {
        values.push_back(do_eval(parts[i]));
        values.push_back(1);
    }
    for (size_t i = 1; i < parts.size(); i += 2) {
        if (parts[i][0] != '+') continue;
        values[i + 1] += values[i - 1];
        values[i - 1] = 1;
    }
    return std::accumulate(values.begin(), values.end(), size_t(1), [](size_t acc, size_t v) -> size_t {
        return acc * v;
    });
}

int main() {
    std::vector<std::string_view> parts{};
    std::string line{};
    size_t part1{0};
    size_t part2{0};

    parts.reserve(128);

    while (std::getline(std::cin, line)) {
        split(aoc::trim(line), parts);
        part1 += p1_eval(parts);
        part2 += p2_eval(parts);
    }

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);

    return 0;
}
