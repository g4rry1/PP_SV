#pragma once
#include "../include/annotator.h"
#include "verilog_token.h"
#include <fstream>
#include <regex>
#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>

#include "slang/parsing/ParserMetadata.h"
#include "slang/syntax/AllSyntax.h"
#include "slang/syntax/SyntaxTree.h"
#include "slang/text/SourceLocation.h"
#include "slang/text/SourceManager.h"

using namespace std;
using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;

vector<my_token> find_tokens(SyntaxNode& node, slang::SourceManager& source_manager);

vector<my_token> print_trivia(slang::parsing::Trivia trivia, slang::SourceManager& source_manager);

int format_tokens(vector<my_token>& tokens);
