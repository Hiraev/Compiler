cmake_minimum_required(VERSION 3.9)
project(mh-compiler C)

set(CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(mh-compiler compiler/compiler.c compiler/debug/debug.c compiler/lexer.c compiler/synalyzer.c compiler/errors.c compiler/semalyzer.c compiler/polish_notation.c compiler/checker.c compiler/code_generator.c)