cmake_minimum_required(VERSION 3.15)
project(my_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest REQUIRED)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/lib/slang)
    message(FATAL_ERROR "Slang submodule not found! Run: git submodule update --init --recursive")
endif()

add_subdirectory(lib/slang ${CMAKE_BINARY_DIR}/slang_build)

if(EXISTS ${CMAKE_SOURCE_DIR}/lib/prettyprint)
    add_subdirectory(lib/prettyprint ${CMAKE_BINARY_DIR}/prettyprint_build)
endif()

add_executable(my_project src/lib/main.cpp src/lib/processing_tokens.cpp)

target_compile_options(my_project PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -g
)

target_include_directories(my_project PRIVATE
    lib/slang/include
    lib/slang/external
)

if(EXISTS ${CMAKE_SOURCE_DIR}/lib/prettyprint)
    target_include_directories(my_project PRIVATE
        lib/prettyprint/src
    )
endif()

target_link_libraries(my_project PRIVATE slang_slang)

# # Если prettyprint есть, линкуем и его
# if(EXISTS ${CMAKE_SOURCE_DIR}/lib/prettyprint)
#     target_link_libraries(my_project PRIVATE prettyprint)
# endif()

enable_testing()

add_executable(run_tests ${CMAKE_SOURCE_DIR}/src/tests/tests.cpp)
enable_testing()
add_test(NAME run_all_tests COMMAND run_tests)

target_link_libraries(run_tests PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    pthread 
)

target_include_directories(run_tests PRIVATE
    lib/slang/include
    lib/slang/external
)

target_link_libraries(run_tests PRIVATE slang_slang)

add_test(NAME SystemTests COMMAND run_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)


find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    set(FORMAT_SOURCE_FILES
        src/lib/main.cpp
        src/lib/PP_SV.h
        src/lib/processing_tokens.cpp
        src/tests/tests.cpp
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -style=file -i ${FORMAT_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting specified source files"
        VERBATIM
    )
endif()
