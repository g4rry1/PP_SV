

/* slang throws out all constructions starting with ` from cst at the preprocessing stage,
 we need to figure out how to handle them differently for the formatter*/

/*Although Slang stores places where structures with ` once existed,
 it still does not preserve this for everyone.*/

#include "PP_SV.h"
#include <fstream>
#include <regex>
#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>

#include "slang/parsing/ParserMetadata.h"
#include "slang/syntax/AllSyntax.h"
#include "slang/syntax/SyntaxTree.h"
#include "slang/text/SourceLocation.h"
#include "slang/text/SourceManager.h"

using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;

std::vector<my_token> all_tokens;


void find_tokens(SyntaxNode& root) {

    slang::size_t count_child = root.getChildCount();

    for (slang::size_t i = 0; i < count_child; i++) {
        if (auto childNode = root.childNode(i); childNode) {
            find_tokens(*childNode);
        }
        else if (auto token = root.childToken(i); token) {

            auto list_of_trivia = token.trivia();
            auto count_of_trivia = list_of_trivia.size();

            for (size_t j = 0; j < count_of_trivia; j++) {
                auto trivia = list_of_trivia[j];
                if (trivia.kind == TriviaKind::LineComment ||
                    trivia.kind == TriviaKind::BlockComment) {
                    my_token trivia_token;
                    trivia_token.kind = TokenKind::Unknown;
                    trivia_token.text = trivia.getRawText();
                    all_tokens.push_back(trivia_token);
                }
                if(trivia.kind == TriviaKind::Directive) {
                    auto& syntax = *trivia.syntax();
                    find_tokens(syntax);
                }
                if(trivia.kind == TriviaKind::SkippedSyntax){
                    find_tokens(*trivia.syntax());
                }
                if(trivia.kind == TriviaKind::SkippedTokens){
                        
                    for (Token t : trivia.getSkippedTokens()){
                        my_token new_t;
                        new_t.kind = t.kind;
                        new_t.text = t.rawText();
                        all_tokens.push_back(new_t);
                    }
                }
                if(trivia.kind == TriviaKind::DisabledText){
                    my_token trivia_tok;
                    trivia_tok.kind = TokenKind::Unknown;
                    trivia_tok.text = trivia.getRawText();
                    all_tokens.push_back(trivia_tok);
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

int main(int argc, char** argv) {
    slang::driver::Driver driver;
    driver.addStandardArgs();
    if (!driver.parseCommandLine(argc, argv)) {
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


    find_tokens(tree->root());
    //for(size_t i = 0; i < all_tokens.size(); i++){
        //std::cout << all_tokens[i].kind << " " << all_tokens[i].text << "\n";
    //}

    if (int a = format_tokens(all_tokens)) {
        std::cerr << "openBlocks more than closeBlocks " << a << '\n';
        return 1;
    };

    return 0;
}