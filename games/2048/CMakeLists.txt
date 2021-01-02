
add_library(g2048 2048.cpp board.cpp board.h)
target_link_libraries(g2048 mcts_solver)

add_executable(play2048 play_2048.cpp)
target_link_libraries(play2048 g2048)
target_enable_clang_tidy(play2048)


add_executable(solve2048 solve_2048.cpp policies.cpp)
target_link_libraries(solve2048 g2048)
target_enable_clang_tidy(solve2048)


add_executable(testG2048 test_2048.cpp)
target_link_libraries(testG2048 CONAN_PKG::gtest g2048)
gtest_add_tests(TARGET testG2048)
target_enable_clang_tidy(testG2048)
