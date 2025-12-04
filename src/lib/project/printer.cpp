#include "../include/printer.h"

#include <iostream>

vector<my_token> find_tokens(SyntaxNode& node, slang::SourceManager& source_manager) {
    vector<my_token> all_tokens;
    // if (all_tokens.size() == 0){
    // my_token token;
    // token.kind = unknown;
    // token.text = "";
    // token.rule = {};
    // all_tokens.push_back(token);
    // }
    size_t count_child = node.getChildCount();
    for (size_t i = 0; i < count_child; i++) {
        if (auto childNode = node.childNode(i); childNode) {
            auto child_tokens = find_tokens(*childNode, source_manager);
            all_tokens.insert(all_tokens.end(), child_tokens.begin(), child_tokens.end());

            if (node.kind == SyntaxKind::PragmaDirective) {
                my_token new_line_tok;
                new_line_tok.kind = end_dir;
                new_line_tok.rule.dir_end = true;
                all_tokens.push_back(new_line_tok);
            }
        }
        else if (auto token = node.childToken(i); token) {
            if (source_manager.isIncludedFileLoc(token.location())) {
                continue;
            }

            SmallVector<const Trivia*> pending;
            for (const auto& trivia : token.trivia()) {
                pending.push_back(&trivia);
                auto loc = trivia.getExplicitLocation();
                if (loc) {
                    if (!source_manager.isIncludedFileLoc(*loc)) {
                        for (auto t : pending) {
                            auto trivia_tokens = print_trivia(*t, source_manager);
                            all_tokens.insert(all_tokens.end(), trivia_tokens.begin(),
                                              trivia_tokens.end());
                        }
                    }
                    else {
                        if (trivia.kind == TriviaKind::Directive ||
                            trivia.kind == TriviaKind::SkippedSyntax ||
                            trivia.kind == TriviaKind::SkippedTokens) {
                            auto trivia_tokens = print_trivia(trivia, source_manager);
                            all_tokens.insert(all_tokens.end(), trivia_tokens.begin(),
                                              trivia_tokens.end());
                        }
                    }
                    pending.clear();
                }
            }

            for (auto t : pending) {
                auto trivia_tokens = print_trivia(*t, source_manager);
                all_tokens.insert(all_tokens.end(), trivia_tokens.begin(), trivia_tokens.end());
            }

            if (source_manager.isMacroLoc(token.location())) {
                continue;
            }

            my_token new_token;
            if (node.kind == SyntaxKind::SimplePragmaExpression ||
                (node.parent != NULL && node.parent->kind == SyntaxKind::DefineDirective &&
                 token.rawText() == "initial")) {
                new_token.kind = getFormatTokenType(TokenKind::Unknown);
            }
            else if (node.kind == SyntaxKind::MacroUsage) {
                new_token.kind = getFormatTokenType(TokenKind::MacroUsage);
            }
            else if (node.parent != NULL && node.parent->kind == SyntaxKind::MacroActualArgument) {
                new_token.kind = getFormatTokenType(TokenKind::StringLiteral);
            }
            else {
                new_token.kind = getFormatTokenType(token.kind);
            }
            new_token.text = token.rawText();
            all_tokens.push_back(new_token);
        }
    }

    return all_tokens;
}

vector<my_token> print_trivia(slang::parsing::Trivia trivia, slang::SourceManager& source_manager) {
    vector<my_token> tokens;

    if (trivia.kind == TriviaKind::LineComment || trivia.kind == TriviaKind::BlockComment) {
        my_token trivia_token;
        trivia_token.kind = FormatTokenType::comment;

        trivia_token.text = trivia.getRawText();

        // Комментарии управляют своим собственным форматированием
        // trivia_token.rule.newline_after = true;
        // trivia_token.rule.space_before = false;
        // trivia_token.rule.space_after = false;
        // trivia_token.rule.break_before = false;

        tokens.push_back(trivia_token);
    }
    if (trivia.kind == TriviaKind::Directive) {
        auto& syntax = *trivia.syntax();
        auto directive_tokens = find_tokens(syntax, source_manager);
        tokens.insert(tokens.end(), directive_tokens.begin(), directive_tokens.end());
    }
    if (trivia.kind == TriviaKind::SkippedSyntax) {
        auto skipped_tokens = find_tokens(*trivia.syntax(), source_manager);
        tokens.insert(tokens.end(), skipped_tokens.begin(), skipped_tokens.end());
    }
    if (trivia.kind == TriviaKind::SkippedTokens) {
        for (Token t : trivia.getSkippedTokens()) {
            my_token new_t;
            new_t.kind = getFormatTokenType(t.kind);
            new_t.text = t.rawText();
            tokens.push_back(new_t);
        }
    }
    if (trivia.kind == TriviaKind::DisabledText) {
        my_token trivia_tok;
        trivia_tok.kind = getFormatTokenType(TokenKind::Unknown);
        trivia_tok.text = trivia.getRawText();
        tokens.push_back(trivia_tok);
    }

    return tokens;
}
