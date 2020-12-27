#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string_view>
#include <charconv>
#include <optional>
#include <functional>
#include <algorithm>
#include <numeric>
#include <array>
#include <deque>
#include <sstream>
#include <chrono>
#include <regex>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cerrno>

#if !defined(DEBUG)
#define DEBUG 0
#endif

namespace fmt {
    template <typename T, size_t N>
    struct formatter<std::array<T, N>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::array<T, N>& a, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (a.size() > 0) {
                format_to(ctx.out(), "{}", a.at(0));
                for (auto i = 1; i < a.size(); i++)
                    format_to(ctx.out(), ", {}", a.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename T>
    struct formatter<std::vector<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::vector<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", v.at(0));
                for (auto i = 1; i < v.size(); i++)
                    format_to(ctx.out(), ", {}", v.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename T>
    struct formatter<std::deque<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::deque<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", v.at(0));
                for (auto i = 1; i < v.size(); i++)
                    format_to(ctx.out(), ", {}", v.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename T>
    struct formatter<std::set<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::set<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", *std::cbegin(v));
                for (auto i = std::next(std::cbegin(v)); i != std::cend(v); i++)
                    format_to(ctx.out(), ", {}", *i);
            }
            return format_to(ctx.out(), "}}");
        }
    };

    template <typename T>
    struct formatter<std::unordered_set<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::unordered_set<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", *std::cbegin(v));
                for (auto i = std::next(std::cbegin(v)); i != std::cend(v); i++)
                    format_to(ctx.out(), ", {}", *i);
            }
            return format_to(ctx.out(), "}}");
        }
    };

    template <typename K, typename V>
    struct formatter<std::unordered_map<K, V>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::unordered_map<K, V>& m, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            for (const auto& [k, v] : m)
                format_to(ctx.out(), "\n   {} -> {}", k, v);
            return format_to(ctx.out(), "{}}}", m.size() ? "\n" : "");
        }
    };

    template <typename K, typename V>
    struct formatter<std::map<K, V>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::map<K, V>& m, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            for (const auto& [k, v] : m)
                format_to(ctx.out(), "\n   {} -> {}", k, v);
            return format_to(ctx.out(), "{}}}", m.size() ? "\n" : "");
        }
    };

    template <typename T1, typename T2>
    struct formatter<std::pair<T1, T2>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::pair<T1, T2>& v, FormatContext &ctx) {
            return format_to(ctx.out(), "{{{}, {}}}", v.first, v.second);
        }
    };

    template <typename T>
    struct formatter<std::list<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::list<T>& l, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (l.size() > 0) {
                auto it = l.begin();
                format_to(ctx.out(), "{}", *it);
                for (auto i = 1; i < l.size(); i++) {
                    it++;
                    format_to(ctx.out(), ", {}", *it);
                }
            }
            return format_to(ctx.out(), "]");
        }
    };
}

namespace aoc {
    namespace detail {
        inline constexpr size_t hash() {
            return 0;
        }

        template <typename T>
        inline constexpr size_t hash(const T& v) {
            return std::hash<T>{}(v);
        }

        template <typename T1, typename T2>
        inline constexpr size_t hash(const T1& t1, const T2& t2) {
            size_t s1 = std::hash<T1>{}(t1);
            size_t s2 = std::hash<T2>{}(t2);
            return s1 ^ s2 + 0x9e3779b9 + (s1 << 6) + (s1 >> 2);
        }
    }
    template <typename T, typename... Ts>
    inline constexpr size_t combine_hashes(const T& s1, const Ts&... s2) {
        return detail::hash(s1, s2...);
    }
    template <typename T, typename... Ts>
    inline constexpr size_t hash(const T& t, const Ts& ...ts) {
        return combine_hashes(t, detail::hash(ts)...);
    }

    template <typename IT>
    inline IT advance(const IT& it, size_t n = 1) {
        auto ret{it};
        std::advance(ret, n);
        return ret;
    }

    inline std::string_view trim(std::string_view sv) {
        while (!sv.empty() && std::isspace(sv.front())) sv.remove_prefix(1);
        while (!sv.empty() && std::isspace(sv.back())) sv.remove_suffix(1);
        return sv;
    }


    inline constexpr std::string_view
    substr(std::string_view the_string, size_t from = 0,
           size_t count = std::numeric_limits<size_t>::max())
    {
        if (from >= the_string.size()) return std::string_view();
        return std::string_view(the_string.data() + from, std::min(count, the_string.size() - from));
    }

