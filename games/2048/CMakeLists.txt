
add_library(g2048 2048.cpp board.cpp board.h)

add_executable(play2048 play_2048.cpp)
target_link_libraries(play2048 g2048)

add_executable(solve2048 solve_2048.cpp policies.cpp)
target_link_libraries(solve2048 g2048)

add_executable(testG2048 test_2048.cpp)
target_link_libraries(testG2048 gtest_main g2048)