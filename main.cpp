#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>
#include <iostream>
#include <prettyprint.h>
#include "slang/syntax/SyntaxPrinter.h"

#include "slang/parsing/ParserMetadata.h"
#include "slang/syntax/SyntaxNode.h"
#include "slang/syntax/SyntaxTree.h"
#include "slang/text/SourceLocation.h"
#include "slang/text/SourceManager.h"

using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;


void print_childs(const SyntaxNode &root){
    //std::cout << "root : " << root.kind << "\n";
    //std::cout << "subtree end \n";

    slang::size_t count_child = root.getChildCount();
    
    for(slang::size_t i = 0; i < count_child; i++){
        if (auto childNode = root.childNode(i); childNode){
            print_childs(*childNode);
        }
            
        else if (auto token = root.childToken(i); token){
            std::cout << "token start\n";
            std::cout << token.toString() << "\n";
            std::cout << token.kind << "\n";
            std::cout << token.rawText() << "\n";
            std::cout << "token end\n";
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