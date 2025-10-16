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


::testing::AssertionResult RunPrettyPrinterTest(const std::string& file) {
    fs::path test_dir = "tests_files";
    fs::path relative_path = fs::relative(file, test_dir);

    std::string output_file = "intermediate_files/result_of_test.sv";

    std::string parse_test_file = "bin/slang --parse-only " + file + " > /dev/null 2>&1";
    int result_test_file = std::system(parse_test_file.c_str());

    if (result_test_file != 0) {
        //std::cout << "[  SKIPPED  ] Original file does not parse: "
        //<< relative_path << std::endl;
        return ::testing::AssertionSuccess();
    }

    
    std::string pp = "./build/my_project " + file + " > " + output_file;
    int result_of_pp = std::system(pp.c_str());

    if (result_of_pp != 0) {
        return ::testing::AssertionFailure()
               << "Program returned error for file: " << relative_path;
    }

    
    std::string parse_pp_file =
        "bin/slang --parse-only " + output_file + " > /dev/null 2>&1";
    int result_parsed = std::system(parse_pp_file.c_str());

    if (result_parsed != 0) {
        if(!result_test_file){
            return ::testing::AssertionFailure()
                   << "Pretty printer output failed to parse for file: "
                   << relative_path;
        }
    }

    std::string ast_test_file = "bin/slang --ast-json intermediate_files/orig_ast.json " + file + " > /dev/null 2>&1";
    std::string ast_pp_file = "bin/slang --ast-json intermediate_files/pp_ast.json " + output_file + " > /dev/null 2>&1";
    std::system(ast_test_file.c_str());
    std::system(ast_pp_file.c_str());

    auto size1 = fs::file_size("intermediate_files/orig_ast.json");
    auto size2 = fs::file_size("intermediate_files/pp_ast.json");

    if(size1 != size2){
        return ::testing::AssertionFailure()
        << "Incorrect AST after PP";
    }

    std::string second_formating = "./build/my_project " + output_file + " > " + "intermediate_files/second_format.sv";
    int result_of_2_format = std::system(second_formating.c_str());

    if (result_of_2_format != 0) {
        return ::testing::AssertionFailure()
               << "Program returned error for second_format";
    }

    std::string diff_check ="diff intermediate_files/second_format.sv " + output_file;
    int result_of_diff = std::system(diff_check.c_str());

    if(result_of_diff != 0){
        return ::testing::AssertionFailure()
               << "second format != first format";
    }

    return ::testing::AssertionSuccess();
}




class PrettyPrinterTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PrettyPrinterTest, SystemVerilogParsing) {
    EXPECT_TRUE(RunPrettyPrinterTest(GetParam()));
}


INSTANTIATE_TEST_SUITE_P(
    AllSVTests,
    PrettyPrinterTest,
    ::testing::ValuesIn(collect_sv_files("tests_files"))
);


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    try {
        return RUN_ALL_TESTS();
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }
}
