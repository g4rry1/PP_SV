#include <slang/driver/Driver.h>
#include <slang/syntax/SyntaxPrinter.h>
#include <iostream>
#include <prettyprint.h>

using namespace slang;
using namespace slang::syntax;
using namespace slang::parsing;


int main(int argc, char **argv){
    slang::driver::Driver driver;
    driver.addStandardArgs();
    if (!driver.parseCommandLine(argc, argv)){
            return 1;
    }
    bool ok = driver.parseAllSources();
    if (!ok) {
        std::cout << "error: failed to parse input files\n";
        return 1;
    }

    if (driver.syntaxTrees.empty()) {
        std::cout << "error: failed to parse input files\n";
        return 1;
    }
    auto tree = driver.syntaxTrees.back();
    SyntaxPrinter printer(driver.sourceManager);
    printer.setIncludeDirectives(true);
    printer.setSquashNewlines(false);
    std::string output = printer.print(*tree).str();
    printf("%s", output.c_str());
    return 0;
}