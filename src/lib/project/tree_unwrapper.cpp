#include "../include/tree_unwrapper.h"

#include "../include/annotator.h"
#include "../include/verilog_token.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

void TokenUnwrapper::unwrapper_token(vector<my_token>& tokens) {
    tabulating(tokens);
}

int TokenUnwrapper::tab = 0;

void TokenUnwrapper::tabulating(vector<my_token>& tokens) {
    for (size_t i = 1; i < tokens.size() - 1; i++) {
        if ((tokens[i].kind == keyword_flow && tokens[i].text == "begin") ||
            (tokens[i].kind == open_group && tokens[i].text == "{") || // fix!!!!
            tokens[i].kind == keyword_declaration_open) {
            tab++;
        }
        else if ((tokens[i].kind == keyword_flow && tokens[i].text == "end") ||
                 (tokens[i].kind == close_group && tokens[i].text == "}") || // fix!!!
                 tokens[i].kind == keyword_declaration_close) {
            tab = max(0, tab - 1);
        }
        if (tokens[i].rule.break_before || tokens[i - 1].rule.newline_after) {
            if ((tokens[i].kind == keyword_flow && tokens[i].text == "begin") ||
                (tokens[i].kind == open_group && tokens[i].text == "{") ||
                tokens[i].kind == keyword_declaration_open || (tokens[i].kind == close_group)) {
                tokens[i].rule.tab = max(0, tab - 1);
            }
            else {
                tokens[i].rule.tab = tab;
            }
        }
    }
}