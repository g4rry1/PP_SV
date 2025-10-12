#pragma once
#include "slang/syntax/SyntaxNode.h"
#include <prettyprint.h>

void processing_module(slang::syntax::SyntaxNode::Token token, std::shared_ptr<pp::doc> doc, slang::syntax::SyntaxNode &root);