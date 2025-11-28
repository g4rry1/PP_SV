#pragma once
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

enum FormatTokenType {
    unknown, // 0 Группа 9: Неизвестные/служебные токены

    comment,

    // Группа 1: Идентификаторы и системные имена
    identifier,        // 1 Обычные идентификаторы
    system_identifier, // 2 Системные функции ($display, $time)

    // Группа 2: Ключевые слова структурные (управление блоками и отступами)
    keyword_declaration_open,  // 3 Начало блоков: module, function, task
    keyword_declaration_close, // 4 Конец блоков: endmodule, endfunction
    keyword_flow,              // 5 Управление потоком: if, for, while
    keyword_assertion,         // 6 Assertions: assert, property
    keyword_other,             // 7

    // Группа 3: Литералы (базовое форматирование)
    numeric_base,    // 8 Основания чисел: 'd, 'h, 'b
    numeric_literal, // 9 Числа: 123, 8'hFF
    string_literal,  // 10 Строки: "text"
    time_literal,    // 11 Время: 10ns, 5.2us

    // Группа 4: Операторы (пробелы вокруг)
    unary_operator,   // 12 Унарные: ! ~ & | ^
    our_operator,     // 13 Бинарные: + - * / == && // Присваивание: = += -=    // Тернарный: ? :
    delay_operator,   // 14 Задержки: # ##
    pattern_operator, // 15 Паттерны: -> => |-> |=>

    // Группа 5: Группирующие символы (управление пробелами внутри)
    open_group,  // 16 Открывающие: ( { [ '{
    close_group, // 17 Закрывающие: ) } ]

    // Группа 6: Разделители и иерархия (специфичные правила пробелов)
    separator, // 18 Разделители: , ;
    colon,     // 19 Двоеточие (разные контексты)
    hierarchy, // 20 Иерархия: . ::

    // Группа 7: Edge descriptors (специальные ключевые слова)
    edge_descriptor, // 21 posedge, negedge

    // Группа 8: Препроцессор и макросы (минимальное форматирование)
    preprocessor_directive, // 22 Директивы: `define, `include
    macro_usage,            // 23 Макросы

    event_operator,

    end_dir
};

enum ContextType {
    CONTEXT_GLOBAL,
    CONTEXT_GROUP,        // Обобщенный контекст для всех скобок
    CONTEXT_PREPROCESSOR, // В директиве препроцессора
    CONTEXT_MODULE,       // В модуле/interface/package
    CONTEXT_CASE          // В case блоке
};

extern unordered_map<slang::parsing::TokenKind, FormatTokenType> token_to_format_type;

FormatTokenType getFormatTokenType(TokenKind kind);
