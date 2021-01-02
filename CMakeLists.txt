cmake_minimum_required(VERSION 3.14)
project(MCTSSolverV2)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -W -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -W -fPIC -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -W -fPIC -march=native ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Werror  -W -fPIC -march=native ")

set(ZBO_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(zbo)

add_library(mcts_solver INTERFACE)
target_include_directories(mcts_solver INTERFACE .)
target_link_libraries(mcts_solver INTERFACE max_size_vector named_type)


option(MCTS_BUILD_TESTS "Enable compilation of unit tests" ON)
option(MCTS_BUILD_EXAMPLES "Enable compilation of example games" ON)
enable_testing()
include(GoogleTest)

if (MCTS_BUILD_TESTS)
    conan_cmake_run(
            REQUIRES
            gtest/1.10.0
            OPTIONS
            BASIC_SETUP
            CMAKE_TARGETS
            BUILD
            missing)

    add_executable(test_tree test/test_tree.cpp)
    target_link_libraries(test_tree CONAN_PKG::gtest mcts_solver)
    gtest_add_tests(TARGET test_tree)


    add_executable(test_solver test/test_solver.cpp)
    target_link_libraries(test_solver CONAN_PKG::gtest mcts_solver)
    gtest_add_tests(TARGET test_solver)
endif ()

if (MCTS_BUILD_EXAMPLES)
    add_subdirectory(games)
endif ()

