cmake_minimum_required(VERSION 3.14)
project(MCTSSolverV2)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -W -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wall -Werror -W -fPIC -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wall -Werror -W -fPIC -march=native ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Wall -Werror -W -fPIC -march=native ")

add_subdirectory(googletest)
include_directories(.)

add_executable(test_tree test/test_tree.cpp)
target_link_libraries(test_tree gtest_main)

add_executable(test_solver test/test_solver.cpp)
target_link_libraries(test_solver gtest_main)

add_subdirectory(games)