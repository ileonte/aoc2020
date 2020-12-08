#include <aoc.h>

enum class OpCode {
    acc,
    jmp,
    nop,
};
std::ostream& operator<<(std::ostream& out, OpCode code) {
    switch (code) {
        case OpCode::acc: {
            out << "acc";
            break;
        }
        case OpCode::jmp: {
            out << "jmp";
            break;
        }
        case OpCode::nop: {
            out << "nop";
            break;
        }
    }
    return out;
}

struct Instruction {
    OpCode code{OpCode::nop};
    ssize_t arg{0};
};
std::ostream& operator<<(std::ostream& out, const Instruction& instr) {
    out << '{' << instr.code << ' ' << std::showpos << instr.arg << '}';
    return out;
}

struct Program {
    ssize_t acc{0};
    ssize_t ip{0};
    std::vector<Instruction> text{};

    static inline std::optional<Program> read() {
        static const std::regex instruction_re("^([a-z]+)\\s+(\\+|-)(\\d+)");
        std::string line{};
        Program ret{};
        while (std::getline(std::cin, line)) {
            auto sv = aoc::trim(line);
            std::cmatch m{};
            if (!std::regex_match(sv.begin(), sv.end(), m, instruction_re)) {
                fmt::print(std::cerr, "Invalid instruction on line {}: '{}'\n", ret.text.size() + 1, sv);
                return {};
            }

            auto ins_sv = aoc::to_sv(m[1]);
            auto sgn_sv = aoc::to_sv(m[2]);
            auto arg_sv = aoc::to_sv(m[3]);

            Instruction ins{};
            auto cvr = std::from_chars(arg_sv.begin(), arg_sv.end(), ins.arg);
            if (cvr.ptr != arg_sv.end()) {
                fmt::print(std::cerr, "Invalid numeric argument on line {}: '{}{}'\n", ret.text.size() + 1, sgn_sv, arg_sv);
                return {};
            }
            if (sgn_sv.front() == '-') ins.arg *= -1;

            if (ins_sv == "acc") ins.code = OpCode::acc;
            else if (ins_sv == "jmp") ins.code = OpCode::jmp;
            else if (ins_sv == "nop") ins.code = OpCode::nop;
            else {
                fmt::print(std::cerr, "Invalid instruction mnemonic on line {}: '{}'\n", ret.text.size() + 1, ins_sv);
                return {};
            }

            ret.text.push_back(ins);
        }

        return ret;
    }

    inline bool execute(std::function<bool()> pre_instruction_hook = {}) {
        while (ip >= 0 && ip < ssize_t(text.size())) {
            if (pre_instruction_hook) {
                if (!pre_instruction_hook()) return false;
            }
            const auto& ins = text.at(ip);
            switch (ins.code) {
                case OpCode::acc: {
                    acc += ins.arg;
                    break;
                }
                case OpCode::jmp: {
                    ip += ins.arg;
                    continue;
                }
                case OpCode:: nop: {
                    break;
                }
            }
            ip += 1;
        }
        return true;
    }

    inline void reset() {
        ip = 0;
        acc = 0;
    }
};

int main() {
    Program prog{};
    std::set<ssize_t> ip_hist{};
    if (auto v = Program::read(); !v) {
        return 1;
    } else {
        prog = std::move(v.value());
    }

    const auto hook = [&]() -> bool {
        bool ret = !ip_hist.contains(prog.ip);
        ip_hist.insert(prog.ip);
        return ret;
    };

    prog.execute(hook);
    fmt::print("{}\n", prog.acc);

    for (auto& ins : prog.text) {
        auto orig = ins;
        switch (ins.code) {
            case OpCode::acc: {
                continue;
            }
            case OpCode::jmp: {
                ins.code = OpCode::nop;
                break;
            }
            case OpCode::nop: {
                ins.code = OpCode::jmp;
                break;
            }
        }
        ip_hist.clear();
        prog.reset();
        if (prog.execute(hook)) break;
        ins = orig;
    }
    fmt::print("{}\n", prog.acc);

    return 0;
}
