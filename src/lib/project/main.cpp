#include "../include/annotator.h"
#include "../include/printer.h"
#include "../include/tree_unwrapper.h"
#include <iostream>

using namespace std;
using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: incorrect arguments\n");
        return 1;
    }

    slang::driver::Driver driver;
    driver.addStandardArgs();
    if (!driver.parseCommandLine(argc, argv)) {
        return 1;
    }

    auto& source_manager = driver.sourceManager;

    if (!driver.parseAllSources()) {
        fprintf(stderr, "Error: failed to parse input files\n");
        return 1;
    }

    if (driver.syntaxTrees.empty()) {
        fprintf(stderr, "Error: failed to parse input files\n");
        return 1;
    }

    shared_ptr<slang::syntax::SyntaxTree> tree = driver.syntaxTrees.back();
    std::vector<my_token> all_tokens = find_tokens(tree->root(), source_manager);

    TokenAnnotator::annotate_tokens(all_tokens);
    TokenUnwrapper::unwrapper_token(all_tokens);

    for (size_t i = 0; i < all_tokens.size(); i++) {
        auto& token = all_tokens[i];
        auto rule = token.rule;
        if (token.rule.break_before) {
            std::cout << '\n';
        }
        std::cout << string(token.rule.tab * 2, ' ');
        if (rule.space_before) {
            std::cout << ' ';
        }
        // std::cout << " " << token.kind << " ";
        std::cout << token.text;

        if (rule.space_after) {
            std::cout << ' ';
        }

        if (rule.newline_after) {
            std::cout << '\n';
        }
        if (rule.dir_end) {
            std::cout << '\n';
        }
    }
    std::cout << '\n';
    return 0;
}
