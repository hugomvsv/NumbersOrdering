cmake_minimum_required(VERSION 3.16)
project(Projeto_SO_socket C)

set(CMAKE_C_STANDARD 99)

add_executable(Projeto_SO_socket client.c server.c client.c)