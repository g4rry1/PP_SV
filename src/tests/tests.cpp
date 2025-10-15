#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::string test_dir = "tests_files";
    std::vector<std::string> test_files;
    
    if (!fs::exists(test_dir)) {
        std::cerr << "Error: Directory '" << test_dir << "' not found!" << std::endl;
        return 1;
    }
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(test_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".sv") {
                test_files.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
        return 1;
    }
    
    if (test_files.empty()) {
        std::cout << "No .sv files found in directory: " << test_dir << std::endl;
        return 0;
    }
    
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& file : test_files) {
        fs::path relative_path = fs::relative(file, test_dir);
        
        std::string command_1 = "./build/my_project " + file + " > tests_files/result_of_test.sv";
        int result1 = std::system(command_1.c_str());
        
        if (result1 != 0) {
            std::cout << "❌ FAILED: Your program returned error" << std::endl;
            std::cout << "Testing: " << relative_path.string() << std::endl;
            failed++;
            std::cout << "------------------------" << std::endl;
            continue;
        }
        
        std::string command_2 = "~/pretty/slang/build/bin/slang --parse-only tests_files/result_of_test.sv > /dev/null 2>&1";
        int result_of_parsing = std::system(command_2.c_str());
        
        if (result_of_parsing == 0) {
            //std::cout << "✅ PASSED" << std::endl;
            passed++;
        } else {
            //std::cout << "❌ FAILED: slang parsing failed" << std::endl;
            //std::cout << "Testing: " << relative_path.string() << std::endl;
            failed++;
            //std::cout << "------------------------" << std::endl;
        }
    }
    
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total files: " << test_files.size() << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Success rate: " << (passed * 100 / test_files.size()) << "%" << std::endl;
    
    return failed == 0 ? 0 : 1;
}