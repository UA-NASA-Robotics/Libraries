cmake_minimum_required(VERSION 3.0)
project(Convert)

# Library

add_library(Convert
        Convert.h
        Convert.c
        )
target_include_directories(Convert PUBLIC .)

# Test

add_executable(ConvertTests
        Convert.t.cpp
        )
find_package(GTest REQUIRED)
target_link_libraries(ConvertTests PRIVATE
        Convert
        GTest::GTest
        GTest::Main
        )