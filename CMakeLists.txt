cmake_minimum_required(VERSION 3.14)
project(MiniExpressionCompiler VERSION 1.0.0 LANGUAGES CXX)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings for code quality & correctness
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core static library containing Compiler components
add_library(compiler_core STATIC
    src/Tokenizer.cpp
    src/AST.cpp
    src/Bytecode.cpp
    src/Parser.cpp
    src/Compiler.cpp
)

target_include_directories(compiler_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Main command-line application
add_executable(mini_compiler
    src/main.cpp
)

target_link_libraries(mini_compiler PRIVATE compiler_core)

# Automated verification test suite
enable_testing()

add_executable(test_parser
    tests/test_parser.cpp
)

target_link_libraries(test_parser PRIVATE compiler_core)

add_test(NAME ExpressionCompilerVerification COMMAND test_parser)
