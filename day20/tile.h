#pragma once

#include "basedefs.h"
#include "borders.h"

template <char background_char_>
class TSquareTile {
public:
    static constexpr const char background_char = background_char_;

    inline TSquareTile() = default;
    inline TSquareTile(size_t id, size_t dimension, std::vector<char> d)
        : id_(id), dimension_(dimension), data_(std::move(d))
    {
        parse_borders();
    }
    inline TSquareTile(const TSquareTile&) = default;
    inline TSquareTile(TSquareTile&&) = default;
    inline TSquareTile& operator=(const TSquareTile&) = default;
    inline TSquareTile& operator=(TSquareTile&&) = default;

    inline auto id() const { return id_; }
    inline auto width() const { return dimension_; }
    inline auto height() const { return dimension_; }
    inline auto dimension() const { return dimension_; }
    inline const auto& data() const { return data_; }
    inline const auto& borders() const { return borders_; }

    inline TSquareTile& transform(Transform t) {
        auto tmp = data_;
        switch (t) {
            case T_VFlip: {
                for (size_t y = 0; y < dimension_; y++) {
                    auto sv = std::string_view(data_.data() + y * dimension_, dimension_);
                    std::copy(sv.begin(), sv.end(), tmp.data() + (dimension_ - 1 - y) * dimension_);
                }
                break;
            }
            case T_HFlip: {
                for (size_t y = 0; y < dimension_; y++) {
                    auto sv = std::string_view(data_.data() + y * dimension_, dimension_);
                    std::copy(sv.rbegin(), sv.rend(), tmp.data() + y * dimension_);
                }
                break;
            }
            case T_Rot90: {
                for (size_t y = 0; y < dimension_; y++) {
                    for (size_t x = 0; x < dimension_; x++) {
                        auto nx = y;
                        auto ny = dimension_ - 1 - x;
                        tmp[ny * dimension_ + nx] = data_[y * dimension_ + x];
                    }
                }
                break;
            }
            case T_Rot180: {
                for (size_t y = 0; y < dimension_; y++) {
                    for (size_t x = 0; x < dimension_; x++) {
                        auto nx = dimension_ - 1 - x;
                        auto ny = dimension_ - 1 - y;
                        tmp[ny * dimension_ + nx] = data_[y * dimension_ + x];
                    }
                }
                break;
            }
            case T_Rot270: {
                for (size_t y = 0; y < dimension_; y++) {
                    for (size_t x = 0; x < dimension_; x++) {
                        auto nx = dimension_ - 1 - y;
                        auto ny = x;
                        tmp[ny * dimension_ + nx] = data_[y * dimension_ + x];
                    }
                }
                break;
            }
            default: break;
        }
        data_ = std::move(tmp);
        borders_.transform(t);
        return *this;
    }
    template <size_t N>
    inline TSquareTile& transform(const Transform (&ts)[N]) {
        for (auto t : ts) transform(t);
        return *this;
    }

    inline TSquareTile transformed(Transform t) const {
        return TSquareTile(*this).transform(t);
    }
    template <size_t N>
    inline TSquareTile transformed(const Transform (&ts)[N]) const {
        return TSquareTile(*this).transform(ts);
    }

    static inline std::optional<TSquareTile> read(std::istream& in = std::cin) {
        static const auto tile_re = std::regex("Tile\\s+(\\d+):");
        size_t id{0};
        size_t w{0};
        size_t h{0};
        std::vector<char> data{};
        std::string line{};
        std::cmatch m{};

        if (!std::getline(in, line)) return std::nullopt;
        auto sv = aoc::trim(line);
        if (!std::regex_match(sv.begin(), sv.end(), m, tile_re)) {
            fmt::print(std::cerr, "Failed to parse image header: '{}'\n", line);
            return std::nullopt;
        }
        if (!aoc::from_chars(aoc::to_sv(m[1]), id)) {
            fmt::print(std::cerr, "Failed to parse '{}' as a number\n", aoc::to_sv(m[1]));
            return std::nullopt;
        }

        while (std::getline(in, line)) {
            sv = aoc::trim(line);
            if (sv.empty()) break;

            if (!w) {
                if (sv.size() > 16) {
                    fmt::print(std::cerr, "Line too long: '{}'\n", sv);
                    return std::nullopt;
                }
                w = sv.size();
            } else {
                if (sv.size() != w) {
                    fmt::print(std::cerr, "Invalid line lenght: got {}, expected {} ('{}')\n",
                        sv.size(), w, sv);
                    return std::nullopt;
                }
            }

            h += 1;
            data.insert(data.end(), sv.begin(), sv.end());
        }
        if (w != h) {
            fmt::print(std::cerr, "Tile {} is not square - got {} lines, {} columns!\n", id, w, h);
            return std::nullopt;
        }

        return TSquareTile(id, w, std::move(data));
    }

private:
    size_t id_{0};
    size_t dimension_{0};
    std::vector<char> data_{};
    SquareBorders borders_{};

    void parse_borders() {
        size_t line{0};
        size_t shift{16 - dimension_};
        uint16_t t{0};
        uint16_t l{0};
        uint16_t b{0};
        uint16_t r{0};

        for (size_t i = 0; i < dimension_; i++) {
            t = (t << 1) + (data_[i] != background_char_);
            b = (b << 1) + (data_[(dimension_ - 1) * dimension_ + i] != background_char_);
        }
        for (line = 0; line < dimension_; line++) {
            l = (l << 1) + (data_[line * dimension_] != background_char_);
            r = (r << 1) + (data_[line * dimension_ + dimension_ - 1] != background_char_);
        }

        borders_ = SquareBorders(dimension_, t, l, b, r);
    }
};
template <char bg>
inline std::ostream& operator<<(std::ostream& out, const TSquareTile<bg>& tile) {
    draw_image(out, tile.data(), tile.width());
    return out;
}

using SquareTile = TSquareTile<'.'>;

namespace std {
    template <char background_char_>
    struct hash<TSquareTile<background_char_>> {
        inline size_t operator()(const TSquareTile<background_char_>& img) const {
            return img.id();
        }
    };
}
