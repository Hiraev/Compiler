cmake_minimum_required(VERSION 3.9)
project(Compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(Compiler compiler/lexer.c compiler/compiler.c compiler/debug.c compiler/synalyzer.c)