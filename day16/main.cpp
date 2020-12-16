#include <aoc.h>

struct Field {
    static constexpr const size_t invalid_index{std::numeric_limits<size_t>::max()};
    std::string name{};
    size_t min1{0};
    size_t max1{0};
    size_t min2{0};
    size_t max2{0};
    size_t idx{invalid_index};

    static inline std::optional<Field> parse(std::string_view line) {
        static std::regex range_re("^([^:]+):\\s*(\\d+)-(\\d+)+\\s+or\\s+(\\d+)+-(\\d+)+");
        std::cmatch match{};
        Field ret{};
        if (!std::regex_match(line.begin(), line.end(), match, range_re)) return std::nullopt;
        ret.name = match[1].str();
        if (!aoc::from_chars(aoc::to_sv(match[2]), ret.min1)) return std::nullopt;
        if (!aoc::from_chars(aoc::to_sv(match[3]), ret.max1)) return std::nullopt;
        if (!aoc::from_chars(aoc::to_sv(match[4]), ret.min2)) return std::nullopt;
        if (!aoc::from_chars(aoc::to_sv(match[5]), ret.max2)) return std::nullopt;
        return ret;
    }

    inline bool matches(size_t v) const {
        auto t1 = std::clamp(v, min1, max1);
        auto t2 = std::clamp(v, min2, max2);
        return (t1 == v) || (t2 == v);
    }
};
static inline std::ostream& operator<<(std::ostream& out, Field f) {
    out << "{\"" << f.name << "\", ";
    out << f.min1 << ", " << f.max1 << ", ";
    out << f.min2 << ", " << f.max2 << ", ";
    out << f.idx << "}";
    return out;
}

struct Ticket {
    std::vector<size_t> field_values{};

    inline std::vector<size_t> possible_matches(const Field& f) const {
        std::vector<size_t> ret{};
        for (size_t idx = 0; idx < field_values.size(); idx++)
            if (f.matches(field_values.at(idx))) ret.push_back(idx);
        return ret;
    }

    inline bool validate(const std::vector<Field>& fs, size_t& inc) const {
        size_t val{0};
        bool ret{true};
        for (auto v : field_values) {
            bool valid{false};
            for (const auto& f : fs) valid = valid || f.matches(v);
            if (!valid) {
                val += v;
                ret = false;
            }
        }
        inc = val;
        return ret;
    }

    static inline std::optional<Ticket> parse(std::string_view line) {
        Ticket ret{};
        ret.field_values.reserve(32);
        for (auto it : aoc::str_split(line, ',')) {
            size_t tmp{0};
            if (!aoc::from_chars(it, tmp)) return std::nullopt;
            ret.field_values.push_back(tmp);
        }
        return ret;
    }
};

template <typename T>
static inline void intersection(std::vector<T>& v1, const std::vector<T>& v2) {
    std::vector<T> r{};
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(r));
    v1 = std::move(r);
}

int main() {
    std::vector<Field> fields{};
    std::vector<Ticket> tickets{};

    std::string line{};
    while (std::getline(std::cin, line)) {
        auto sv = aoc::trim(line);
        if (sv.empty()) break;
        if (auto r = Field::parse(sv); r) {
            fields.emplace_back(std::move(r.value()));
        } else {
            fmt::print(std::cerr, "Invalid field definition: '{}'\n", line);
            return 1;
        }
    }

    if (!std::getline(std::cin, line)) return 1;
    if (!std::getline(std::cin, line)) return 1;
    if (auto r = Ticket::parse(aoc::trim(line)); r) {
        tickets.emplace_back(std::move(r.value()));
    } else {
        fmt::print(std::cerr, "Failed to parse own ticket: '{}'\n", line);
        return 1;
    }
    if (!std::getline(std::cin, line)) return 1;

    size_t part1{0};
    if (!std::getline(std::cin, line)) return 1;
    while (std::getline(std::cin, line)) {
        if (auto r = Ticket::parse(aoc::trim(line)); r) {
            size_t inc{0};
            if (!r.value().validate(fields, inc)) {
                part1 += inc;
            } else {
                tickets.emplace_back(std::move(r.value()));
            }
        } else {
            fmt::print(std::cerr, "Failed to parse ticket: '{}'\n", line);
            return 1;
        }
    }

    fmt::print("{}\n", part1);

    std::vector<std::vector<size_t>> matches{};
    for (auto& f : fields) {
        auto midx = tickets[1].possible_matches(f);
        for (size_t i = 2; i < tickets.size(); i++) {
            intersection(midx, tickets.at(i).possible_matches(f));
            if (midx.empty()) {
                fmt::print(std::cerr, "No match for '{}'\n", f);
                return 1;
            }
        }
        matches.emplace_back(std::move(midx));
    }
    while (true) {
        size_t count{0};
        for (size_t field_idx = 0; field_idx < fields.size(); field_idx++) {
            auto& field = fields.at(field_idx);
            auto& field_matches = matches.at(field_idx);

            if (field_matches.size() != 1) continue;

            field.idx = field_matches.at(0);
            for (auto& m : matches) {
                if (auto it = std::find(m.begin(), m.end(), field.idx); it != m.end())
                    m.erase(it);
            }
            count += 1;
        }
        if (!count) {
            bool empty = std::accumulate(matches.begin(), matches.end(), true, [](bool acc, const auto& i) -> bool {
                return acc && i.empty();
            });
            if (!empty) {
                fmt::print(std::cerr, "No more matches: {}\n", matches);
                return 1;
            }
            break;
        }
    }

    static constexpr const std::string_view dep_marker("departure");
    size_t part2{1};
    for (auto& f : fields) {
        if (aoc::substr(f.name, 0, dep_marker.size()) == dep_marker) {
            part2 *= tickets[0].field_values[f.idx];
        }
    }
    fmt::print("{}\n", part2);

    return 0;
}
