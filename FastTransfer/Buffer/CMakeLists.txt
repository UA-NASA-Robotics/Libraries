cmake_minimum_required(VERSION 3.0)
project(Buffer)

# Library

add_library(Buffer
        Buffer.h
        Buffer.c
        )
target_include_directories(Buffer PUBLIC .)

# Test

add_executable(BufferTests
        Buffer.t.cpp
        )
find_package(GTest REQUIRED)
target_link_libraries(BufferTests PRIVATE Buffer GTest::GTest GTest::Main)