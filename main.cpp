#include "processing_tokens.h"
#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>
#include <iostream>

#include "slang/parsing/ParserMetadata.h"
#include "slang/syntax/SyntaxTree.h"
#include "slang/text/SourceLocation.h"
#include "slang/text/SourceManager.h"

using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;


void print_childs(SyntaxNode &root){

    auto doc = pp::nil();

    slang::size_t count_child = root.getChildCount();
    
    for(slang::size_t i = 0; i < count_child; i++){
        if (auto childNode = root.childNode(i); childNode){
            print_childs(*childNode);
        }
        else if (auto token = root.childToken(i); token){
            if(token.kind == TokenKind::ModuleKeyword){
                auto internal_of_modul = root.childNode(i + 1);
                processing_module(token, doc, *internal_of_modul);
                return;
            }
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
    print_childs(tree->root());

    return 0;
}