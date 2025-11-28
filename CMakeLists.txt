cmake_minimum_required(VERSION 3.15)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest REQUIRED)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/lib/slang)
    message(FATAL_ERROR "Slang submodule not found! Run: git submodule update --init --recursive")
endif()

# Добавляем slang ПЕРВЫМ, чтобы он мог управлять зависимостями
add_subdirectory(lib/slang ${CMAKE_BINARY_DIR}/slang_build)

if(EXISTS ${CMAKE_SOURCE_DIR}/lib/prettyprint)
    add_subdirectory(lib/prettyprint ${CMAKE_BINARY_DIR}/prettyprint_build)
endif()

# Автоматический поиск исходных файлов вместо жесткого указания
file(GLOB_RECURSE MAIN_SOURCES "src/lib/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "src/tests/*.cpp")

# Проверка существования файлов
if(NOT MAIN_SOURCES)
    message(WARNING "No source files found in src/lib/")
    # Создаем пустой список чтобы избежать ошибки
    set(MAIN_SOURCES "")
endif()

if(NOT TEST_SOURCES)
    message(WARNING "No test files found in src/tests/")
    set(TEST_SOURCES "")
endif()

add_executable(main ${MAIN_SOURCES})

target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -g
)

target_include_directories(main PRIVATE
    lib/slang/include
    lib/slang/external
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib  # Добавляем путь к исходникам
)

if(EXISTS ${CMAKE_SOURCE_DIR}/lib/prettyprint)
    target_include_directories(main PRIVATE
        lib/prettyprint/src
    )
endif()

# ИСПРАВЛЕНИЕ: Правильное подключение библиотеки Slang
# Используем цель, которую создает сам Slang
target_link_libraries(main PRIVATE slang::slang)

enable_testing()

# Добавляем тесты только если есть test files
if(TEST_SOURCES)
    add_executable(run_tests ${TEST_SOURCES})
    
    target_link_libraries(run_tests PRIVATE 
        GTest::gtest 
        GTest::gtest_main
        pthread 
    )

    target_include_directories(run_tests PRIVATE
        lib/slang/include
        lib/slang/external
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tests  # Добавляем путь к тестам
    )

    target_link_libraries(run_tests PRIVATE slang::slang)

    add_test(NAME SystemTests COMMAND run_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    # Автоматически находим все исходные файлы для форматирования
    file(GLOB_RECURSE FORMAT_SOURCE_FILES
        "src/*.cpp"
        "src/*.h"
        "src/*.hpp"
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -style=file -i ${FORMAT_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files"
        VERBATIM
    )
endif()

# Для отладки - выводим найденные файлы
message(STATUS "Main sources: ${MAIN_SOURCES}")
message(STATUS "Test sources: ${TEST_SOURCES}")
