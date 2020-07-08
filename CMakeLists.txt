cmake_minimum_required(VERSION 3.1)
project(system_experience C)

set(CMAKE_C_STANDARD 99)

add_executable(exp1_1 expe1/expe1_1.c)
add_executable(exp1_2 expe1/expe1_2.c)

find_package(Threads REQUIRED)
add_executable(exp2 expe2/expe2.c)
target_link_libraries(exp2 Threads::Threads)

add_executable(exp3_client expe3/msg_client.c)
add_executable(exp3_server expe3/msg_server.c)

add_executable(exp4 expe4/mm.c)