#pragma once
#include "../include/verilog_token.h"
#include <memory>
#include <vector>

struct format_rule {
    bool space_before = false;
    bool space_after = false;
    bool newline_after = false;
    bool break_before = false;
    int tab = 0;
    bool dir_end = false;
};

struct my_token {
    FormatTokenType kind;
    std::string text;
    format_rule rule;
};

struct format_context {
    my_token* prev = nullptr;
    my_token* curr = nullptr;
    my_token* next = nullptr;

    std::vector<ContextType> context_stack;

    bool in_preprocessor() const {
        return !context_stack.empty() && context_stack.back() == CONTEXT_PREPROCESSOR;
    }

    bool in_case() const {
        for (ContextType ctx : context_stack) {
            if (ctx == CONTEXT_CASE)
                return true;
        }
        return false;
    }

    int block_depth = 0;
    bool line_start = true;

    ContextType current_context() const {
        return context_stack.empty() ? CONTEXT_GLOBAL : context_stack.back();
    }

    void push_context(ContextType ctx) { context_stack.push_back(ctx); }

    void pop_context() {
        if (!context_stack.empty()) {
            context_stack.pop_back();
        }
    }

    bool has_context(ContextType ctx) const {
        for (ContextType c : context_stack) {
            if (c == ctx)
                return true;
        }
        return false;
    }

    bool in_group() const {
        return !context_stack.empty() && context_stack.back() == CONTEXT_GROUP;
    }

    bool in_module() const {
        for (ContextType ctx : context_stack) {
            if (ctx == CONTEXT_MODULE)
                return true;
        }
        return false;
    }
};

class TokenAnnotator {
public:
    // Основная функция аннотации с контекстом
    static void annotate_tokens(vector<my_token>& tokens);

    // Функции для анализа пар токенов
    static format_rule analyze_token_environment(format_context* ctx);

    // Вспомогательные функции
    static bool needs_space_after(const my_token& prev, my_token& curr, const my_token& next,
                                  const format_context& ctx);
    static bool needs_space_befor(const my_token& prev, const my_token& curr,
                                  const format_context& ctx);
    static bool needs_new_line_after(const my_token& prev, const my_token& curr,
                                     const my_token& next,
                                     [[maybe_unused]] const format_context& ctx);
    static bool should_break_line(const my_token& prev, const my_token& curr, const my_token& next,
                                  [[maybe_unused]] const format_context& ctx);

    static bool is_expression_operator(FormatTokenType kind);

    static void update_context(format_context* ctx);

private:
    static ContextType get_context_for_group(const std::string& text);
};
