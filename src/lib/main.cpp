#include "PP_SV.h"
#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>

#include "slang/parsing/ParserMetadata.h"
#include "slang/syntax/SyntaxTree.h"
#include "slang/text/SourceLocation.h"
#include "slang/text/SourceManager.h"
#include "slang/syntax/AllSyntax.h"
#include <fstream>
#include <regex>

using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;


std::vector<my_token> all_tokens;


//std::vector<std::string> file_directives;
//int cur_derective = 0;


/*std::vector<std::string> read_directives_from_original_file(const std::string& filename) {
    std::vector<std::string> directives;
    std::ifstream file(filename);
    std::string line;
    std::regex directive_pattern(R"(^\s*`\w+.*)");
    
    while (std::getline(file, line)) {
        if (std::regex_match(line, directive_pattern)) {
            // Убираем начальные пробелы
            size_t start = line.find('`');
            if (start != std::string::npos) {
                directives.push_back(line.substr(start));
            }
        }
    }
    return directives;
}*/

void find_tokens(SyntaxNode &root){  

    slang::size_t count_child = root.getChildCount();
    
    for(slang::size_t i = 0; i < count_child; i++){
        if (auto childNode = root.childNode(i); childNode){
            find_tokens(*childNode);
        }
        else if (auto token = root.childToken(i); token){

            auto list_of_trivia = token.trivia();
            auto count_of_trivia = list_of_trivia.size();

            for(size_t j = 0; j < count_of_trivia; j++){
                if(list_of_trivia[j].kind == TriviaKind::LineComment){
                    my_token trivia_token;
                    trivia_token.kind = TokenKind::Unknown;
                    trivia_token.text = list_of_trivia[j].getRawText();
                    all_tokens.push_back(trivia_token);
                }
                else if(list_of_trivia[j].kind == TriviaKind::Directive){
                    //my_token directive_token;
                    //directive_token.kind = TokenKind::Unknown;
                    //directive_token.text = file_directives[cur_derective];
                    //cur_derective++;
                    //all_tokens.push_back(directive_token);
                }
            }

            my_token new_token;
            new_token.kind = token.kind;
            new_token.text = token.rawText();
            all_tokens.push_back(new_token);
        } 
    }
    return;
}

int main(int argc, char **argv){
    slang::driver::Driver driver;
    driver.addStandardArgs();
    if (!driver.parseCommandLine(argc, argv)){
            return 1;
    }
    bool ok = driver.parseAllSources();
    if (!ok) {
        std::cerr << "error: failed to parse input files\n";
        return 1;
    }

    if (driver.syntaxTrees.empty()) {
        std::cerr << "error: failed to parse input files\n";
        return 1;
    }


    auto tree = driver.syntaxTrees.back();

    //file_directives = read_directives_from_original_file(argv[1]);

    find_tokens(tree->root());

    if(int a = format_tokens(all_tokens)){
        std::cerr << "openBlocks more than closeBlocks " << a << '\n';
        return 1;
    };

    return 0;
}