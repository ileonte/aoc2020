#pragma once

#include <aoc.h>
#include "tile.h"

namespace detail {
    struct TileState {
        const SquareTile* tile;
        size_t transform_id;
    };
    static inline std::ostream& operator<<(std::ostream& out, TileState state) {
        fmt::print(out, "{{{}, {}}}", state.tile->id(), state.transform_id);
        return out;
    }
}

class SquareImage {
public:
    inline SquareImage() = default;
    inline SquareImage(const SquareImage&) = default;
    inline SquareImage(SquareImage&&) = default;
    inline SquareImage& operator=(const SquareImage&) = default;
    inline SquareImage& operator=(SquareImage&&) = default;

    const auto dimension_in_tiles() const { return dimension_in_tiles_; }
    const auto dimension() const {
        return tiles_.empty() ? 0 : dimension_in_tiles_ * tiles_.begin()->second.dimension();
    }
    const auto& tiles() const { return tiles_; }

    inline const auto& orderings() const { return orderings_; }
    inline auto corners() const {
        std::vector<std::vector<size_t>> ret{};
        if (!orderings_.size()) return ret;
        for (auto& o : orderings_) {
            ret.push_back({
                o[0].tile->id(),
                o[dimension_in_tiles_ - 1].tile->id(),
                o[(dimension_in_tiles_ - 1) * dimension_in_tiles_].tile->id(),
                o[(dimension_in_tiles_ - 1) * dimension_in_tiles_ + dimension_in_tiles_ - 1].tile->id()});
        }
        return ret;
    }

    inline bool add_tile(SquareTile tile) {
        if (tiles_.contains(tile.id())) {
            fmt::print(std::cerr, "Duplicate tile ID: {}\n", tile.id());
            return false;
        }
        auto [it, added] = tiles_.insert(std::make_pair(tile.id(), std::move(tile)));
        auto ptile = &it->second;
        for (size_t i = 0; i < aoc::member_count(g_transforms); i++) {
            auto m = ptile->borders().transformed(g_transforms[i]);
            auto v = detail::TileState{ptile, i};

            auto it = top_borders_.find(m.top());
            if (it == top_borders_.end()) top_borders_.emplace(m.top(), std::vector<detail::TileState>{v});
            else it->second.push_back(v);

            it = left_borders_.find(m.left());
            if (it == left_borders_.end()) left_borders_.emplace(m.left(), std::vector<detail::TileState>{v});
            else it->second.push_back(v);
        }
        return true;
    }

    inline bool assemble() {
        size_t tiles_per_dimension = std::lrint(std::sqrt(tiles_.size()));
        if (tiles_per_dimension * tiles_per_dimension != tiles_.size()) {
            fmt::print(std::cerr, "Cannot make a square image with {} tiles!\n", tiles_.size());
            return false;
        }
        dimension_in_tiles_ = tiles_per_dimension;

        auto ordering = std::vector<detail::TileState>{};
        ordering.reserve(tiles_.size());

        for (auto& it : tiles_) {
            for (size_t i = 0; i < aoc::member_count(g_transforms); i++) {
                auto available_tiles = tile_ids();
                ordering.clear();
                auto next = detail::TileState{&it.second, i};
                continue_ordering(ordering, available_tiles, tiles_per_dimension, next);
                if (available_tiles.empty()) {
                    orderings_.emplace_back(std::move(ordering));
                }
            }
        }

        return orderings_.size() > 0;
    }

    inline bool draw_ordering(const std::vector<detail::TileState>& ordering, std::vector<char>& result, size_t& stride) const {
        auto tile_dimension{tiles_.begin()->second.dimension() - 2};
        stride = tile_dimension * dimension_in_tiles_;
        auto size = dimension_in_tiles_ * tile_dimension * stride;

        result.clear();
        result.reserve(size);
        result.insert(result.begin(), size, ' ');

        for (size_t y = 0; y < dimension_in_tiles_; y++) {
            for (size_t x = 0; x < dimension_in_tiles_; x++) {
                auto& o = ordering.at(y * dimension_in_tiles_ + x);
                auto tile = o.tile->transformed(g_transforms[o.transform_id]);
                auto dx = x * tile_dimension;
                auto dy = y * tile_dimension;
                for (size_t sy = 1; sy <= tile_dimension; sy++) {
                    auto sv = std::string_view(tile.data().data() + sy * tile.width() + 1, tile.width() - 2);
                    std::copy(sv.begin(), sv.end(), result.data() + (dy + sy - 1) * stride + dx);
                }
            }
        }

        return true;
    }

    inline std::unordered_set<size_t> tile_ids() const {
        std::unordered_set<size_t> ret{};
        for (auto& it : tiles_) ret.insert(it.first);
        return ret;
    }

private:
    std::unordered_map<size_t, SquareTile> tiles_{};
    std::vector<std::vector<detail::TileState>> orderings_{};
    size_t dimension_in_tiles_{0};
    std::unordered_map<uint16_t, std::vector<detail::TileState>> top_borders_{};
    std::unordered_map<uint16_t, std::vector<detail::TileState>> left_borders_{};

    inline std::vector<detail::TileState>
    chose_next_tile(const std::vector<detail::TileState>& ordering, size_t tiles_per_dimension) const {
        auto current_row = ordering.size() / tiles_per_dimension;

        if (ordering.size() % tiles_per_dimension) {
            auto& current_tile = ordering.back();
            auto m = current_tile.tile->borders().transformed(g_transforms[current_tile.transform_id]);
            auto it = left_borders_.find(m.right());
            if (it != left_borders_.end()) return it->second;
        } else {
            auto& current_tile = ordering.at((current_row - 1) * tiles_per_dimension);
            auto m = current_tile.tile->borders().transformed(g_transforms[current_tile.transform_id]);
            auto it = top_borders_.find(m.bottom());
            if (it != top_borders_.end()) return it->second;
        }

        return {};
    }

    inline void continue_ordering(
        std::vector<detail::TileState>& ordering, std::unordered_set<size_t>& available_tiles,
        size_t tiles_per_dimension, detail::TileState next) const
    {
        ordering.push_back(next);
        available_tiles.erase(next.tile->id());
        if (available_tiles.empty()) return;

        auto candidates = chose_next_tile(ordering, tiles_per_dimension);
        for (auto candidate : candidates) {
            if (!available_tiles.contains(candidate.tile->id())) continue;
            continue_ordering(ordering, available_tiles, tiles_per_dimension, candidate);
            if (available_tiles.empty()) return;
        }

        ordering.pop_back();
        available_tiles.insert(next.tile->id());
    }
};
