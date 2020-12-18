#pragma once

#include <aoc.h>
#include "tokenizer.h"

using ValueType = ssize_t;

enum class AT {
    val,
    expr,
    pexpr,
};
inline std::ostream& operator<<(std::ostream&& out, AT t) {
    switch (t) {
        case AT::val: return out << "AT::val";
        case AT::expr: return out << "AT::expr";
        case AT::pexpr: return out << "AT::pexpr";
        default: return out;
    }
}

struct ASTNode {
    ptrdiff_t lno{0};
    ptrdiff_t col{0};

    inline virtual constexpr AT type() const = 0;
    inline virtual ValueType value() const = 0;
};

struct ASTValue : public ASTNode {
    ValueType val{0};

    inline constexpr AT type() const final {
        return AT::val;
    }

    inline ValueType value() const final {
        return val;
    }
};

struct ASTExpression : public ASTNode {
    const ASTNode* lhs{nullptr};
    const ASTNode* rhs{nullptr};
    char op{'+'};

    inline constexpr AT type() const final {
        return AT::expr;
    }

    inline ValueType value() const final {
        switch (op) {
            case '+': return lhs->value() + rhs->value();
            case '*': return lhs->value() * rhs->value();
            default: std::abort();
        }
    }
};

struct ASTPExpression : public ASTNode {
    const ASTNode* expr{nullptr};
    bool closed{false};

    inline constexpr AT type() const final {
        return AT::pexpr;
    }

    inline ValueType value() const final {
        return expr->value();
    }
};

using SH_ASTNode = std::shared_ptr<ASTNode>;

static inline SH_ASTNode mknode(const Token& tok) {
    ASTNode* pret{nullptr};
    switch (tok.type) {
        case TT::number: {
            auto* val = new ASTValue();
            aoc::from_chars(tok.sv(), val->val);
            pret = val;
            break;
        }
        case TT::op_plus: [[fallthrough]];
        case TT::op_mult: {
            auto* expr = new ASTExpression();
            expr->op = tok.sv().front();
            pret = expr;
            break;
        }
        case TT::paren_open: {
            pret = new ASTPExpression();
            break;
        }
        default: std::abort();
    }
    pret->lno = tok.lineno;
    pret->col = tok.col;
    return SH_ASTNode(pret);
}

template <size_t N, typename T>
const std::array<T, N>& spop(std::stack<T>& stack) {
    static std::array<T, N> ret{};
    for (size_t i = 0; i < N; i++) {
        ret[N - 1 - i] = stack.top();
        stack.pop();
    }
    return ret;
}

static inline bool collapse_stack(std::stack<ASTNode*>& stack) {
    switch (stack.top()->type()) {
        case AT::pexpr:
        case AT::val: {
            if (stack.size() < 2) break;
            auto [op, rhs] = spop<2>(stack);
            switch (op->type()) {
                case AT::expr: {
                    auto [lhs] = spop<1>(stack);
                    auto* expr = dynamic_cast<ASTExpression*>(op);
                    expr->lhs = lhs;
                    expr->rhs = rhs;
                    stack.push(expr);
                    return collapse_stack(stack);
                }
                case AT::pexpr: {
                    auto* pexpr = dynamic_cast<ASTPExpression*>(op);
                    if (!pexpr->closed) {
                        fmt::print(std::cerr, "Error at {}:{}: expected '+' or '*'\n", rhs->lno, rhs->col);
                        return false;
                    }
                    pexpr->expr = rhs;
                    stack.push(pexpr);
                    break;
                }
                default: {
                    fmt::print(std::cerr, "Error at {}:{}: expected '(' or '+' or '*' before number\n", rhs->lno, rhs->col);
                    return false;
                }
            }
            break;
        }
        case AT::expr: {
            if (stack.size() < 2) break;
            auto [op, rhs] = spop<2>(stack);
            if (op->type() != AT::pexpr) {
                fmt::print(std::cerr, "Error at {}:{}: expected '('\n", rhs->lno, rhs->col);
                return false;
            }
            auto* pexpr = dynamic_cast<ASTPExpression*>(op);
            if (pexpr->closed) {
                fmt::print(std::cerr, "Internal error at {}:{}: closed pexpr\n", rhs->lno, rhs->col);
                return false;
            }
            pexpr->expr = op;
            stack.push(pexpr);
            break;
        }
    }
    return true;
}

inline bool compile(const std::vector<Token>& tokens, std::vector<SH_ASTNode>& ast_storage, ASTNode const*& root) {
    std::stack<ASTNode*> stack{};
    ast_storage.clear();

    if (tokens.empty()) {
        fmt::print(std::cerr, "Cannot compile empty list of tokens!\n");
        return false;
    }

    switch (tokens[0].type) {
        case TT::number: [[fallthrough]];
        case TT::paren_open: {
            ast_storage.push_back(mknode(tokens[0]));
            stack.push(ast_storage.back().get());
            break;
        }
        default: {
            fmt::print(std::cerr, "Error at {}:{}: expected '(' or number at start of expression, got {}\n",
                tokens[0].lineno, tokens[0].col, tokens[0]);
            return false;
        }
    }

    for (size_t idx = 1; idx < tokens.size(); idx++) {
        auto& tok = tokens.at(idx);
        switch (tok.type) {
            case TT::number: {
                ast_storage.push_back(mknode(tok));
                stack.push(ast_storage.back().get());
                if (!collapse_stack(stack)) return false;
                break;
            }
            case TT::op_plus: [[fallthrough]];
            case TT::op_mult: [[fallthrough]];
            case TT::paren_open: {
                ast_storage.push_back(mknode(tok));
                stack.push(ast_storage.back().get());
                break;
            }
            case TT::paren_close: {
                switch (stack.top()->type()) {
                    case AT::val: {
                        if (stack.size() < 2) {
                            fmt::print(std::cerr, "Error at {}:{}: unmatched ')' <\n", tok.lineno, tok.col);
                            return false;
                        }
                        auto [op, val] = spop<2>(stack);
                        break;
                    }
                    case AT::pexpr: {
                        auto* pexpr = dynamic_cast<ASTPExpression*>(stack.top());
                        if (pexpr->closed) {
                            fmt::print(std::cerr, "Error at {}:{}: unmatched ')'\n", tok.lineno, tok.col);
                            return false;
                        }
                        pexpr->closed = true;
                        break;
                    }
                    default: break;
                }
                if (!collapse_stack(stack)) return false;
                break;
            }
            default: break;
        }
    }

    root = stack.top();
    return true;
}
