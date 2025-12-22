#include "../include/annotator.h"
#include "../include/printer.h"
#include "../include/tree_unwrapper.h"
#include <iostream>
#include <string>
#include <vector>

#include "slang/driver/Driver.h"
#include "slang/numeric/ConstantValue.h"

using namespace std;
using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;

struct Flags {
    bool write = false;
    bool help = false;
    bool parseCmd = true;
};

Flags parseFlags(int argc, char** argv, vector<int>& input_files) {
    Flags flags;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--write" || arg == "-w") {
            flags.write = true;
        }
        else if (arg == "--help" || arg == "-h") {
            flags.help = true;
        }
        else if (arg[0] != '-') {
            input_files.push_back(i);
        }
        else {
            flags.parseCmd = false;
            cerr << "incorrect flag '" << arg << "'" << endl;
        }
    }

    return flags;
}

int format_one_file(const vector<int>& input_files, char** argv) {
    for (int file_index : input_files) {
        slang::driver::Driver driver;
        driver.addStandardArgs();

        if (!driver.parseCommandLine(1, argv)) {
            return 1;
        }
        if (driver.syntaxTrees.empty()) {
            cerr << "Error: failed to parse input file: " << argv[file_index] << endl;
            return 1;
        }

        auto& source_manager = driver.sourceManager;

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
    }
    return 0;
}

int main(int argc, char** argv) {
    vector<int> input_files;
    Flags flag = parseFlags(argc, argv, input_files);

    if (flag.parseCmd && !input_files.empty()) {
        return format_one_file(input_files, argv);
    }
    return 0;
}
