#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

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

// Функция, которая выполняет тест одного файла
::testing::AssertionResult RunPrettyPrinterTest(const std::string& file) {
    fs::path test_dir = "tests_files";
    fs::path relative_path = fs::relative(file, test_dir);

    std::string output_file = "tests_files/result_of_test.sv";

    // 1. Запуск программы pretty-printer
    std::string command_1 = "./build/my_project " + file + " > " + output_file;
    int result1 = std::system(command_1.c_str());

    if (result1 != 0) {
        return ::testing::AssertionFailure()
               << "Program returned error for file: " << relative_path;
    }

    // 2. Проверка, что результат парсится slang’ом
    std::string command_2 =
        "~/pretty/slang/build/bin/slang --parse-only " + output_file + " > /dev/null 2>&1";
    int result_parsed = std::system(command_2.c_str());

    if (result_parsed != 0) {
        // Проверим, может быть исходный тест-файл тоже не парсится
        std::string command_3 =
            "~/pretty/slang/build/bin/slang --parse-only " + file + " > /dev/null 2>&1";
        int result_test_file = std::system(command_3.c_str());

        if (result_test_file != 0) {
             std::cout << "[  SKIPPED  ] Original file does not parse: "
              << relative_path << std::endl;
                return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                   << "Pretty printer output failed to parse for file: "
                   << relative_path;
        }
    }

    return ::testing::AssertionSuccess();
}

// ----------- ТЕСТЫ ------------

// Мы создаем параметризованный тест, чтобы каждый .sv файл стал отдельным тестом
class PrettyPrinterTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PrettyPrinterTest, SystemVerilogParsing) {
    EXPECT_TRUE(RunPrettyPrinterTest(GetParam()));
}

// Генерация тестов
INSTANTIATE_TEST_SUITE_P(
    AllSVTests,
    PrettyPrinterTest,
    ::testing::ValuesIn(collect_sv_files("tests_files"))
);

// Главная функция
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    try {
        return RUN_ALL_TESTS();
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }
}
