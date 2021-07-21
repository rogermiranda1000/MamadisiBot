cmake_minimum_required(VERSION 3.16)
project(MamadisiBotC)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(deps/sleepy-discord)

add_executable(MamadisiBotC main.cpp secrets.h MamadisiBot.h MamadisiBot.cpp)

target_link_libraries(MamadisiBotC sleepy-discord)
target_link_libraries(MamadisiBotC mariadb)
