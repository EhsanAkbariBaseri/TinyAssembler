cmake_minimum_required(VERSION 3.6)
project(Assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES asm.c)
add_executable(Assembler ${SOURCE_FILES})