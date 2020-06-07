#include <gtest/gtest.h>

#include "2048.h"

TEST(Board, SetValues)
{
    for (size_t y = 0; y < 4; ++y)
    {
        for (size_t x = 0; x < 4; ++x)
        {
            g2048::Board board{};
            uint8_t desVal = 0xF;
            board.set(x, y, desVal);
            EXPECT_EQ(board.at(x, y), desVal)
                << " at position (" << x << "," << y << ") -> " << std::hex << board.raw() << "\n"
                << board;
        }
    }
}

std::array<std::array<uint8_t, 4>, 4> transpose(std::array<std::array<uint8_t, 4>, 4> val)
{
    for (uint8_t x = 0; x < 4; ++x)
    {
        for (uint8_t y = x + 1; y < 4; ++y)
        {
            std::swap(val[x][y], val[y][x]);
        }
    }
    return val;
}

struct TestData
{
    std::array<std::array<uint8_t, 4>, 4> source;
    g2048::Actions action;
    std::array<std::array<uint8_t, 4>, 4> target;
};

class G2048Test : public testing::TestWithParam<TestData>
{
};

TEST_P(G2048Test, Action)
{
    auto params = GetParam();

    g2048::G2048State state(transpose(params.source));
    g2048::G2048Problem problem;
    std::cout << "Before: --------------------\n";
    state.print();
    problem.performAction(params.action, state);
    std::cout << "After: #####################\n";
    state.print();
    ASSERT_EQ(state.board(), transpose(params.target));
}

// clang-format off
INSTANTIATE_TEST_CASE_P(
    , G2048Test,
    testing::ValuesIn({TestData{.source = {{{0, 0, 2, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0}}},
                                .action = g2048::DOWN,
                                .target = {{{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 2, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {2, 2, 0, 0},
                                            {0, 0, 0, 0}}},
                                .action = g2048::RIGHT,
                                .target = {{{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 0, 3},
                                            {0, 0, 0, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {3, 2, 2, 0},
                                            {0, 0, 0, 0}}},
                                .action = g2048::RIGHT,
                                .target = {{{0, 0, 0, 0},
                                            {0, 0, 0, 0},
                                            {0, 0, 3, 3},
                                            {0, 0, 0, 0}}}},
                       TestData{.source = {{{0, 0, 0, 0},
                                            {0, 0, 0, 4},
                                            {2, 0, 4, 2},
                                            {0, 2, 4, 8}}},
                                .action = g2048::RIGHT,
                                .target = {{{0, 0, 0, 0},
                                            {0, 0, 0, 4},
                                            {0, 2, 4, 2},
                                            {0, 2, 4, 8}}}}}));

// clang-format on
