cmake_minimum_required(VERSION 3.16)
project(MamadisiBotC)

set(CMAKE_CXX_STANDARD 98)

add_executable(MamadisiBotC main.cpp)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(MamadisiBotC sleepy-discord)
