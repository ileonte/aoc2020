#include <aoc.h>

struct IngredientInfo {
    std::string_view name{};
    std::string_view allergen{};
    size_t appearances{0};
};
std::ostream& operator<<(std::ostream& out, const IngredientInfo& info) {
    return out << "{\"" << info.name << "\", \"" << info.allergen << "\", " << info.appearances << "}";
}

using IngredientNames = std::vector<std::string_view>;
using UniqueIngredients = std::unordered_set<std::string_view>;
using AllergenStore = std::unordered_map<std::string_view, UniqueIngredients>;
using IngredientStore = std::vector<IngredientInfo>;
using IngredientMap = std::unordered_map<std::string_view, IngredientInfo>;

static inline UniqueIngredients unique_ingredients(const UniqueIngredients& existing, const IngredientNames& additions) noexcept {
    UniqueIngredients ret{};
    for (auto& i : additions) {
        if (existing.contains(i)) ret.insert(i);
    }
    return ret;
}

static inline bool parse_input(const std::vector<std::string>& data_store, IngredientMap& ingredient_map) noexcept {
    static const std::regex line_re("([^\\(]+)\\s*\\(contains\\s+([^\\)]+)\\)");
    static const std::regex ingredient_split_re("\\s+");
    static const std::regex allergen_split_re("\\s*,\\s*");
    AllergenStore allergen_store{};

    for (auto& line : data_store) {
        auto sv = aoc::trim(line);
        std::cmatch m{};
        if (!std::regex_match(sv.begin(), sv.end(), m, line_re)) {
            fmt::print(std::cerr, "Failed to parse '{}'\n", line);
            return false;
        }

        auto ingredients = aoc::str_split(aoc::to_sv(m[1]), ingredient_split_re);
        auto allergens = aoc::str_split(aoc::to_sv(m[2]), allergen_split_re);

        for (auto i : ingredients) {
            auto [it, added] = ingredient_map.insert({i, IngredientInfo{i, "", 1}});
            if (!added) it->second.appearances += 1;
        }

        for (auto a : allergens) {
            auto [it, added] = allergen_store.insert({a, UniqueIngredients(ingredients.begin(), ingredients.end())});
            if (!added) it->second = unique_ingredients(it->second, ingredients);
        }
    }

    while (true) {
        bool all_done{true};
        size_t resolved{0};

        for (auto& [allergen, ingredients] : allergen_store) {
            if (!ingredients.size()) continue;
            if (ingredients.size() > 1) {
                all_done = false;
                continue;
            }

            auto ingredient = *ingredients.begin();
            ingredient_map[ingredient].allergen = allergen;
            resolved += 1;

            for (auto& it : allergen_store) it.second.erase(ingredient);
        }

        if (!resolved) {
            if (all_done) break;
            fmt::print(std::cerr, "Failed to resolve ingredient-to-allergen mapping:\n{}", allergen_store);
            return false;
        }
    }

    return true;
}

int main() {
    std::vector<std::string> data_store{};
    std::string line{};
    while (std::getline(std::cin, line)) data_store.push_back(line);

    IngredientMap ingredients{};
    if (!parse_input(data_store, ingredients)) return 1;

    fmt::print("{}\n", std::accumulate(
        ingredients.begin(), ingredients.end(),
        size_t(0),
        [](size_t acc, const IngredientMap::value_type& info) -> size_t {
            return acc + (info.second.allergen.empty() ? info.second.appearances : 0);
        }));

    IngredientStore dangerous_ingredients{};
    for (auto& it : ingredients) {
        if (it.second.allergen.empty()) continue;
        dangerous_ingredients.push_back(it.second);
    }
    std::sort(
        dangerous_ingredients.begin(), dangerous_ingredients.end(),
        [](const IngredientInfo& i1, const IngredientInfo& i2) -> bool {
            return i1.allergen < i2.allergen;
        }
    );
    for (size_t i = 0; i < dangerous_ingredients.size(); i++) {
        fmt::print("{}{}",
            dangerous_ingredients[i].name,
            i < dangerous_ingredients.size() - 1 ? "," : "");
    }
    fmt::print("\n");

    return 0;
}
