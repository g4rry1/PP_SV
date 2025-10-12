#include "slang/syntax/SyntaxNode.h"
#include <prettyprint.h>
#include <iostream>
#include <queue>

using namespace slang;
using namespace std;
using namespace pp;
using namespace slang::syntax;
using namespace slang::parsing;

shared_ptr<doc> processing_module(shared_ptr<doc> doc,SyntaxNode &root){
    size_t count_of_tokens = root.getChildCount();
    for(size_t i = 0; i < count_of_tokens; i++){
        if (auto child = root.childToken(i); child){
            
            auto list_of_trivia = child.trivia();
            auto count_of_trivia = list_of_trivia.size();
            for(size_t i = 0; i < count_of_trivia; i++){
                if(list_of_trivia[i].kind == TriviaKind::LineComment){
                    doc = doc + " " + (string)list_of_trivia[i].getRawText();
                }
            }

            if(child.kind == TokenKind::InputKeyword || 
               child.kind == TokenKind::OutputKeyword){
                doc = doc + nest(4, line()) + (string)child.rawText();
            }
            else if(child.kind == TokenKind::Semicolon){
                doc = doc + (string)child.rawText() + line();
            }
            else if(child.kind == TokenKind::CloseParenthesis){
                doc = doc + line() + (string)child.rawText();
            }
            else{
                doc = doc + " " + (string)child.rawText();
            }
        } else if (auto child = root.childNode(i); child){
            doc = processing_module(doc, *child);
        }
    }
    return doc;
}