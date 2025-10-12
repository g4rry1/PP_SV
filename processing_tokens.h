#pragma once
#include "slang/syntax/SyntaxNode.h"
#include <prettyprint.h>

std::shared_ptr<pp::doc> processing_module(std::shared_ptr<pp::doc> doc, slang::syntax::SyntaxNode &root);