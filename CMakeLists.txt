cmake_minimum_required(VERSION 3.31)
project(Client_Server_for_ASM C)

set(CMAKE_C_STANDARD 11)

add_executable(server src/server_win.c)
target_link_libraries(server ws2_32)

add_executable(client src/client_win.c)
target_link_libraries(client ws2_32)