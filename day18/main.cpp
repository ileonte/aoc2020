#include <aoc.h>
#include "tokenizer.h"
#include "ast.h"

int main() {
    std::string line{};
    std::vector<Token> tokens{};
    std::vector<SH_ASTNode> ast_storage{};
    const ASTNode* ast_root{nullptr};
    ptrdiff_t lineno{1};
    ValueType part1{0};

    // tokens.reserve(256);
    // ast_storage.reserve(256);

    // while (std::getline(std::cin, line)) {
    //     auto sv = aoc::trim(line);
    //     if (!tokenize(sv, lineno, tokens)) return 1;
    //     if (!compile(tokens, ast_storage, ast_root)) return 1;
    //     part1 += ast_root->value();
    //     lineno += 1;
    // }

    // fmt::print("{}\n", part1);

    tokenize("1 + (1 * 2)", 1, tokens);
    compile(tokens, ast_storage, ast_root);

    return 0;
}
