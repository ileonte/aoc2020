#include <aoc.h>

struct ValidatorResult {
    size_t shift;
    bool valid;
};

using FieldValidator = ValidatorResult (*)(std::string_view);

static const std::map<std::string_view, FieldValidator> valid_fields {
    {"cid", [](std::string_view) -> ValidatorResult {
        return {7, true};
    }},
    {"pid", [](std::string_view val) -> ValidatorResult {
        size_t len{0};
        bool ok{true};
        for (auto ch : val) {
            len += 1;
            ok = ok && std::isdigit(ch);
        }
        return {6, ok && (len == 9)};
    }},
    {"ecl", [](std::string_view val) -> ValidatorResult {
        return {
            5,
            (val == "amb" || val == "blu" || val == "brn" || val == "gry" || val == "grn" || val == "hzl" || val == "oth")
        };
    }},
    {"hcl", [](std::string_view val) -> ValidatorResult {
        bool ok{true};
        if (val.size() != 7) return {4, false};
        if (val.at(0) != '#') return {4, false};
        val.remove_prefix(1);
        for (auto ch : val) ok = ok && std::isxdigit(ch);
        return {4, ok};
    }},
    {"hgt", [](std::string_view val) -> ValidatorResult {
        int ival{0};
        if (val.size() == 4) {
            if (aoc::substr(val, 2) != "in") return {3, false};
            val.remove_suffix(2);
            std::from_chars(val.begin(), val.end(), ival);
            return {3, std::clamp(ival, 59, 76) == ival};
        } else if (val.size() == 5) {
            if (aoc::substr(val, 3) != "cm") return {3, false};
            val.remove_suffix(2);
            std::from_chars(val.begin(), val.end(), ival);
            return {3, std::clamp(ival, 150, 193) == ival};
        } else {
            return {3, false};
        }
    }},
    {"eyr", [](std::string_view val) -> ValidatorResult {
        int ival{0};
        std::from_chars(val.begin(), val.end(), ival);
        return {2, std::clamp(ival, 2020, 2030) == ival};
    }},
    {"iyr", [](std::string_view val) -> ValidatorResult {
        int ival{0};
        std::from_chars(val.begin(), val.end(), ival);
        return {1, std::clamp(ival, 2010, 2020) == ival};
    }},
    {"byr", [](std::string_view val) -> ValidatorResult {
        int ival{0};
        std::from_chars(val.begin(), val.end(), ival);
        return {0, std::clamp(ival, 1920, 2002) == ival};
    }},
};
constexpr size_t valid_mask = 0x7f;

static inline void process_passport_line(std::string_view line, size_t& field_hash, bool& valid) {
    for (auto kvpair : aoc::str_split(line, ' ')) {
        auto kv = aoc::str_split(kvpair, ':');
        if (kv.size() != 2) continue;
        if (auto v = valid_fields.find(kv.at(0)); v != valid_fields.end()) {
            auto ret = v->second(kv.at(1));
            field_hash |= (1 << ret.shift);
            valid = valid && ret.valid;
        }
    }
}

static inline bool passport_has_required_fields(size_t field_hash) {
    return (field_hash & valid_mask) == valid_mask;
}

int main() {
    size_t field_hash{0};
    bool valid{true};
    size_t answer_p1{0};
    size_t answer_p2{0};

    process_passport_line(
        "pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980 hcl:#623a2f",
        field_hash,
        valid);

    const auto update_counts = [&]() {
        auto fields_ok = passport_has_required_fields(field_hash);
        answer_p1 += fields_ok;
        answer_p2 += (fields_ok && valid);
        field_hash = 0;
        valid = true;
    };

    std::string line{};
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            update_counts();
            continue;
        }
        process_passport_line(line, field_hash, valid);
    }
    if (!line.empty()) update_counts();

    fmt::print("{}\n", answer_p1);
    fmt::print("{}\n", answer_p2);

    return 0;
}
