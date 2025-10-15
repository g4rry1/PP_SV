#include "slang/syntax/SyntaxTree.h"
#include <iostream>


struct my_token {
    slang::parsing::TokenKind kind;
    std::string text;
};


struct layout_item {
    std::string text;
    bool newlineBefore = false;
    bool newlineAfter = false;
    bool spaceBefore = false;
    bool spaceAfter = false;
    int indentLevel = 0;
};

struct format_rule {
    bool newlineBefore = false;
    bool newlineAfter = false;
    bool spaceBefore = false;
    bool spaceAfter = false; 
    bool blockStart = false;
    bool blockEnd = false;
};


int format_tokens(std::vector<my_token>& tokens);