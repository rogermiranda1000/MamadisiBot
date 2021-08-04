cmake_minimum_required(VERSION 3.16)
project(MamadisiBotC)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Debug) # Debug (gdb & valgrind)
set(CMAKE_BUILD_TYPE RELEASE)

add_subdirectory(deps/sleepy-discord)

add_library(Wolfram STATIC
	deps/Wolfram/WAEngine.cpp
	deps/Wolfram/WAImage.cpp
	deps/Wolfram/WAPod.cpp
	deps/Wolfram/WAPodState.cpp
	deps/Wolfram/WAQuery.cpp
	deps/Wolfram/WASubpod.cpp
)

add_executable(MamadisiBotC main.cpp secrets.h MamadisiBot.h MamadisiBot.cpp ImageDownloader.h ImageDownloader.cpp EquationSolver.h EquationSolver.cpp)

target_link_libraries(MamadisiBotC sleepy-discord)
target_link_libraries(MamadisiBotC Wolfram)
target_link_libraries(MamadisiBotC mariadb)