    inline std::vector<std::string_view>
    str_split(std::string_view the_string, char sep,
              size_t max_splits = std::numeric_limits<size_t>::max())
    {
        std::vector<std::string_view> ret;
        size_t start = 0;
        size_t end = 0;
        while (start < the_string.size()) {
            end = the_string.find(sep, start);
            ret.push_back(substr(the_string, start, end - start));
            if (end >= the_string.size()) break;
            start = end + 1;
            if (ret.size() >= max_splits - 1) {
                if (start < the_string.size())
                    ret.push_back(substr(the_string, start));
                break;
            }
        }
        if (start == the_string.size() && !the_string.empty()) {
            ret.push_back("");
        }
        return ret;
    }

    template <typename T>
    constexpr inline T cpow(T base, T pow) {
        T ret{1};
        for (T p = 1; p <= pow; p++)
            ret *= base;
        return ret;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T gcd(T v1, T v2) {
        T n1 = std::abs(std::max(v1, v2));
        T n2 = std::abs(std::min(v1, v2));
        if (!n2) return 1;
        while (n2) {
            T t = n1;
            n1 = n2;
            n2 = t % n2;
        }
        return n1;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T lcm(T v1, T v2) {
        return v1 * v2 / gcd(v1, v2);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T bitmask(size_t size, size_t final_shift = 0) {
        T r{1};
        for (size_t i = 1; i < size; i++) r = (r << 1) | 1;
        return r << final_shift;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    static inline T set_bit(T n, size_t shift, bool on_off) {
        auto mask = (T(1) << shift);
        return on_off ? (n | mask) : (n & ~mask);
    }

    template <typename Callable, typename... Args>
    inline std::chrono::nanoseconds time_call(const Callable& f, Args... args)
    {
        auto start = std::chrono::steady_clock::now();
        f(args...);
        auto end = std::chrono::steady_clock::now();
        return end - start;
    }

    inline std::string_view to_sv(std::cmatch::const_reference r) {
        return std::string_view(r.first, r.length());
    }

    inline std::string_view to_sv(const std::string& s) {
        return std::string_view(s);
    }

    inline std::vector<std::string_view>
    str_split(std::string_view the_string, const std::regex& re,
              size_t max_splits = std::numeric_limits<size_t>::max())
    {
        std::vector<std::string_view> ret{};
        max_splits = std::max(size_t(2), max_splits);
        while (true) {
            std::cmatch r{};
            if (!std::regex_search(the_string.begin(), the_string.end(), r, re)) {
                ret.push_back(the_string);
                break;
            }
            ret.push_back(to_sv(r.prefix()));
            the_string = to_sv(r.suffix());
            if (the_string.empty()) break;
            if (ret.size() >= max_splits - 1) {
                ret.push_back(the_string);
                break;
            }
        }
        return ret;
    }

    template <typename T>
    inline bool from_chars(std::string_view sv, T& value, int base = 10) {
        auto rc = std::from_chars(sv.begin(), sv.end(), value, base);
        return rc.ptr = sv.end();
    }

    template <typename T>
    inline std::optional<T> from_chars(std::string_view sv, int base = 10) {
        T ret{};
        auto rc = std::from_chars(sv.begin(), sv.end(), ret, base);
        if (rc.ptr == sv.end()) return ret;
        return std::nullopt;
    }

    template <typename T>
    inline constexpr std::pair<T, T> numeric_limits() {
        return std::make_pair(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }

    inline auto accumulate(auto& c, auto init, auto op) {
        return std::accumulate(std::begin(c), std::end(c), init, op);
    }

    template <typename T, size_t N>
    inline constexpr size_t member_count(const T (&)[N]) noexcept {
        return N;
    }
}

namespace aoc::detail::defer {
    class helper {
    public:
        template <typename Callable>
        class helper_impl {
        public:
            helper_impl(Callable c) : call_(c) {}
            ~helper_impl() { call_(); }
        private:
            Callable call_;
        };

        static inline const helper& instance() {
            static helper g_instance;
            return g_instance;
        }

        template <typename Callable>
        inline helper_impl<Callable> operator<<(Callable c) const { return c; }
    };
}

#define _AOC_DEFERHACK_UNUSED_ATTR_DEF_ [[maybe_unused]]

#define _aoc_deferhack_cathack_2(a, b) a ## b
#define _aoc_deferhack_cathack_1(a, b) _aoc_deferhack_cathack_2(a, b)
#define defer \
    _AOC_DEFERHACK_UNUSED_ATTR_DEF_ \
    const auto& _aoc_deferhack_cathack_1(_deferhack_val_for_line_, __LINE__) = \
        aoc::detail::defer::helper::instance() << [&]()
