cmake_minimum_required(VERSION 3.16)
project(system_experience C)

set(CMAKE_C_STANDARD 99)

add_executable(system_experience main.c experience.h chapter1/task1.c chapter1/task2.c)
add_executable(test test.c)
add_executable(exp1_2 chapter1/exp1_2.c)
