cmake_minimum_required(VERSION 3.10)

project(ojson VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(json INTERFACE)
target_include_directories(json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(json_test "${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp")
target_link_libraries(json_test PRIVATE json)
