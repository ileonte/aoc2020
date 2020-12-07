#include <aoc.h>

struct Node {
    std::string name{};
    size_t count{0};
};
std::ostream& operator<<(std::ostream& out, const Node& node) {
    out << "{\"" << node.name << "\", " << node.count << '}';
    return out;
}

static std::map<std::string, std::vector<Node>> bags{};

static std::vector<Node>& get_bag(const std::string& name) {
    auto r = bags.find(name);
    if (r != bags.end()) return r->second;
    auto [it, in] = bags.emplace(std::make_pair(name, std::vector<Node>{}));
    return it->second;
}

static size_t can_contain(const std::vector<Node>& nodes, const std::string& name) {
    for (const auto& node : nodes) {
        if (node.name == name) return node.count;
    }
    return 0;
}

static size_t count_sub_bags(const std::string& name) {
    const auto& contains = bags.at(name);
    size_t ret{0};

    for (const auto& it : contains) {
        ret += it.count + it.count * count_sub_bags(it.name);
    }

    return ret;
}

static void part1(const std::string& target) {
    std::vector<std::string> search_terms{};
    std::set<std::string> ret{};

    search_terms.push_back(target);

    while (search_terms.size()) {
        std::vector<std::string> next_searches{};
        for (const auto& term : search_terms) {
            for (const auto& [parent, contains] : bags) {
                if (can_contain(contains, term)) {
                    next_searches.push_back(parent);
                    ret.insert(parent);
                }
            }
        }
        search_terms = std::move(next_searches);
    }

    fmt::print("{}\n", ret.size());
}

static void part2(const std::string& target) {
    fmt::print("{}\n", count_sub_bags(target));
}

int main() {
    const auto split_re = std::regex("\\s+bags?(?:\\s+contain\\s+)?(?:[,. ]+)?");
    std::string line{};

    while (std::getline(std::cin, line)) {
        auto parts = aoc::str_split(line, split_re);
        if (parts.size() < 2) continue;

        auto& children = get_bag(std::string(parts[0]));
        for (size_t i = 1; i < parts.size(); i++) {
            auto p = parts.at(i);
            if (p == "no other") continue;

            size_t count{0};
            auto c = std::from_chars(p.begin(), p.end(), count);
            p = std::string_view(c.ptr + 1, p.end());

            children.push_back(Node {
                .name = std::string(p),
                .count = count,
            });
        }
    }

    part1("shiny gold");
    part2("shiny gold");

    return 0;
}
