cmake_minimum_required(VERSION 3.10)
project(AdventOfCodeTemplate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(solution main.cpp)

# Set output directory
set_target_properties(solution PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
