#include "../include/annotator.h"

#include "../include/printer.h"
#include "../include/verilog_token.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

void TokenAnnotator::annotate_tokens(vector<my_token>& tokens) {
    format_context ctx;

    // Инициализируем начальный контекст
    ctx.push_context(CONTEXT_GLOBAL);

    for (size_t i = 1; i < tokens.size() - 1; i++) {
        ctx.prev = (i > 0) ? &tokens[i - 1] : nullptr;
        ctx.curr = &tokens[i];
        ctx.next = (i < tokens.size() - 1) ? &tokens[i + 1] : nullptr;

        update_context(&ctx);

        format_rule rule = analyze_token_environment(&ctx);
        tokens[i].rule = rule;

        ctx.line_start = (rule.newline_after || rule.break_before);
    }
}

format_rule TokenAnnotator::analyze_token_environment(format_context* ctx) {
    format_rule rule;

    rule.space_after = needs_space_after(*ctx->prev, *ctx->curr, *ctx->next, *ctx);
    rule.space_before = needs_space_befor(*ctx->prev, *ctx->curr, *ctx);
    rule.newline_after = needs_new_line_after(*ctx->prev, *ctx->curr, *ctx->next, *ctx);
    rule.break_before = should_break_line(*ctx->prev, *ctx->curr, *ctx->next, *ctx);

    return rule;
}

bool TokenAnnotator::needs_space_after(const my_token& prev, my_token& curr, const my_token& next,
                                       [[maybe_unused]] const format_context& ctx) {
    // if (ctx.in_brackets())
    // {
    //     if (curr.kind == our_operator || curr.kind == unary_operator || curr.kind == colon ||
    //         curr.kind == pattern_operator || curr.kind == delay_operator)
    //     {
    //         return false;
    //     }
    // }
    // if (ctx.in_preprocessor())
    // {
    //     if (curr.kind == preprocessor_directive || next.kind == preprocessor_directive)
    //     {
    //         return true;
    //     }
    //     return false;
    // }
    if (prev.kind == unknown && curr.kind == identifier) {
        return true; // костыль
    }
    if (next.kind == close_group || next.kind == separator) {
        return false;
    }

    if (curr.kind == open_group || curr.kind == event_operator) {
        return false;
    }

    if (prev.kind) {
        return true;
    }

    return true;
}

bool TokenAnnotator::needs_space_befor(const my_token& prev, const my_token& curr,
                                       [[maybe_unused]] const format_context& ctx) {
    if (prev.text[0] == '\\') {
        return true;
    }
    if (ctx.in_group()) {
        if (curr.kind == our_operator || curr.kind == unary_operator || curr.kind == colon ||
            curr.kind == pattern_operator || curr.kind == delay_operator) {
            return false;
        }
    }

    if (curr.kind == event_operator) {
        return true;
    }
    if (curr.kind == keyword_flow && !prev.rule.space_after) {
        return true; // костыль
    }
    if (prev.kind) {
        return false;
    }

    return false;
}

bool TokenAnnotator::should_break_line(const my_token& prev, const my_token& curr,
                                       const my_token& next,
                                       [[maybe_unused]] const format_context& ctx) {
    // if (ctx.in_preprocessor())
    // {
    //     return false;
    // }
    if (prev.rule.newline_after) {
        return false;
    }
    if (prev.kind == keyword_flow && (curr.kind == keyword_flow && curr.text != "end")) {
        return false;
    }
    if (prev.kind == macro_usage) {
        return false;
    }
    if (prev.kind == comment || prev.text == "begin") {
        return true;
    }
    if (prev.kind == end_dir) {
        return true;
    }
    if ((curr.kind == keyword_declaration_open && curr.text != "begin") ||
        curr.kind == keyword_declaration_close) {
        return true;
    }
    if (curr.kind == preprocessor_directive) {
        return true;
    }
    if (next.kind) {
        return false;
    }

    return false;
}

bool TokenAnnotator::needs_new_line_after(const my_token& prev, const my_token& curr,
                                          const my_token& next,
                                          [[maybe_unused]] const format_context& ctx) {
    // if (ctx.in_preprocessor())
    // {
    //     return false;
    // }
    if (next.kind == macro_usage) // костыль
    {
        return false;
    }
    if (curr.kind == keyword_flow && (next.kind == keyword_flow && next.text != "end")) {
        return false;
    }
    if ((curr.kind == separator && next.kind != comment) ||
        (curr.kind == keyword_declaration_close && prev.kind != separator)) {
        return true;
    }
    if ((next.kind == keyword_flow || next.kind == keyword_declaration_close)) {
        return true;
    }
    if (prev.kind) {
        return false;
    }

    return false;
}

bool TokenAnnotator::is_expression_operator(FormatTokenType kind) {
    return kind == unary_operator || kind == our_operator;
}

void TokenAnnotator::update_context(format_context* ctx) {
    if (!ctx->curr)
        return;

    if (ctx->curr->kind == open_group) {
        ctx->push_context(CONTEXT_GROUP);
    }
    else if (ctx->curr->kind == close_group) {
        // Ищем соответствующую открывающую группу
        while (!ctx->context_stack.empty() && ctx->current_context() != CONTEXT_GROUP &&
               ctx->current_context() != CONTEXT_GLOBAL) {
            ctx->pop_context();
        }
        if (!ctx->context_stack.empty() && ctx->current_context() == CONTEXT_GROUP) {
            ctx->pop_context();
        }
    }

    if (ctx->curr->kind == preprocessor_directive) {
        ctx->push_context(CONTEXT_PREPROCESSOR);
    }
    else if (ctx->curr->rule.dir_end && ctx->in_preprocessor()) {
        ctx->pop_context();
    }

    // Обработка ключевых слов модулей
    if (ctx->curr->kind == keyword_declaration_open) {
        if (ctx->curr->text == "module" || ctx->curr->text == "interface" ||
            ctx->curr->text == "package") {
            ctx->push_context(CONTEXT_MODULE);
        }
    }
    else if (ctx->curr->kind == keyword_declaration_close) {
        if (ctx->curr->text.find("end") == 0) {
            while (!ctx->context_stack.empty() && ctx->current_context() != CONTEXT_MODULE &&
                   ctx->current_context() != CONTEXT_GLOBAL) {
                ctx->pop_context();
            }
            if (!ctx->context_stack.empty() && ctx->current_context() == CONTEXT_MODULE) {
                ctx->pop_context();
            }
        }
    }

    // Обработка case
    if (ctx->curr->kind == keyword_flow) {
        if (ctx->curr->text == "case" || ctx->curr->text == "casex" || ctx->curr->text == "casez") {
            ctx->push_context(CONTEXT_CASE);
        }
        else if (ctx->curr->text == "endcase") {
            while (!ctx->context_stack.empty() && ctx->current_context() != CONTEXT_CASE) {
                ctx->pop_context();
            }
            if (!ctx->context_stack.empty()) {
                ctx->pop_context();
            }
        }
    }
}
