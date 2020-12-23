#include <aoc.h>

template <typename T>
static inline std::vector<T> sublist(const std::vector<T>& l, size_t n) {
    return std::vector<T>(l.begin(), aoc::advance(l.begin(), n));
}

template <typename T>
static inline T score(const std::vector<T>& l) {
    T ret{0};
    auto mul = l.size();
    for (auto c : l) ret += c * mul--;
    return ret;
}

template <typename C>
static inline size_t container_hash(const C& l) {
    if (l.empty()) return 0;
    size_t ret{aoc::hash(l.front())};
    for (auto it = aoc::advance(l.begin()); it != l.end(); it = aoc::advance(it)) {
        ret = aoc::combine_hashes(ret, *it);
    }
    return ret;
}

template <typename T>
static inline T pop_front(std::vector<T>& v) {
    T ret{v.front()};
    v.erase(v.begin());
    return ret;
}

static inline size_t part1(const std::vector<size_t>& p1_list, const std::vector<size_t>& p2_list) {
    auto l1 = std::vector<size_t>(p1_list);
    auto l2 = std::vector<size_t>(p2_list);
    while (!l1.empty() && !l2.empty()) {
        auto c1 = pop_front(l1);
        auto c2 = pop_front(l2);
        if (c1 > c2) {
            l1.push_back(c1);
            l1.push_back(c2);
        } else {
            l2.push_back(c2);
            l2.push_back(c1);
        }
    }
    return score(l1.empty() ? l2 : l1);
}

static inline size_t part2_recurse(std::vector<size_t>& l1, std::vector<size_t>& l2, size_t game = 1) {
    std::set<std::pair<size_t, size_t>> seen_combos{};
    size_t winner{0};
    size_t round{1};

    while (!l1.empty() && !l2.empty()) {
        auto check = std::make_pair(container_hash(l1), container_hash(l2));
        if (seen_combos.contains(check)) return 1;
        seen_combos.insert(check);
        auto p = std::make_pair(pop_front(l1), pop_front(l2));
        if ((p.first <= l1.size()) && (p.second <= l2.size())) {
            auto t1 = sublist(l1, p.first);
            auto t2 = sublist(l2, p.second);
            winner = part2_recurse(t1, t2, game + 1);
        } else {
            winner = p.first > p.second ? 1 : 2;
        }
        if (winner == 1) {
            l1.push_back(p.first);
            l1.push_back(p.second);
        } else {
            l2.push_back(p.second);
            l2.push_back(p.first);
        }
        round++;
    }

    return l1.empty() ? 2 : 1;
}

static inline size_t part2(const std::vector<size_t>& p1_list, const std::vector<size_t>& p2_list) {
    auto l1 = std::vector<size_t>(p1_list);
    auto l2 = std::vector<size_t>(p2_list);
    part2_recurse(l1, l2);
    return score(l1.empty() ? l2 : l1);
}

static inline bool read_player_deck(std::vector<size_t>& deck) {
    std::string line{};
    if (!std::getline(std::cin, line)) return false;
    auto sv = aoc::trim(line);
    if (!sv.starts_with("Player")) return false;
    while (std::getline(std::cin, line)) {
        sv = aoc::trim(line);
        if (sv.empty()) break;
        deck.push_back(aoc::from_chars<size_t>(sv).value());
    }
    return true;
}

int main() {
    std::vector<size_t> l1{};
    std::vector<size_t> l2{};

    if (!read_player_deck(l1)) return 1;
    if (!read_player_deck(l2)) return 1;

    auto p1 = part1(l1, l2);
    auto p2 = part2(l1, l2);
    fmt::print("{}\n", p1);
    fmt::print("{}\n", p2);

    return 0;
}
