cmake_minimum_required(VERSION 3.14)
project(MCTSSolverV2)

set(CMAKE_CXX_STANDARD 17)



set(CMAKE_CXX_FLAGS "-g -Wall -Werror -W -fPIC -march=native ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wall -Werror -W -fPIC -fsanitize=address")

add_subdirectory(googletest)
include_directories(.)

add_executable(testTicTacToe testTicTacToe.cpp mcts/solver.h games/GameTicTacToe.h mcts/details/solver_impl.h)


add_executable(play2048 games/play2048.cpp games/2048.cpp)
add_executable(solve2048 games/solve2048.cpp games/2048.cpp)


add_executable(testG2048 games/test2048.cpp games/2048.cpp)
target_link_libraries(testG2048 gtest_main)

add_executable(testTree testTree.cpp)
target_link_libraries(testTree gtest_main)

add_executable(test_solver test_solver.cpp)
target_link_libraries(test_solver gtest_main)

