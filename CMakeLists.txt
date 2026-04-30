cmake_minimum_required(VERSION 4.2)
project(lambda LANGUAGES CXX)

option(ENABLE_WARNINGS_AS_ERRORS "Treat C++ warnings as compiler error" ON)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(dependencies)
include(unity)
include(warning)

add_subdirectory(lambda-core)
add_subdirectory(lambda-runtime)

enable_testing()
add_subdirectory(tests)