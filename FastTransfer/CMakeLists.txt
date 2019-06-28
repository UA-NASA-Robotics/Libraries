cmake_minimum_required(VERSION 3.0)
project(FastTransfer)

add_subdirectory(Buffer)
add_subdirectory(Convert)

# Library

add_library(FT FT.h FT.c)
target_link_libraries(FT PRIVATE Buffer Convert)

# Tests

add_executable(FastTransferTests FT.t.cpp)
find_package(GTest REQUIRED)
target_link_libraries(FastTransferTests PRIVATE
        FT
        GTest::GTest
        GTest::Main
        )
target_compile_options(FastTransferTests PRIVATE -std=c++11)
