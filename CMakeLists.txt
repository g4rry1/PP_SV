cmake_minimum_required(VERSION 3.15)
project(my_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Путь к Slang
add_subdirectory(${CMAKE_SOURCE_DIR}/../slang ${CMAKE_BINARY_DIR}/slang_build)

# Добавляем исполняемый файл
add_executable(my_project main.cpp)

# Пути к заголовкам
target_include_directories(my_project PRIVATE
    ${CMAKE_SOURCE_DIR}/../slang/include
    ${CMAKE_SOURCE_DIR}/../slang/external
    ${CMAKE_SOURCE_DIR}/../prettyprint/src
)

# Линковка со Slang
target_link_libraries(my_project PRIVATE slang_slang)

