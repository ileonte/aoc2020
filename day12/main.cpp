#include <aoc.h>

struct MovementDelta {
    ssize_t dx{0};
    ssize_t dy{0};
};
std::ostream& operator<<(std::ostream& out, MovementDelta delta) {
    return out << "{.dx=" << delta.dx << ", .dy=" << delta.dy << '}';
}

struct Position {
    ssize_t x{0};
    ssize_t y{0};
};
std::ostream& operator<<(std::ostream& out, Position pos) {
    return out << "{.x=" << pos.x << ", .y=" << pos.y << '}';
}

static constexpr const std::array<MovementDelta, 4> turn_deltas{{
    { 1,   0}, // east
    { 0,  -1}, // south
    {-1,   0}, // west
    { 0,   1}, // north
}};

enum class ActionType : size_t {
    move_east,
    move_south,
    move_west,
    move_north,
    move_forward,   // movement relative to facing
    turn_left,
    turn_right,
};
std::ostream& operator<<(std::ostream& out, ActionType type) {
    switch (type) {
        case ActionType::move_east: return out << "move_east";
        case ActionType::move_south: return out << "move_south";
        case ActionType::move_west: return out << "move_west";
        case ActionType::move_north: return out << "move_north";
        case ActionType::move_forward: return out << "move_forward";
        case ActionType::turn_left: return out << "turn_left";
        case ActionType::turn_right: return out << "turn_right";
    }
    return out;
}

struct Action {
    ActionType action{ActionType::move_east};
    ssize_t magnitude{0};;

    static inline std::optional<Action> parse(std::string_view line) {
        auto sv = aoc::trim(line);
        if (sv.empty()) return std::nullopt;

        ssize_t magnitude{0};
        char op = sv[0];
        sv.remove_prefix(1);
        auto rc = std::from_chars(sv.begin(), sv.end(), magnitude);
        if (rc.ptr != sv.end()) {
            fmt::print(std::cerr, "Invalid numeric value '{}' on line '{}'\n", sv, line);
            return std::nullopt;
        }

        switch (op) {
            case 'N': return Action {ActionType::move_north, magnitude};
            case 'S': return Action {ActionType::move_south, magnitude};
            case 'E': return Action {ActionType::move_east, magnitude};
            case 'W': return Action {ActionType::move_west, magnitude};
            case 'F': return Action {ActionType::move_forward, magnitude};
            case 'R': return Action {ActionType::turn_right, magnitude};
            case 'L': return Action {ActionType::turn_left, magnitude};
            default: {
                fmt::print(std::cerr, "Unknown opcode '{}' on line '{}'\n", op, line);
                return std::nullopt;
            }
        }
    }
};
std::ostream& operator<<(std::ostream& out, Action act) {
    return out << "{.action=" << act.action << ", .magnitude=" << act.magnitude << '}';
}

struct Ship1 {
    Position position{};
    ssize_t direction{0};

    inline void execute(Action action) {
        switch (action.action) {
            case ActionType::move_east: [[fallthrough]];
            case ActionType::move_south: [[fallthrough]];
            case ActionType::move_west: [[fallthrough]];
            case ActionType::move_north: {
                auto idx = size_t(action.action);
                auto dlt = MovementDelta {
                    .dx = turn_deltas[idx].dx * action.magnitude,
                    .dy = turn_deltas[idx].dy * action.magnitude,
                };
                position.x += dlt.dx;
                position.y += dlt.dy;
                break;
            }
            case ActionType::turn_left: {
                for (ssize_t i = 0; i < action.magnitude / 90; i++) {
                    direction -= 1;
                    if (direction < 0) direction = turn_deltas.size() - 1;
                }
                break;
            }
            case ActionType::turn_right: {
                for (ssize_t i = 0; i < action.magnitude / 90; i++) {
                    direction += 1;
                    if (direction >= turn_deltas.size()) direction = 0;
                }
                break;
            }
            case ActionType::move_forward: {
                auto dlt = MovementDelta {
                    .dx = turn_deltas[direction].dx * action.magnitude,
                    .dy = turn_deltas[direction].dy * action.magnitude,
                };
                position.x += dlt.dx;
                position.y += dlt.dy;
                break;
            }
        }
    }
};

struct Ship2 {
    Position position{};
    MovementDelta beacon{10, 1};

    inline void execute(Action action) {
        switch (action.action) {
            case ActionType::move_east: [[fallthrough]];
            case ActionType::move_south: [[fallthrough]];
            case ActionType::move_west: [[fallthrough]];
            case ActionType::move_north: {
                auto idx = size_t(action.action);
                auto dlt = MovementDelta {
                    .dx = turn_deltas[idx].dx * action.magnitude,
                    .dy = turn_deltas[idx].dy * action.magnitude,
                };
                beacon.dx += dlt.dx;
                beacon.dy += dlt.dy;
                break;
            }
            case ActionType::turn_left: [[fallthrough]];
            case ActionType::turn_right: {
                auto angle = action.action == ActionType::turn_left ? action.magnitude : 360 - action.magnitude;
                ssize_t sin{0};
                ssize_t cos{1};
                switch (angle) {
                    case 90: {
                        sin = 1;
                        cos = 0;
                        break;
                    }
                    case 180: {
                        sin = 0;
                        cos = -1;
                        break;
                    }
                    case 270: {
                        sin = -1;
                        cos = 0;
                        break;
                    }
                }
                auto new_beacon = MovementDelta {
                    .dx = beacon.dx * cos - beacon.dy * sin,
                    .dy = beacon.dx * sin + beacon.dy * cos,
                };
                beacon = new_beacon;
                break;
            }
            case ActionType::move_forward: {
                auto dlt = MovementDelta {
                    .dx = beacon.dx * action.magnitude,
                    .dy = beacon.dy * action.magnitude,
                };
                position.x += dlt.dx;
                position.y += dlt.dy;
                break;
            }
        }
    }
};

int main() {
    std::string line{};
    Ship1 ship1{};
    Ship2 ship2{};

    while (std::getline(std::cin, line)) {
        auto r = Action::parse(line);
        if (!r) return 1;
        ship1.execute(r.value());
        ship2.execute(r.value());
    }

    fmt::print("{}\n", std::labs(ship1.position.x) + std::labs(ship1.position.y));
    fmt::print("{}\n", std::labs(ship2.position.x) + std::labs(ship2.position.y));

    return 0;
}
