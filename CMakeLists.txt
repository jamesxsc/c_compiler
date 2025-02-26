cmake_minimum_required(VERSION 3.10)
project(c_compiler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(
        -g
        -Wall
        -Wextra
        -Werror
        -Wno-unused-parameter
        -O0
        -rdynamic
        --coverage
)

# Add coverage link flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

# Include directories
include_directories(include)

# Flex and Bison
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
BISON_TARGET(Parser
        src/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
        VERBOSE
)
FLEX_TARGET(Scanner
        src/lexer.flex
        ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.hpp
)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# Get all source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(c_compiler
        ${SOURCES}
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
)

# Set output directory
set_target_properties(c_compiler PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)
