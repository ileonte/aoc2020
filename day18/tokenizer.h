#pragma once

#include <aoc.h>

enum class TT {
    invalid,
    number,
    paren_open,
    paren_close,
    op_plus,
    op_mult,
};
std::ostream& operator<<(std::ostream& out, TT t) {
    switch (t) {
        case TT::invalid: return out << "TT:invalid";
        case TT::number: return out << "TT::number";
        case TT::paren_open: return out << "TT::paren_open";
        case TT::paren_close: return out << "TT::paren_close";
        case TT::op_plus: return out << "TT::op_plus";
        case TT::op_mult: return out << "TT::op_mult";
        default: return out;
    }
}
struct Token {
    TT type{TT::invalid};
    const char* ptr{nullptr};
    ptrdiff_t len{0};
    ptrdiff_t lineno{0};
    ptrdiff_t col{0};

    inline std::string_view sv() const {
        if (ptr) return std::string_view(ptr, len);
        return std::string_view("");
    }
};
std::ostream& operator<<(std::ostream& out, const Token& t) {
    out << '{' << t.lineno << ':' << t.col << ' ' << t.type;
    if (t.ptr) out << ": \"" << std::string_view(t.ptr, t.len) << '"';
    return out << '}';
}

inline bool tokenize(std::string_view line, ptrdiff_t lineno, std::vector<Token>& tokens) {
    auto* start{line.begin()};
    const auto add_token = [&tokens, line, lineno](TT type, const char* ptr = nullptr, ptrdiff_t len = 0) {
        tokens.push_back(Token {
            .type = type,
            .ptr = ptr,
            .len = len,
            .lineno = lineno,
            .col = ptr - line.begin() + 1,
        });
    };

    tokens.clear();

    while (start < line.end()) {
        if (std::isspace(*start)) {
            start++;
            continue;
        }

        switch (*start) {
            case '+': {
                add_token(TT::op_plus, start, 1);
                break;
            }
            case '*': {
                add_token(TT::op_mult, start, 1);
                break;
            }
            case '(': {
                add_token(TT::paren_open, start, 1);
                break;
            }
            case ')': {
                add_token(TT::paren_close, start, 1);
                break;
            }
            case '0' ... '9': {
                auto* end = start;
                while (end < line.end() && std::isdigit(*end)) end++;
                add_token(TT::number, start, end - start);
                start = end - 1;
                break;
            }
        }

        start += 1;
    }

    return true;
}
