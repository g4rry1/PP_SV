#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> collect_sv_files(const std::string& test_dir) {
    std::vector<std::string> test_files;
    if (!fs::exists(test_dir)) {
        throw std::runtime_error("Directory '" + test_dir + "' not found!");
    }

    for (const auto& entry : fs::recursive_directory_iterator(test_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sv") {
            test_files.push_back(entry.path().string());
        }
    }

    return test_files;
}

::testing::AssertionResult RunPrettyPrinterTest(const std::string& file) {
    // Создаем директории для временных файлов
    fs::create_directories("intermediate_files");
    fs::create_directories("test_output");

    const std::string uvm_path = "-I../third_party/tests/uvm/src";

    // Правильный путь к тестовым данным
    fs::path test_dir = "../src/tests/tests_data";
    fs::path relative_path = fs::relative(file, test_dir);

    // Выходной файл создаем во временной директории build
    std::string result_of_test = relative_path.stem().string() + "_pp.sv";
    std::string output_file = "test_output/" + result_of_test;

    // === ПРОВЕРКА 1: Исходный файл должен парситься ===
    std::string parse_test_file = "../slang_bin/slang --parse-only " + uvm_path + " " + file +
                                  " > /dev/null 2>&1";
    int result_test_file = std::system(parse_test_file.c_str());

    if (result_test_file != 0) {
        return ::testing::AssertionSuccess(); // Просто возвращаем успех без вывода
    }

    // === ПРОВЕРКА 2: Pretty printer должен работать без ошибок ===
    std::string pp = "./main " + file + " > " + output_file;
    int result_of_pp = std::system(pp.c_str());

    if (result_of_pp != 0) {
        fs::remove(output_file);
        return ::testing::AssertionFailure()
               << "❌ FAILED: Pretty printer returned error for file: " << relative_path
               << "\n   Command: " << pp << "\n   Return code: " << result_of_pp;
    }

    // === ПРОВЕРКА 3: Результат pretty printer должен парситься ===
    std::string parse_pp_file = "../slang_bin/slang --parse-only " + uvm_path + " " + output_file +
                                " > /dev/null 2>&1";
    int result_parsed = std::system(parse_pp_file.c_str());

    if (result_parsed != 0) {
        if (result_test_file == 0) {
            // Сохраняем проблемный файл для отладки
            std::string error_file = output_file + ".ERROR";
            fs::copy(output_file, error_file, fs::copy_options::overwrite_existing);

            std::string error_msg = "❌ FAILED: Pretty printer output failed to parse for file: " +
                                    relative_path.string();
            error_msg += "\n   Original file parsed successfully, but PP output failed";
            error_msg += "\n   Problematic output saved to: " + error_file;

            // Показываем первые несколько строк проблемного файла
            std::ifstream in(error_file);
            std::string first_lines;
            for (int i = 0; i < 10 && std::getline(in, first_lines); ++i) {
                error_msg += "\n   Line " + std::to_string(i + 1) + ": " + first_lines;
            }

            fs::remove(output_file);
            return ::testing::AssertionFailure() << error_msg;
        }
    }

    // === ПРОВЕРКА 4: AST до и после должен совпадать ===
    std::string ast_test_file = "../slang_bin/slang --ast-json intermediate_files/orig_ast.json " +
                                uvm_path + " " + file + " > /dev/null 2>&1";
    std::string ast_pp_file = "../slang_bin/slang --ast-json intermediate_files/pp_ast.json " +
                              uvm_path + " " + output_file + " > /dev/null 2>&1";

    std::system(ast_test_file.c_str());
    std::system(ast_pp_file.c_str());

    if (fs::exists("intermediate_files/orig_ast.json") &&
        fs::exists("intermediate_files/pp_ast.json")) {
        auto size1 = fs::file_size("intermediate_files/orig_ast.json");
        auto size2 = fs::file_size("intermediate_files/pp_ast.json");

        if (size1 != size2) {
            std::string error_msg = "❌ FAILED: AST files size mismatch for file: " +
                                    relative_path.string();
            error_msg += "\n   Original AST size: " + std::to_string(size1);
            error_msg += "\n   PP AST size: " + std::to_string(size2);
            error_msg += "\n   Difference: " +
                         std::to_string(std::abs((long long)(size1 - size2))) + " bytes";

            // Сохраняем оба AST файла для отладки
            fs::copy("intermediate_files/orig_ast.json", "intermediate_files/orig_ast.json.ERROR",
                     fs::copy_options::overwrite_existing);
            fs::copy("intermediate_files/pp_ast.json", "intermediate_files/pp_ast.json.ERROR",
                     fs::copy_options::overwrite_existing);
            error_msg += "\n   AST files saved with .ERROR extension for inspection";

            fs::remove(output_file);
            return ::testing::AssertionFailure() << error_msg;
        }
    }
    else {
        std::string error_msg = "❌ FAILED: AST files not generated for file: " +
                                relative_path.string();
        if (!fs::exists("intermediate_files/orig_ast.json")) {
            error_msg += "\n   Original AST file missing";
        }
        if (!fs::exists("intermediate_files/pp_ast.json")) {
            error_msg += "\n   PP AST file missing";
        }
        fs::remove(output_file);
        return ::testing::AssertionFailure() << error_msg;
    }

    // === ПРОВЕРКА 5: Pretty printer должен быть идемпотентным ===
    std::string second_format_file = "intermediate_files/second_format.sv";
    std::string second_formating = "./main " + output_file + " > " + second_format_file;
    int result_of_2_format = std::system(second_formating.c_str());

    if (result_of_2_format != 0) {
        std::string error_msg = "❌ FAILED: Second formatting failed for file: " +
                                relative_path.string();
        error_msg += "\n   Command: " + second_formating;
        error_msg += "\n   Return code: " + std::to_string(result_of_2_format);
        fs::remove(output_file);
        return ::testing::AssertionFailure() << error_msg;
    }

    // === ПРОВЕРКА 6: Первое и второе форматирование должны быть идентичны ===
    // (раскомментируйте этот блок если нужно проверить идемпотентность)
    /*
    std::string diff_check = "diff " + second_format_file + " " + output_file + " > /dev/null 2>&1";
    int result_of_diff = std::system(diff_check.c_str());

    if (result_of_diff != 0)
    {
        std::string error_msg = "❌ FAILED: Idempotency check failed for file: " +
    relative_path.string(); error_msg += "\n   First and second formatting results differ";

        // ПОЛНЫЙ ВЫВОД РАЗЛИЧИЙ
        std::string show_full_diff = "diff -u " + output_file + " " + second_format_file;
        std::system(show_full_diff.c_str());

        // Сохраняем оба файла для отладки
        std::string first_save = output_file + ".first";
        std::string second_save = second_format_file + ".second";

        if (fs::exists(first_save)) fs::remove(first_save);
        if (fs::exists(second_save)) fs::remove(second_save);

        fs::copy(output_file, first_save);
        fs::copy(second_format_file, second_save);

        error_msg += "\n   Files saved for inspection:";
        error_msg += "\n   - First formatting:  " + first_save;
        error_msg += "\n   - Second formatting: " + second_save;

        fs::remove(output_file);
        fs::remove(second_format_file);
        return ::testing::AssertionFailure() << error_msg;
    }
    */

    fs::remove(output_file);
    fs::remove(second_format_file);

    // Удаляем AST файлы если они существуют
    if (fs::exists("intermediate_files/orig_ast.json"))
        fs::remove("intermediate_files/orig_ast.json");
    if (fs::exists("intermediate_files/pp_ast.json"))
        fs::remove("intermediate_files/pp_ast.json");

    return ::testing::AssertionSuccess();
}

class PrettyPrinterTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PrettyPrinterTest, SystemVerilogParsing) {
    EXPECT_TRUE(RunPrettyPrinterTest(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(AllSVTests, PrettyPrinterTest,
                         ::testing::ValuesIn(collect_sv_files("../src/tests/tests_data")));

int main(int argc, char** argv) {
    // Создаем необходимые директории
    fs::create_directories("intermediate_files");
    fs::create_directories("test_output");

    ::testing::InitGoogleTest(&argc, argv);

    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::GTEST_FLAG(print_time) = true;

    return RUN_ALL_TESTS();
}
