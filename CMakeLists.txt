cmake_minimum_required(VERSION 3.16)
project(MamadisiBotC)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Debug) # Debug (gdb & valgrind)
set(CMAKE_BUILD_TYPE RELEASE)

set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(APP_ROOT_DEPENDENCIES_DIR ${APP_ROOT_DIR}/deps)
set(APP_ROOT_WOLFRAM_DIR ${APP_ROOT_DEPENDENCIES_DIR}/Wolfram)
include_directories(${APP_ROOT_WOLFRAM_DIR})

add_subdirectory(${APP_ROOT_DEPENDENCIES_DIR}/sleepy-discord)
add_subdirectory(${APP_ROOT_WOLFRAM_DIR})

add_executable(MamadisiBotC main.cpp secrets.h MamadisiBot.h MamadisiBot.cpp ImageDownloader.h ImageDownloader.cpp EquationSolver.h EquationSolver.cpp)

target_link_libraries(MamadisiBotC sleepy-discord)
target_link_libraries(MamadisiBotC Wolfram)
target_link_libraries(MamadisiBotC mariadb)
